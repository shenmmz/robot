# -*- coding: utf-8 -*-
"""实时推理服务模块。


1. `FrameProcessorThread` 将 Qt 摄像头帧转换为 OpenCV BGR 图像；
2. `MediaPipePoseHandService.process_bgr()` 接收 BGR 图像并转为 MediaPipe 需要的 RGB；
3. MediaPipe Pose 输出 `pose_world_landmarks` 与 `pose_landmarks`；
4. 使用 world landmarks 计算左右手臂各 6 个方向角；
5. 使用 image landmarks 把骨架和角度文字绘制回画面；
6. 将角度、电机 6 命令和识别状态封装为 payload，交回 UI/通信模块。

注意：
- 这里的 pitch/yaw 是以“朝摄像机前伸且平行地面”为零位的有符号方向角；
- pitch 上抬为正、下压为负，yaw 向画面右侧为正、左侧为负；
- 这些角度不是严格的生理解剖关节角，但适合当前“姿态方向 -> 电机控制”链路；
- OpenCV 默认图像为 BGR，MediaPipe 输入为 RGB，因此入口处必须做色彩空间转换。
"""

from __future__ import annotations

from dataclasses import asdict
from typing import Iterable

import numpy as np

from app.models import ArmAngles, Direction, JointRotation, Motor6Command, Point3D, Side
from solver.motor_mapping import iter_motor_mappings
from solver.wrist import PalmRollTracker


# MediaPipe world 坐标沿用图像观察方向：X 向画面右侧、Y 向下、负 Z 朝向摄像机。
# 因此“面向摄像机、手臂水平前伸”的零位方向为 -Z。
_RIGHT_GLOBAL = np.array([1.0, 0.0, 0.0], dtype=np.float32)
_UP_GLOBAL = np.array([0.0, -1.0, 0.0], dtype=np.float32)
_FORWARD_GLOBAL = np.array([0.0, 0.0, -1.0], dtype=np.float32)
_ANGLE_LIMIT_DEG = 90.0
_HAND_MATCH_MAX_DISTANCE = 0.25


