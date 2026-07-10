# -*- coding: utf-8 -*-
"""实时推理服务模块。


1. `FrameProcessorThread` 将 Qt 摄像头帧转换为 OpenCV BGR 图像；
2. `MediaPipePoseHandService.process_bgr()` 接收 BGR 图像并转为 MediaPipe 需要的 RGB；
3. MediaPipe Pose 输出 `pose_world_landmarks` 与 `pose_landmarks`；
4. 使用 world landmarks 计算左右手臂各 6 个方向角；
5. 使用 image landmarks 把骨架和角度文字绘制回画面；
6. 将角度、电机 6 命令和识别状态封装为 payload，交回 UI/通信模块。

注意：
- 这里的 pitch/yaw 表示骨骼向量在全局 XZ 平面上的方向角；
- 这些角度不是严格的生理解剖关节角，但适合当前“姿态方向 -> 电机控制”链路；
- OpenCV 默认图像为 BGR，MediaPipe 输入为 RGB，因此入口处必须做色彩空间转换。
"""

from __future__ import annotations

from dataclasses import asdict
from typing import Iterable

import numpy as np

from app.models import ArmAngles, Direction, JointRotation, Motor6Command, Side
from solver.motor_mapping import iter_motor_mappings


_X_GLOBAL = np.array([1.0, 0.0, 0.0], dtype=np.float32)
_Y_GLOBAL = np.array([0.0, 1.0, 0.0], dtype=np.float32)
_Z_GLOBAL = np.array([0.0, 0.0, 1.0], dtype=np.float32)


def _build_angle_motor_labels() -> dict[str, str]:
    """根据映射表生成角度字段对应的电机标签。

    输入：无，读取 `solver.motor_mapping.iter_motor_mappings()`。
    输出：`{angle_field: "M1/M2"}` 形式的字典。

    功能：让画面叠字只依赖映射表，不在推理绘制逻辑里硬编码电机编号。
    """
    grouped: dict[str, list[int]] = {}
    for mapping in iter_motor_mappings():
        if mapping.source != "angles":
            continue
        grouped.setdefault(mapping.angle_field, []).append(mapping.motor_id)
    return {
        field: "/".join(f"M{motor_id}" for motor_id in sorted(motor_ids))
        for field, motor_ids in grouped.items()
    }