def _build_angle_motor_labels() -> dict[str, str]:
    """根据映射表生成角度字段对应的电机标签。

    输入：无，读取 `solver.motor_mapping.iter_motor_mappings()`。
    输出：`{angle_field: "M1/M2"}` 形式的字典。

    功能：让画面叠字只依赖映射表，不在推理绘制逻辑里硬编码电机编号。
    """
    grouped: dict[str, list[int]] = {}
    for mapping in iter_motor_mappings():
        if mapping.source == "motor_6" and mapping.angle_field == "signed_roll_deg":
            grouped.setdefault("wrist_rotation.horizontal_deg", []).append(mapping.motor_id)
            continue
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
    - 初始化时创建长生命周期的 `mp.solutions.pose.Pose` 和 `mp.solutions.hands.Hands`；
    - 每帧只调用 `process_bgr()`，避免重复加载模型影响实时性。
    """

    def __init__(
        self,
        min_detection_confidence: float = 0.5,
        min_tracking_confidence: float = 0.5,
    ):
        """初始化 Pose、Hands、绘图工具和左右掌面跟踪器。

        输入：
        - min_detection_confidence：Pose/Hands 首次检测最低置信度，范围 0~1；
        - min_tracking_confidence：视频模式下跨帧跟踪最低置信度，范围 0~1。

        输出：无。

        功能：模型对象在整个采集周期内复用，避免每帧重新加载；左右手各自维护
        `PalmRollTracker`，防止一只手的法向量历史污染另一只手。
        """
        import cv2
        import mediapipe as mp

        # 延迟导入 cv2/mediapipe：让模块导入更轻，错误也能在线程里回传给 UI。
        self._cv2 = cv2
        self._mp = mp
        self._mp_pose = mp.solutions.pose
        self._mp_hands = mp.solutions.hands
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
        self._hands = self._mp_hands.Hands(
            static_image_mode=False,
            max_num_hands=2,
            model_complexity=1,
            min_detection_confidence=min_detection_confidence,
            min_tracking_confidence=min_tracking_confidence,
        )
        self._palm_roll_trackers = {
            Side.LEFT: PalmRollTracker(Side.LEFT),
            Side.RIGHT: PalmRollTracker(Side.RIGHT),
        }

    def close(self) -> None:
        """释放 MediaPipe Pose/Hands 资源。

        输入：无。
        输出：无。
        功能：窗口关闭或线程退出时调用，避免底层模型资源泄漏。
        """
        if self._pose is not None:
            self._pose.close()
            self._pose = None
        if self._hands is not None:
            self._hands.close()
            self._hands = None
        for tracker in self._palm_roll_trackers.values():
            tracker.reset()

    def process_bgr(self, frame_bgr: np.ndarray) -> tuple[np.ndarray, dict]:
        """处理一帧 BGR 图像并返回标注图与推理结果。

        输入：
        - frame_bgr：OpenCV BGR 图像，shape 为 (H, W, 3)，dtype 通常为 uint8。

        输出：
        - annotated：绘制骨架和角度文字后的 BGR 图像；
        - payload：包含 angles、motor_6、motor_commands、has_pose 等字段的字典。

        功能实现：
        - BGR -> RGB 后调用 MediaPipe Pose 和 Hands；
        - 使用 Pose world landmarks 计算肩/肘/腕俯仰角；
        - 使用 Hands world landmarks 拟合掌面并计算手腕 Roll；
        - 使用 image landmarks 做可视化和左右手匹配；
        - 按项目既有 payload 协议封装结果。
        """
        # MediaPipe 只接受 RGB；Qt/worker 传入的是 OpenCV BGR。
        frame_rgb = self._cv2.cvtColor(frame_bgr, self._cv2.COLOR_BGR2RGB)
        # Pose 提供身体与前臂，Hands 提供完整的 21 点手掌几何。
        frame_rgb.flags.writeable = False
        results = self._pose.process(frame_rgb)
        hands_results = self._hands.process(frame_rgb)
        frame_rgb.flags.writeable = True
        # 不直接修改输入帧，避免上游缓存或调试时被副作用影响。
        annotated = frame_bgr.copy()

        if not results.pose_world_landmarks or not results.pose_landmarks:
            # 没识别到人体时仍返回合法 payload，让 UI/通信保持稳定。
            return annotated, self._empty_payload("pose_missing")

        # world_lms 用于角度计算；img_lms 用于绘图和可见性判断。
        world_lms = results.pose_world_landmarks.landmark
        img_lms = results.pose_landmarks.landmark
        hand_matches = self._match_hands_to_pose(img_lms, hands_results)

        # 分别计算左右臂：[肩pitch, 肩yaw, 肘pitch, 肘yaw, 腕pitch, 腕roll]。
        left_angles = self._calc_arm_full(world_lms, "LEFT")
        right_angles = self._calc_arm_full(world_lms, "RIGHT")

        left_roll = self._track_palm_roll(
            Side.LEFT,
            hand_matches.get("LEFT"),
            hands_results,
            world_lms,
            wrist_pitch_deg=float(left_angles[4]),
        )
        right_roll = self._track_palm_roll(
            Side.RIGHT,
            hand_matches.get("RIGHT"),
            hands_results,
            world_lms,
            wrist_pitch_deg=float(right_angles[4]),
        )

        # 保留原 Pose 手部可见性语义，避免 Hands 短暂丢失时阻塞 M1-M5。
        has_left_hand = self._landmarks_visible(
            img_lms, ("LEFT_WRIST", "LEFT_THUMB", "LEFT_PINKY", "LEFT_INDEX")
        )
        has_right_hand = self._landmarks_visible(
            img_lms, ("RIGHT_WRIST", "RIGHT_THUMB", "RIGHT_PINKY", "RIGHT_INDEX")
        )
        left_angles[5] = _command_roll_or_zero(left_roll)
        right_angles[5] = _command_roll_or_zero(right_roll)

        # 把骨架和角度文字画回预览画面。
        self._draw(
            annotated,
            results.pose_landmarks,
            img_lms,
            left_angles,
            right_angles,
            hands_results,
            hand_matches,
            {"LEFT": left_roll, "RIGHT": right_roll},
        )

        angles_payload = [
            asdict(self._build_arm_angles(Side.LEFT, left_angles)),
            asdict(self._build_arm_angles(Side.RIGHT, right_angles)),
        ]
        motor_6_payload = asdict(right_roll)

        return annotated, {
            "source": "video_inf_pose_hands",
            "angles": angles_payload,
            "motor_6": motor_6_payload,
            "motor_commands": [motor_6_payload] if motor_6_payload.get("valid") else [],
            "control_stage": "video_inf_pose_hands_6d",
            "has_pose": True,
            "has_face": False,
            "has_left_hand": has_left_hand,
            "has_right_hand": has_right_hand,
            "has_left_palm": bool(left_roll.valid),
            "has_right_palm": bool(right_roll.valid),
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
        - 腕部角度来自“腕 -> 手部三点中心”向量。
        """
        # 读取肩、肘、腕和三个手部参考点的 world 坐标。
        shoulder = self._world_lm(world_lms, f"{side}_SHOULDER")
        elbow = self._world_lm(world_lms, f"{side}_ELBOW")
        wrist = self._world_lm(world_lms, f"{side}_WRIST")
        thumb = self._world_lm(world_lms, f"{side}_THUMB")
        pinky = self._world_lm(world_lms, f"{side}_PINKY")
        index = self._world_lm(world_lms, f"{side}_INDEX")

        # 三根向量都沿肢体向外，保证水平前伸时方向一致并对应 0° 零位。
        shoulder_to_elbow = elbow - shoulder
        elbow_to_wrist = wrist - elbow
        hand_mid = (thumb + pinky + index) / 3.0
        wrist_to_hand_mid = hand_mid - wrist

        # 以 -Z 前伸、-Y 向上、+X 向右为基准计算连续的有符号角度。
        shoulder_pitch, shoulder_yaw = self._calc_vector_2d_angle(shoulder_to_elbow)
        elbow_pitch, elbow_yaw = self._calc_vector_2d_angle(elbow_to_wrist)
        wrist_pitch, wrist_yaw = self._calc_vector_2d_angle(wrist_to_hand_mid)

        return np.round(
            [
                shoulder_pitch,
                shoulder_yaw,
                elbow_pitch,
                elbow_yaw,
                wrist_pitch,
                wrist_yaw,
            ],
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
        """计算以前伸水平为零位的有符号 pitch/yaw。

        输入：
        - vec：任意三维方向向量。

        输出：
        - `np.ndarray([pitch, yaw])`，单位为度，范围均为 `[-90, 90]`。

        功能实现：
        - 先归一化向量；
        - pitch 是向量相对水平面的仰角，上抬为正、下压为负；
        - yaw 是水平投影相对前伸方向（-Z）的方位角，右偏为正、左偏为负；
        - 对异常或人体不可达的背向角连续钳制到 `[-90, 90]`，不再做角度折返。
        """
        vec_unit = _vec3_norm(vec)
        if np.linalg.norm(vec_unit) < 1e-6:
            return np.array([0.0, 0.0], dtype=np.float32)

        right = float(np.dot(vec_unit, _RIGHT_GLOBAL))
        up = float(np.dot(vec_unit, _UP_GLOBAL))
        forward = float(np.dot(vec_unit, _FORWARD_GLOBAL))
        horizontal_length = float(np.hypot(right, forward))

        pitch = float(np.degrees(np.arctan2(up, horizontal_length)))
        # 垂直向量没有可靠的水平方向，约定 yaw 为 0，避免噪声放大。
        raw_yaw = 0.0 if horizontal_length < 1e-6 else float(np.degrees(np.arctan2(right, forward)))

        return np.array(
            [
                np.clip(pitch, -_ANGLE_LIMIT_DEG, _ANGLE_LIMIT_DEG),
                np.clip(raw_yaw, -_ANGLE_LIMIT_DEG, _ANGLE_LIMIT_DEG),
            ],
            dtype=np.float32,
        )

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
        shoulder_vertical = _direction_from_signed_angle(shoulder_pitch, Direction.UP, Direction.DOWN)
        shoulder_horizontal = _direction_from_signed_angle(shoulder_yaw, Direction.RIGHT, Direction.LEFT)
        elbow_vertical = _direction_from_signed_angle(elbow_pitch, Direction.UP, Direction.DOWN)
        elbow_horizontal = _direction_from_signed_angle(elbow_yaw, Direction.RIGHT, Direction.LEFT)
        wrist_vertical = _direction_from_signed_angle(wrist_pitch, Direction.UP, Direction.DOWN)
        wrist_horizontal = _direction_from_signed_angle(wrist_yaw, Direction.RIGHT, Direction.LEFT)

        return ArmAngles(
            side=side,
            shoulder_deg=shoulder_pitch,
            elbow_deg=elbow_pitch,
            wrist_deg=wrist_pitch,
            vertical=shoulder_vertical,
            horizontal=shoulder_horizontal,
            shoulder_rotation=JointRotation(
                vertical=shoulder_vertical,
                vertical_deg=shoulder_pitch,
                horizontal=shoulder_horizontal,
                horizontal_deg=shoulder_yaw,
            ),
            arm_rotation=JointRotation(
                vertical=elbow_vertical,
                vertical_deg=elbow_pitch,
                horizontal=elbow_horizontal,
                horizontal_deg=elbow_yaw,
            ),
            wrist_rotation=JointRotation(
                vertical=wrist_vertical,
                vertical_deg=wrist_pitch,
                horizontal=wrist_horizontal,
                horizontal_deg=wrist_yaw,
            ),
            shoulder_flex_deg=shoulder_pitch,
            shoulder_abduction_deg=shoulder_yaw,
            elbow_flex_deg=elbow_pitch,
            elbow_abduction_deg=elbow_yaw,
            wrist_flex_deg=wrist_pitch,
        )

    def _match_hands_to_pose(self, img_lms, hands_results) -> dict[str, tuple[int, float]]:
        """按腕点距离把 Hands 检测结果匹配到 Pose 左右手。

        输入：
        - img_lms：Pose 的归一化图像关键点列表，用于读取左右腕位置；
        - hands_results：Hands 当前帧结果，包含图像关键点、三维关键点和 handedness。

        输出：
        - `{\"LEFT\": (hand_index, confidence), \"RIGHT\": (...)}`；
        - `hand_index` 是 Hands 多手结果下标，`confidence` 是 0~1 的融合置信度；
        - 未找到距离足够近的手时，对应侧不出现在字典中。

        逻辑：
        1. 计算每个 Hands 腕点到 Pose 左右腕的二维归一化距离；
        2. 丢弃距离超过 `_HAND_MATCH_MAX_DISTANCE` 的候选；
        3. 按距离从小到大做一对一贪心匹配，保证同一 Hands 结果不会分给两侧；
        4. 将 handedness 检测分数和腕点距离分数按 0.6/0.4 融合。

        说明：左右侧以 Pose 腕点为准，不直接相信 handedness 标签，避免非镜像输入导致
        MediaPipe 左右手标签与实际画面方向相反。
        """
        image_hands = list(getattr(hands_results, "multi_hand_landmarks", None) or [])
        world_hands = list(getattr(hands_results, "multi_hand_world_landmarks", None) or [])
        hand_count = min(len(image_hands), len(world_hands))
        if hand_count == 0:
            return {}

        pose_wrists = {
            "LEFT": img_lms[self._kp["LEFT_WRIST"].value],
            "RIGHT": img_lms[self._kp["RIGHT_WRIST"].value],
        }
        pairs: list[tuple[float, str, int]] = []
        wrist_index = self._mp_hands.HandLandmark.WRIST.value
        for side, pose_wrist in pose_wrists.items():
            for hand_index in range(hand_count):
                hand_wrist = image_hands[hand_index].landmark[wrist_index]
                distance = float(np.hypot(hand_wrist.x - pose_wrist.x, hand_wrist.y - pose_wrist.y))
                if distance <= _HAND_MATCH_MAX_DISTANCE:
                    pairs.append((distance, side, hand_index))

        matches: dict[str, tuple[int, float]] = {}
        used_hands: set[int] = set()
        handedness = list(getattr(hands_results, "multi_handedness", None) or [])
        for distance, side, hand_index in sorted(pairs):
            if side in matches or hand_index in used_hands:
                continue
            handedness_score = 1.0
            if hand_index < len(handedness) and handedness[hand_index].classification:
                handedness_score = float(handedness[hand_index].classification[0].score)
            match_score = max(0.0, 1.0 - distance / _HAND_MATCH_MAX_DISTANCE)
            confidence = float(np.clip(0.6 * handedness_score + 0.4 * match_score, 0.0, 1.0))
            matches[side] = (hand_index, confidence)
            used_hands.add(hand_index)
        return matches

    def _track_palm_roll(
        self,
        side: Side,
        match: tuple[int, float] | None,
        hands_results,
        world_lms,
        *,
        wrist_pitch_deg: float,
    ) -> Motor6Command:
        """把匹配后的单手 Hands 关键点转换为掌面 Roll 命令。

        输入：
        - side：待处理的左手或右手；
        - match：`_match_hands_to_pose()` 输出的手下标和融合置信度，可为空；
        - hands_results：Hands 当前帧完整结果；
        - world_lms：Pose 三维关键点，用于计算身体向上方向；
        - wrist_pitch_deg：当前侧腕部俯仰角，随 Roll 命令一起输出。

        输出：
        - `Motor6Command`；匹配和掌面拟合有效时允许运动，否则返回保持命令。

        逻辑：读取 Hands world landmarks 的腕点及 5/9/13/17 四个 MCP 点，统一转换为
        `Point3D`，再调用当前侧 `PalmRollTracker.update()`。这样掌面计算只使用同一个
        Hands 三维坐标系，避免混合 Pose 与 Hands 点坐标原点造成几何偏差。
        """
        tracker = self._palm_roll_trackers[side]
        if match is None:
            return tracker.missing(f"{side.value}_hands_missing", wrist_pitch_deg)

        hand_index, detection_confidence = match
        world_hands = list(getattr(hands_results, "multi_hand_world_landmarks", None) or [])
        if hand_index >= len(world_hands):
            return tracker.missing(f"{side.value}_hands_world_missing", wrist_pitch_deg)

        hand_lms = world_hands[hand_index].landmark
        kp = self._mp_hands.HandLandmark
        return tracker.update(
            hand_wrist=_point3d(hand_lms[kp.WRIST.value]),
            index_mcp=_point3d(hand_lms[kp.INDEX_FINGER_MCP.value]),
            middle_mcp=_point3d(hand_lms[kp.MIDDLE_FINGER_MCP.value]),
            ring_mcp=_point3d(hand_lms[kp.RING_FINGER_MCP.value]),
            pinky_mcp=_point3d(hand_lms[kp.PINKY_MCP.value]),
            world_up=self._body_up(world_lms),
            detection_confidence=detection_confidence,
            wrist_pitch_deg=wrist_pitch_deg,
        )

    def _body_up(self, world_lms) -> np.ndarray:
        """根据 Pose 肩髋四点计算身体向上单位向量。

        输入：Pose world landmarks 列表。
        输出：长度为 3 的 NumPy 单位向量，方向为髋中心指向肩中心。
        异常处理：肩髋重合导致向量退化时，回退到 MediaPipe 全局向上方向 `-Y`。
        """
        left_shoulder = self._world_lm(world_lms, "LEFT_SHOULDER")
        right_shoulder = self._world_lm(world_lms, "RIGHT_SHOULDER")
        left_hip = self._world_lm(world_lms, "LEFT_HIP")
        right_hip = self._world_lm(world_lms, "RIGHT_HIP")
        shoulder_mid = (left_shoulder + right_shoulder) * 0.5
        hip_mid = (left_hip + right_hip) * 0.5
        body_up = _vec3_norm(shoulder_mid - hip_mid)
        return body_up if np.linalg.norm(body_up) > 1e-6 else _UP_GLOBAL.copy()

    def _draw(
        self,
        image_bgr,
        pose_landmarks,
        img_lms,
        left_angles: np.ndarray,
        right_angles: np.ndarray,
        hands_results,
        hand_matches: dict[str, tuple[int, float]],
        palm_commands: dict[str, Motor6Command],
    ) -> None:
        """把骨架和左右臂 6 维角度绘制到 BGR 图像上。

        输入：
        - image_bgr：待绘制的 OpenCV BGR 图像；
        - pose_landmarks：MediaPipe 图像坐标关键点；
        - img_lms：关键点列表，用于文字定位；
        - left_angles/right_angles：左右臂 6 维角度数组；
        - hands_results：MediaPipe Hands 21 点结果；
        - hand_matches：Hands 检测与 Pose 左右腕的匹配结果；
        - palm_commands：左右手掌面 Roll 与法向量结果。

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
        self._draw_hands(image_bgr, hands_results, hand_matches, palm_commands)

        height, width = image_bgr.shape[:2]
        # 左侧文字靠关键点右侧显示，右侧文字靠关键点左侧显示，减少遮挡身体。
        self._draw_arm_text(image_bgr, img_lms, width, height, "LEFT", left_angles, (0, 255, 255), align_left=True)
        self._draw_arm_text(image_bgr, img_lms, width, height, "RIGHT", right_angles, (255, 0, 0), align_left=False)

    def _draw_hands(
        self,
        image_bgr,
        hands_results,
        hand_matches: dict[str, tuple[int, float]],
        palm_commands: dict[str, Motor6Command],
    ) -> None:
        """绘制完整 Hands 骨架，并为匹配成功的手绘制掌面法向量。

        输入：
        - image_bgr：待修改的 OpenCV BGR 图像；
        - hands_results：Hands 当前帧图像关键点；
        - hand_matches：左右侧与 Hands 下标的匹配结果；
        - palm_commands：左右侧掌面法向量和 Roll 计算结果。

        输出：无，直接修改 `image_bgr`。

        逻辑：左手使用黄色、右手使用蓝色、未匹配手使用灰色绘制 21 点骨架；命令中
        包含有效法向量时继续调用 `_draw_palm_normal()`。
        """
        image_hands = list(getattr(hands_results, "multi_hand_landmarks", None) or [])
        side_by_index = {match[0]: side for side, match in hand_matches.items()}
        colors = {"LEFT": (0, 220, 220), "RIGHT": (255, 120, 0)}
        for hand_index, landmarks in enumerate(image_hands):
            side = side_by_index.get(hand_index, "")
            color = colors.get(side, (180, 180, 180))
            self._drawing.draw_landmarks(
                image_bgr,
                landmarks,
                self._mp_hands.HAND_CONNECTIONS,
                self._drawing.DrawingSpec(color, 2, 2),
                self._drawing.DrawingSpec(color, 2, 1),
            )
            command = palm_commands.get(side)
            if command is not None and command.palm_normal is not None:
                self._draw_palm_normal(image_bgr, landmarks, command, color)

    def _draw_palm_normal(self, image_bgr, landmarks, command: Motor6Command, color) -> None:
        """在掌心绘制法向量的二维投影和深度分量。

        输入：
        - image_bgr：待修改的 BGR 图像；
        - landmarks：当前手 21 个归一化图像关键点；
        - command：包含 `palm_normal` 的掌面 Roll 命令；
        - color：该侧手骨架使用的 BGR 颜色。

        输出：无，直接绘制箭头和 `Nz` 文本。

        逻辑：掌心位置取腕点与 4 个 MCP 点的图像质心；法向量 X/Y 分量决定箭头在
        屏幕上的方向，Z 分量以 `Nz:+/-x.xx` 显示。掌心与手背翻转时 Nz 符号应相反，
        可用于直观检查法向量是否与真实掌面一致。
        """
        height, width = image_bgr.shape[:2]
        kp = self._mp_hands.HandLandmark
        center_indices = (
            kp.WRIST.value,
            kp.INDEX_FINGER_MCP.value,
            kp.MIDDLE_FINGER_MCP.value,
            kp.RING_FINGER_MCP.value,
            kp.PINKY_MCP.value,
        )
        center_x = int(np.mean([landmarks.landmark[index].x for index in center_indices]) * width)
        center_y = int(np.mean([landmarks.landmark[index].y for index in center_indices]) * height)
        normal = command.palm_normal
        scale = max(45, int(min(width, height) * 0.07))
        end_x = int(np.clip(center_x + normal.x * scale, 0, width - 1))
        end_y = int(np.clip(center_y + normal.y * scale, 0, height - 1))
        self._cv2.arrowedLine(
            image_bgr,
            (center_x, center_y),
            (end_x, end_y),
            color,
            3,
            self._cv2.LINE_AA,
            tipLength=0.25,
        )
        self._cv2.putText(
            image_bgr,
            f"Nz:{normal.z:+.2f}",
            (max(0, center_x - 35), min(height - 6, center_y + 34)),
            self._cv2.FONT_HERSHEY_SIMPLEX,
            0.55,
            color,
            1,
            self._cv2.LINE_AA,
        )

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
                ("wrist_rotation.horizontal_deg", "W Roll", values[5]),
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
                y = int(np.clip(y_base + row * 20, 18, max(18, height - 4)))
                self._cv2.putText(
                    image_bgr,
                    text,
                    (max(0, x), y),
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
            "source": "video_inf_pose_hands",
            "angles": [],
            "motor_6": motor_6_payload,
            "motor_commands": [],
            "control_stage": "video_inf_pose_hands_6d",
            "has_pose": False,
            "has_face": False,
            "has_left_hand": False,
            "has_right_hand": False,
            "has_left_palm": False,
            "has_right_palm": False,
            "pose_valid": False,
        }


def _vec3_norm(vec: np.ndarray) -> np.ndarray:
    """向量归一化。

    输入：三维向量。
    输出：单位向量；若向量长度过小，返回同形状零向量。
    """
    norm = np.linalg.norm(vec)
    return vec / norm if norm > 1e-6 else np.zeros_like(vec)


def _direction_from_signed_angle(
    angle: float,
    positive: Direction,
    negative: Direction,
) -> Direction:
    """根据有符号角度生成方向；零位没有运动方向。"""
    if angle > 1e-6:
        return positive
    if angle < -1e-6:
        return negative
    return Direction.UNKNOWN


def _point3d(landmark) -> Point3D:
    """把 MediaPipe 三维关键点转换为项目 `Point3D`。

    输入：具有 x/y/z 属性的 MediaPipe landmark。
    输出：三个坐标均转成 Python float 的不可变 `Point3D`。
    """
    return Point3D(x=float(landmark.x), y=float(landmark.y), z=float(landmark.z))


def _command_roll_or_zero(command: Motor6Command) -> float:
    """提取命令中的有符号 Roll 供角度面板显示。

    输入：`Motor6Command`。
    输出：存在 `signed_roll_deg` 时返回其浮点值，否则返回 0；真正的发送有效性仍由
    `motor_6.valid` 和 `motion_allowed` 控制，不会因为显示回退值而误发动作。
    """
    value = command.signed_roll_deg
    return 0.0 if value is None else float(value)


def _round_deg(value) -> float:
    """角度统一保留 2 位小数，便于显示和通信。"""
    return round(float(value), 2)