class MediaPipePoseHandService:
    """MediaPipe Pose 实时推理服务。

    输入：
    - OpenCV BGR 格式图像帧。

    输出：
    - 标注后的 BGR 图像；
    - 与 UI/通信模块兼容的 payload 字典。

    实现：
    - 初始化时创建一个长生命周期的 `mp.solutions.pose.Pose`；
    - 每帧只调用 `process_bgr()`，避免重复加载模型影响实时性。
    """

    def __init__(
        self,
        min_detection_confidence: float = 0.5,
        min_tracking_confidence: float = 0.5,
    ):
        import cv2
        import mediapipe as mp

        # 延迟导入 cv2/mediapipe：让模块导入更轻，错误也能在线程里回传给 UI。
        self._cv2 = cv2
        self._mp = mp
        self._mp_pose = mp.solutions.pose
        self._drawing = mp.solutions.drawing_utils

        # 关键点枚举表：保留 video_inf.py 中的命名方式，便于和原脚本对照。
        self._kp = {
            "LEFT_SHOULDER": self._mp_pose.PoseLandmark.LEFT_SHOULDER,
            "RIGHT_SHOULDER": self._mp_pose.PoseLandmark.RIGHT_SHOULDER,
            "LEFT_ELBOW": self._mp_pose.PoseLandmark.LEFT_ELBOW,
            "RIGHT_ELBOW": self._mp_pose.PoseLandmark.RIGHT_ELBOW,
            "LEFT_WRIST": self._mp_pose.PoseLandmark.LEFT_WRIST,
            "RIGHT_WRIST": self._mp_pose.PoseLandmark.RIGHT_WRIST,
            "LEFT_HIP": self._mp_pose.PoseLandmark.LEFT_HIP,
            "RIGHT_HIP": self._mp_pose.PoseLandmark.RIGHT_HIP,
            "RIGHT_THUMB": self._mp_pose.PoseLandmark.RIGHT_THUMB,
            "RIGHT_PINKY": self._mp_pose.PoseLandmark.RIGHT_PINKY,
            "RIGHT_INDEX": self._mp_pose.PoseLandmark.RIGHT_INDEX,
            "LEFT_THUMB": self._mp_pose.PoseLandmark.LEFT_THUMB,
            "LEFT_PINKY": self._mp_pose.PoseLandmark.LEFT_PINKY,
            "LEFT_INDEX": self._mp_pose.PoseLandmark.LEFT_INDEX,
        }
        self._angle_motor_labels = _build_angle_motor_labels()

        self._pose = self._mp_pose.Pose(
            # 视频模式：MediaPipe 会利用上一帧跟踪结果，实时性优于静态图模式。
            static_image_mode=False,
            min_detection_confidence=min_detection_confidence,
            min_tracking_confidence=min_tracking_confidence,
            # 当前只需要人体骨架，不需要分割 mask，关闭可减少计算量。
            enable_segmentation=False,
        )

    def close(self) -> None:
        """释放 MediaPipe Pose 资源。

        输入：无。
        输出：无。
        功能：窗口关闭或线程退出时调用，避免底层模型资源泄漏。
        """
        if self._pose is not None:
            self._pose.close()
            self._pose = None

    def process_bgr(self, frame_bgr: np.ndarray) -> tuple[np.ndarray, dict]:
        """处理一帧 BGR 图像并返回标注图与推理结果。

        输入：
        - frame_bgr：OpenCV BGR 图像，shape 为 (H, W, 3)，dtype 通常为 uint8。

        输出：
        - annotated：绘制骨架和角度文字后的 BGR 图像；
        - payload：包含 angles、motor_6、motor_commands、has_pose 等字段的字典。

        功能实现：
        - BGR -> RGB 后调用 MediaPipe Pose；
        - 使用 world landmarks 计算 6 维角度；
        - 使用 image landmarks 做可视化和可见性判断；
        - 按项目既有 payload 协议封装结果。
        """
        # MediaPipe 只接受 RGB；Qt/worker 传入的是 OpenCV BGR。
        frame_rgb = self._cv2.cvtColor(frame_bgr, self._cv2.COLOR_BGR2RGB)
        # 核心推理调用：得到归一化图像点和三维 world 点。
        results = self._pose.process(frame_rgb)
        # 不直接修改输入帧，避免上游缓存或调试时被副作用影响。
        annotated = frame_bgr.copy()

        if not results.pose_world_landmarks or not results.pose_landmarks:
            # 没识别到人体时仍返回合法 payload，让 UI/通信保持稳定。
            return annotated, self._empty_payload("pose_missing")

        # world_lms 用于角度计算；img_lms 用于绘图和可见性判断。
        world_lms = results.pose_world_landmarks.landmark
        img_lms = results.pose_landmarks.landmark

        # 分别计算左右臂：[肩pitch, 肩yaw, 肘pitch, 肘yaw, 腕pitch, 腕yaw]。
        left_angles = self._calc_arm_full(world_lms, "LEFT")
        right_angles = self._calc_arm_full(world_lms, "RIGHT")

        # 把骨架和角度文字画回预览画面。
        self._draw(annotated, results.pose_landmarks, img_lms, left_angles, right_angles)

        # Pose 模型没有独立手部检测，这里用腕/拇指/小指/食指关键点可见性近似判断手部有效。
        has_left_hand = self._landmarks_visible(
            img_lms, ("LEFT_WRIST", "LEFT_THUMB", "LEFT_PINKY", "LEFT_INDEX")
        )
        has_right_hand = self._landmarks_visible(
            img_lms, ("RIGHT_WRIST", "RIGHT_THUMB", "RIGHT_PINKY", "RIGHT_INDEX")
        )

        angles_payload = [
            asdict(self._build_arm_angles(Side.LEFT, left_angles)),
            asdict(self._build_arm_angles(Side.RIGHT, right_angles)),
        ]
        # 6 号电机沿用项目原有字段：右手腕左右翻转角。
        motor_6_payload = asdict(self._build_motor_6(right_angles, has_right_hand, img_lms))

        return annotated, {
            "source": "video_inf_pose",
            "angles": angles_payload,
            "motor_6": motor_6_payload,
            "motor_commands": [motor_6_payload] if motor_6_payload.get("valid") else [],
            "control_stage": "video_inf_pose_6d",
            "has_pose": True,
            "has_face": False,
            "has_left_hand": has_left_hand,
            "has_right_hand": has_right_hand,
            "pose_valid": has_left_hand or has_right_hand,
        }

    def _calc_arm_full(self, world_lms, side: str) -> np.ndarray:
        """计算单侧手臂 6 维方向角。

        输入：
        - world_lms：MediaPipe Pose 的 `pose_world_landmarks.landmark`；
        - side：`LEFT` 或 `RIGHT`，用于选择左右侧关键点。

        输出：
        - numpy 数组 `[肩pitch, 肩yaw, 肘pitch, 肘yaw, 腕pitch, 腕yaw]`。

        功能实现：
        - 肩部角度来自“肩 -> 肘”向量；
        - 肘部角度来自“肘 -> 腕”向量；
        - 腕部角度来自“手部三点中心 -> 腕”向量。
        """
        # 读取肩、肘、腕和三个手部参考点的 world 坐标。
        shoulder = self._world_lm(world_lms, f"{side}_SHOULDER")
        elbow = self._world_lm(world_lms, f"{side}_ELBOW")
        wrist = self._world_lm(world_lms, f"{side}_WRIST")
        thumb = self._world_lm(world_lms, f"{side}_THUMB")
        pinky = self._world_lm(world_lms, f"{side}_PINKY")
        index = self._world_lm(world_lms, f"{side}_INDEX")

        # 上臂向量、前臂向量、手腕参考向量均继承 video_inf.py 的算法。
        shoulder_to_elbow = elbow - shoulder
        elbow_to_wrist = wrist - elbow
        hand_mid = (thumb + pinky + index) / 3.0
        hand_mid_to_wrist = wrist - hand_mid

        # 每根向量投影到 XZ 平面，再分别计算相对 Z/X 轴的 pitch/yaw。
        shoulder_pitch, shoulder_yaw = self._calc_vector_2d_angle(shoulder_to_elbow)
        elbow_pitch, elbow_yaw = self._calc_vector_2d_angle(elbow_to_wrist)
        wrist_pitch, wrist_yaw = self._calc_vector_2d_angle(hand_mid_to_wrist)

        return np.round(
            [shoulder_pitch, shoulder_yaw, elbow_pitch, elbow_yaw, wrist_pitch, wrist_yaw],
            2,
        )

    def _world_lm(self, world_lms, name: str) -> np.ndarray:
        """按关键点名称读取 MediaPipe world landmark。

        输入：
        - world_lms：MediaPipe world landmark 列表；
        - name：`self._kp` 中的关键点名称。

        输出：
        - `np.ndarray([x, y, z], dtype=float32)`。
        """
        lm = world_lms[self._kp[name].value]
        return np.array([lm.x, lm.y, lm.z], dtype=np.float32)

    @staticmethod
    def _calc_vector_2d_angle(vec: np.ndarray) -> np.ndarray:
        """计算向量在 XZ 平面上的 pitch/yaw。

        输入：
        - vec：任意三维方向向量。

        输出：
        - `np.ndarray([pitch, yaw])`，单位为度。

        功能实现：
        - 先归一化向量；
        - 去掉 Y 分量得到 XZ 平面投影；
        - pitch 为投影向量与 Z 轴夹角；
        - yaw 为投影向量与 X 轴夹角。
        """
        vec_unit = _vec3_norm(vec)
        # 删除 Y 轴分量，相当于把向量投影到全局 XZ 平面。
        vec_xz = vec_unit - float(np.dot(vec_unit, _Y_GLOBAL)) * _Y_GLOBAL
        if np.linalg.norm(vec_xz) < 1e-6:
            # 向量几乎垂直于 XZ 平面时，投影不稳定，返回 0 避免抖动和 NaN。
            return np.array([0.0, 0.0], dtype=np.float32)

        vec_xz_unit = _vec3_norm(vec_xz)
        pitch = _angle_abs(vec_xz_unit, _Z_GLOBAL)
        yaw = _angle_abs(vec_xz_unit, _X_GLOBAL)
        return np.array([pitch, yaw], dtype=np.float32)

    def _build_arm_angles(self, side: Side, values: np.ndarray) -> ArmAngles:
        """把 6 维角度数组转换为项目统一的 ArmAngles 数据模型。

        输入：
        - side：左臂或右臂；
        - values：`_calc_arm_full()` 输出的 6 维角度。

        输出：
        - `ArmAngles` dataclass，用于 UI 显示、过滤和通信。
        """
        shoulder_pitch, shoulder_yaw, elbow_pitch, elbow_yaw, wrist_pitch, wrist_yaw = [
            _round_deg(v) for v in values
        ]

        return ArmAngles(
            side=side,
            shoulder_deg=shoulder_pitch,
            elbow_deg=elbow_pitch,
            wrist_deg=wrist_pitch,
            vertical=Direction.UP,
            horizontal=Direction.RIGHT,
            shoulder_rotation=JointRotation(
                vertical=Direction.UP,
                vertical_deg=shoulder_pitch,
                horizontal=Direction.RIGHT,
                horizontal_deg=shoulder_yaw,
            ),
            arm_rotation=JointRotation(
                vertical=Direction.UP,
                vertical_deg=elbow_pitch,
                horizontal=Direction.RIGHT,
                horizontal_deg=elbow_yaw,
            ),
            wrist_rotation=JointRotation(
                vertical=Direction.UP,
                vertical_deg=wrist_pitch,
                horizontal=Direction.RIGHT,
                horizontal_deg=wrist_yaw,
            ),
            shoulder_flex_deg=shoulder_pitch,
            shoulder_abduction_deg=shoulder_yaw,
            elbow_flex_deg=elbow_pitch,
            elbow_abduction_deg=elbow_yaw,
            wrist_flex_deg=wrist_pitch,
        )

    def _build_motor_6(self, right_angles: np.ndarray, has_right_hand: bool, img_lms) -> Motor6Command:
        """根据右手腕 yaw 角生成 6 号电机命令。

        输入：
        - right_angles：右臂 6 维角度数组；
        - has_right_hand：右手关键点是否可见；
        - img_lms：MediaPipe 图像关键点，用于计算置信度。

        输出：
        - `Motor6Command`，字段保持和原控制链路兼容。

        功能实现：
        - 以 `wrist_yaw = 90°` 作为居中参考；
        - 大于 90° 视为右旋，小于 90° 视为左旋；
        - `signed_roll_deg` 保留符号，`roll_deg` 存绝对角度。
        """
        if not has_right_hand:
            return Motor6Command(reason="right_hand_missing")

        wrist_pitch = _round_deg(right_angles[4])
        wrist_yaw = _round_deg(right_angles[5])
        # 原 video_inf.py 输出 yaw 为 0~180 的无符号夹角，这里平移到 -90~90 控制区间。
        signed_roll = _round_deg(wrist_yaw - 90.0)
        direction = Direction.RIGHT if signed_roll > 0.0 else Direction.LEFT
        if abs(signed_roll) < 1e-6:
            direction = Direction.UNKNOWN

        return Motor6Command(
            valid=True,
            mode="video_inf_pose_wrist_yaw",
            reference="pose_wrist_yaw_90deg_center",
            roll_deg=abs(signed_roll),
            signed_roll_deg=signed_roll,
            raw_signed_roll_deg=signed_roll,
            direction=direction,
            thumb_direction=direction,
            rotation_side="neutral" if direction == Direction.UNKNOWN else "outward",
            wrist_pitch_deg=wrist_pitch,
            confidence=self._right_hand_confidence(img_lms),
            motion_allowed=True,
            reason="ok",
        )

    def _draw(self, image_bgr, pose_landmarks, img_lms, left_angles: np.ndarray, right_angles: np.ndarray) -> None:
        """把骨架和左右臂 6 维角度绘制到 BGR 图像上。

        输入：
        - image_bgr：待绘制的 OpenCV BGR 图像；
        - pose_landmarks：MediaPipe 图像坐标关键点；
        - img_lms：关键点列表，用于文字定位；
        - left_angles/right_angles：左右臂 6 维角度数组。

        输出：无，直接修改 `image_bgr`。
        """
        # 绘制 MediaPipe Pose 骨架连线。
        self._drawing.draw_landmarks(
            image_bgr,
            pose_landmarks,
            self._mp_pose.POSE_CONNECTIONS,
            self._drawing.DrawingSpec((0, 0, 255), 2),
            self._drawing.DrawingSpec((0, 255, 0), 2),
        )

        height, width = image_bgr.shape[:2]
        # 左侧文字靠关键点右侧显示，右侧文字靠关键点左侧显示，减少遮挡身体。
        self._draw_arm_text(image_bgr, img_lms, width, height, "LEFT", left_angles, (0, 255, 255), align_left=True)
        self._draw_arm_text(image_bgr, img_lms, width, height, "RIGHT", right_angles, (255, 0, 0), align_left=False)

    def _draw_arm_text(
        self,
        image_bgr,
        img_lms,
        width: int,
        height: int,
        side: str,
        values: np.ndarray,
        color: tuple[int, int, int],
        *,
        align_left: bool,
    ) -> None:
        """在肩、肘、腕附近绘制 pitch/yaw 文本。

        输入：
        - image_bgr：待绘制图像；
        - img_lms：MediaPipe 图像坐标关键点列表；
        - width/height：图像尺寸；
        - side：`LEFT` 或 `RIGHT`；
        - values：该侧 6 维角度；
        - color：OpenCV BGR 颜色；
        - align_left：True 时文字向右展开，False 时文字向左展开。

        输出：无，直接修改 `image_bgr`。

        说明：
        - 电机编号来自 `solver.motor_mapping`，映射表变更后画面叠字同步变化；
        - OpenCV Hershey 字体不稳定支持中文，因此画面叠字保留英文缩写；
        - 代码注释与文档统一使用中文 UTF-8。
        """
        label_sets = (
            (
                f"{side}_SHOULDER",
                ("shoulder_flex_deg", "Sh Pitch", values[0]),
                ("shoulder_abduction_deg", "Sh Yaw", values[1]),
            ),
            (
                f"{side}_ELBOW",
                ("elbow_flex_deg", "El Pitch", values[2]),
                ("elbow_abduction_deg", "El Yaw", values[3]),
            ),
            (
                f"{side}_WRIST",
                ("wrist_flex_deg", "W Pitch", values[4]),
                ("wrist_rotation.horizontal_deg", "W Yaw", values[5]),
            ),
        )
        for landmark_name, line1, line2 in label_sets:
            # 文本锚点取对应关节的归一化图像坐标，再换算为像素坐标。
            lm = img_lms[self._kp[landmark_name].value]
            x_base = int(np.clip(lm.x, 0.0, 1.0) * (width - 1))
            y_base = int(np.clip(lm.y, 0.0, 1.0) * (height - 1))
            for row, (field, label, value) in enumerate((line1, line2)):
                motor_label = self._angle_motor_labels.get(field, "")
                label_text = f"{motor_label} {label}" if motor_label else label
                text = f"{label_text}:{float(value):.1f}deg"
                text_width = self._cv2.getTextSize(text, self._cv2.FONT_HERSHEY_SIMPLEX, 0.6, 1)[0][0]
                if align_left:
                    x = min(x_base + 8, max(0, width - text_width - 2))
                else:
                    # 右侧身体的文字向左展开，避免跑出画面右边缘。
                    x = x_base - text_width - 8
                y = y_base + row * 20
                self._cv2.putText(
                    image_bgr,
                    text,
                    (max(0, x), max(18, y)),
                    self._cv2.FONT_HERSHEY_SIMPLEX,
                    0.6,
                    color,
                    1,
                    self._cv2.LINE_AA,
                )

    def _landmarks_visible(self, img_lms, names: Iterable[str], threshold: float = 0.2) -> bool:
        """判断一组图像关键点是否达到可见性阈值。

        输入：
        - img_lms：MediaPipe 图像关键点列表；
        - names：待检查的关键点名称集合；
        - threshold：最低 visibility 阈值。

        输出：
        - True 表示所有关键点都足够可见，否则 False。
        """
        for name in names:
            lm = img_lms[self._kp[name].value]
            if getattr(lm, "visibility", 1.0) < threshold:
                return False
        return True

    def _right_hand_confidence(self, img_lms) -> float:
        """计算右手腕/手指关键点的保守置信度。

        输入：
        - img_lms：MediaPipe 图像关键点列表。

        输出：
        - 四个右手相关关键点 visibility 的最小值，四舍五入到 2 位小数。
        """
        names = ("RIGHT_WRIST", "RIGHT_THUMB", "RIGHT_PINKY", "RIGHT_INDEX")
        values = [float(getattr(img_lms[self._kp[name].value], "visibility", 1.0)) for name in names]
        return _round_deg(min(values))

    @staticmethod
    def _empty_payload(reason: str) -> dict:
        """构造无检测结果时的空 payload。

        输入：
        - reason：无效原因，例如 `pose_missing`。

        输出：
        - 与正常推理结果结构一致的字典，便于 UI/过滤/通信模块统一处理。
        """
        motor_6_payload = asdict(Motor6Command(reason=reason))
        return {
            "source": "video_inf_pose",
            "angles": [],
            "motor_6": motor_6_payload,
            "motor_commands": [],
            "control_stage": "video_inf_pose_6d",
            "has_pose": False,
            "has_face": False,
            "has_left_hand": False,
            "has_right_hand": False,
            "pose_valid": False,
        }


def _vec3_norm(vec: np.ndarray) -> np.ndarray:
    """向量归一化。

    输入：三维向量。
    输出：单位向量；若向量长度过小，返回同形状零向量。
    """
    norm = np.linalg.norm(vec)
    return vec / norm if norm > 1e-6 else np.zeros_like(vec)


def _angle_abs(v1: np.ndarray, v2: np.ndarray) -> float:
    """计算两个向量的无符号夹角。

    输入：两个三维向量。
    输出：夹角，单位为度，范围约为 0~180。
    """
    v1_unit = _vec3_norm(v1)
    v2_unit = _vec3_norm(v2)
    # clip 防止浮点误差让 arccos 输入略微超过 [-1, 1]。
    dot = float(np.clip(np.dot(v1_unit, v2_unit), -1.0, 1.0))
    return float(np.round(np.rad2deg(np.arccos(dot)), 2))


def _round_deg(value) -> float:
    """角度统一保留 2 位小数，便于显示和通信。"""
    return round(float(value), 2)
