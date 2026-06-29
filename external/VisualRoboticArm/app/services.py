from __future__ import annotations

from dataclasses import asdict
from pathlib import Path

import numpy as np

from app.models import Motor6Command, Point3D
from solver.angle_calc import compute_arm_angles, compute_right_wrist_motor_6

# 手部模型文件路径（与 mediapipe_test 使用同一官方 hand_landmarker.task）
_MODEL_DIR = Path(__file__).resolve().parent.parent / "models"
_HAND_MODEL_PATH = _MODEL_DIR / "hand_landmarker.task"

# 手部骨架连线（与 mp.solutions.holistic.HAND_CONNECTIONS 一致）
_HAND_CONNECTIONS = (
    (0, 1), (1, 2), (2, 3), (3, 4),
    (0, 5), (5, 6), (6, 7), (7, 8),
    (5, 9), (9, 10), (10, 11), (11, 12),
    (9, 13), (13, 14), (14, 15), (15, 16),
    (13, 17), (17, 18), (18, 19), (19, 20),
    (0, 17),
)


class MediaPipePoseHandService:
    """Pose + HandLandmarker 独立组合服务，替代原 Holistic 方案。

    改造要点：
    - Pose：沿用 mp.solutions.pose.Pose()（Solutions API），负责躯干/手臂关键点
    - 手部：使用 mp.tasks.vision.HandLandmarker（Tasks API）独立手部模型，精度显著提升
    - 面部 mesh 完全移除，无面部检测开销
    """

    def __init__(
        self,
        model_complexity: int = 1,
        smooth_landmarks: bool = True,
    ):
        import cv2
        import mediapipe as mp

        self._cv2 = cv2
        self._mp = mp

        # --- Pose（Solutions API，仅关注上肢关键点） ---
        if not hasattr(mp, "solutions") or not hasattr(mp.solutions, "pose"):
            raise ImportError("需要 mediapipe solutions.pose 模块，请安装 mediapipe>=0.10.18")

        self._pose = mp.solutions.pose.Pose(
            static_image_mode=False,
            model_complexity=model_complexity,
            smooth_landmarks=smooth_landmarks,
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5,
        )
        self._pose_landmark = mp.solutions.pose.PoseLandmark

        # --- 手部（Tasks API 独立模型） ---
        if not _HAND_MODEL_PATH.exists():
            raise FileNotFoundError(
                f"手部模型文件未找到: {_HAND_MODEL_PATH}\n"
                "请下载 hand_landmarker.task 放入 models/ 目录\n"
                "官方地址: https://storage.googleapis.com/mediapipe-models/"
                "hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task"
            )

        from mediapipe.tasks import python as mp_python
        from mediapipe.tasks.python import vision

        # 读取模型文件二进制内容，通过 model_asset_buffer 传入，绕过
        # mediapipe C 扩展对 model_asset_path 的路径拼接问题
        with open(_HAND_MODEL_PATH, 'rb') as _f:
            _model_buffer = _f.read()

        options = vision.HandLandmarkerOptions(
            base_options=mp_python.BaseOptions(model_asset_buffer=_model_buffer),
            running_mode=vision.RunningMode.VIDEO,
            num_hands=2,
            min_hand_detection_confidence=0.5,
            min_hand_presence_confidence=0.5,
            min_tracking_confidence=0.5,
        )
        self._hand_landmarker = vision.HandLandmarker.create_from_options(options)

        # 绘图工具（Pose 沿用 solutions drawing_utils，手部手动绘制）
        self._drawing = mp.solutions.drawing_utils
        self._styles = mp.solutions.drawing_styles

        # 时间戳基准（HandLandmarker 需要单调递增毫秒时间戳）
        self._start_time = None

    # ------------------------------------------------------------------
    # 生命周期
    # ------------------------------------------------------------------

    def close(self):
        if self._pose is not None:
            self._pose.close()
            self._pose = None
        if self._hand_landmarker is not None:
            self._hand_landmarker.close()
            self._hand_landmarker = None

    # ------------------------------------------------------------------
    # 主处理入口
    # ------------------------------------------------------------------

    def process_bgr(self, frame_bgr: np.ndarray) -> tuple[np.ndarray, dict]:
        import time

        frame_rgb = self._cv2.cvtColor(frame_bgr, self._cv2.COLOR_BGR2RGB)

        # 1. Pose 检测（Solutions API）
        pose_results = self._pose.process(frame_rgb)

        # 2. 手部检测（Tasks API，需要 mp.Image + 时间戳）
        if self._start_time is None:
            self._start_time = time.perf_counter()
        timestamp_ms = int((time.perf_counter() - self._start_time) * 1000)

        mp_image = self._mp.Image(image_format=self._mp.ImageFormat.SRGB, data=frame_rgb)
        hand_results = self._hand_landmarker.detect_for_video(mp_image, timestamp_ms)

        # 3. 按左右手分类
        left_hand, right_hand = self._classify_hands(hand_results)

        # 4. 绘制
        annotated = frame_bgr.copy()
        self._draw(annotated, pose_results, left_hand, right_hand)

        # 5. 角度计算
        angles = self._compute_arm_angles(pose_results, left_hand, right_hand)
        motor_6 = self._compute_motor_6(pose_results, right_hand)
        motor_6_payload = asdict(motor_6)

        payload = {
            "angles": [asdict(a) for a in angles if a is not None],
            "motor_6": motor_6_payload,
            "motor_commands": [motor_6_payload] if motor_6.valid else [],
            "control_stage": "right_wrist_motor_6_only",
            "has_pose": pose_results.pose_landmarks is not None,
            "has_face": False,
            "has_left_hand": left_hand is not None,
            "has_right_hand": right_hand is not None,
        }
        return annotated, payload

    # ------------------------------------------------------------------
    # 左右手分类
    # ------------------------------------------------------------------

    @staticmethod
    def _classify_hands(hand_results):
        """根据 HandLandmarker 的 handedness 输出拆分为左/右手关键点列表。

        Tasks API 返回格式：
        - hand_results.hand_landmarks: list[list[NormalizedLandmark]]
        - hand_results.handedness: list[list[Category]]，Category.category_name 为 "Left"/"Right"
        """
        left = None
        right = None

        if hand_results.hand_landmarks:
            for i, landmarks in enumerate(hand_results.hand_landmarks):
                if i < len(hand_results.handedness) and hand_results.handedness[i]:
                    category = hand_results.handedness[i][0]
                    if category.category_name == "Left":
                        left = list(landmarks)
                    elif category.category_name == "Right":
                        right = list(landmarks)

        return left, right

    # ------------------------------------------------------------------
    # 绘制
    # ------------------------------------------------------------------

    def _draw(self, image_bgr, pose_results, left_hand, right_hand):
        """绘制 Pose 骨架 + 手部关键点（不含面部 mesh）。"""
        mp_pose = self._mp.solutions.pose

        # Pose 骨架
        if pose_results.pose_landmarks is not None:
            self._drawing.draw_landmarks(
                image_bgr,
                pose_results.pose_landmarks,
                mp_pose.POSE_CONNECTIONS,
                landmark_drawing_spec=self._styles.get_default_pose_landmarks_style(),
            )

        # 手部（Tasks API 返回普通 list，手动绘制）
        height, width = image_bgr.shape[:2]

        if left_hand is not None:
            self._draw_hand_manual(image_bgr, left_hand, width, height, (0, 210, 255))
        if right_hand is not None:
            self._draw_hand_manual(image_bgr, right_hand, width, height, (0, 210, 255))
            self._draw_right_palm_plane(image_bgr, right_hand, width, height)

    def _draw_hand_manual(self, image_bgr, landmarks, width, height, color):
        """手动绘制手部关键点与骨架连线。

        landmarks: Task API 返回的 list[NormalizedLandmark]，每个有 .x/.y/.z 属性。
        """
        points = []
        for lm in landmarks:
            x = int(np.clip(lm.x, 0.0, 1.0) * (width - 1))
            y = int(np.clip(lm.y, 0.0, 1.0) * (height - 1))
            points.append((x, y))

        # 骨架连线
        for start, end in _HAND_CONNECTIONS:
            if start < len(points) and end < len(points):
                self._cv2.line(image_bgr, points[start], points[end], color, 2)

        # 关键点（指尖/腕部用更大圆点）
        for i, pt in enumerate(points):
            radius = 6 if i in (0, 4, 8, 12, 16, 20) else 4
            self._cv2.circle(image_bgr, pt, radius, (0, 255, 0), -1)
            self._cv2.circle(image_bgr, pt, radius + 1, (0, 80, 0), 1)

    def _draw_right_palm_plane(self, image_bgr, hand_landmarks, img_width, img_height):
        """绘制右手掌面（用于 6 号电机现场确认）。"""
        if len(hand_landmarks) < 18:
            return

        def point(idx: int) -> tuple[int, int]:
            lm = hand_landmarks[idx]
            x = int(np.clip(lm.x, 0.0, 1.0) * (img_width - 1))
            y = int(np.clip(lm.y, 0.0, 1.0) * (img_height - 1))
            return x, y

        wrist = point(0)
        thumb_tip = point(4)
        index_mcp = point(5)
        middle_mcp = point(9)
        ring_mcp = point(13)
        pinky_mcp = point(17)

        palm_poly = np.array([wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp], dtype=np.int32)
        overlay = image_bgr.copy()
        self._cv2.fillConvexPoly(overlay, palm_poly, (80, 190, 255))
        self._cv2.addWeighted(overlay, 0.24, image_bgr, 0.76, 0.0, image_bgr)
        self._cv2.polylines(image_bgr, [palm_poly], True, (0, 150, 255), 2, self._cv2.LINE_AA)
        self._cv2.line(image_bgr, wrist, thumb_tip, (255, 80, 80), 2, self._cv2.LINE_AA)
        self._cv2.circle(image_bgr, wrist, 5, (0, 80, 255), -1, self._cv2.LINE_AA)
        self._cv2.circle(image_bgr, thumb_tip, 5, (255, 80, 80), -1, self._cv2.LINE_AA)

    # ------------------------------------------------------------------
    # 角度计算
    # ------------------------------------------------------------------

    def _compute_arm_angles(self, pose_results, left_hand, right_hand):
        if pose_results.pose_landmarks is None:
            return []

        pose = pose_results.pose_landmarks.landmark

        def pose_point(idx) -> Point3D:
            lm = pose[idx]
            return Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        left_shoulder = pose_point(self._pose_landmark.LEFT_SHOULDER)
        left_elbow = pose_point(self._pose_landmark.LEFT_ELBOW)
        left_wrist = pose_point(self._pose_landmark.LEFT_WRIST)
        left_hip = pose_point(self._pose_landmark.LEFT_HIP)

        right_shoulder = pose_point(self._pose_landmark.RIGHT_SHOULDER)
        right_elbow = pose_point(self._pose_landmark.RIGHT_ELBOW)
        right_wrist = pose_point(self._pose_landmark.RIGHT_WRIST)
        right_hip = pose_point(self._pose_landmark.RIGHT_HIP)

        # 手部 MCP 参考点（Tasks API：landmarks 已是 list，直接索引）
        left_wrist_mcp = None
        if left_hand is not None and len(left_hand) > 5:
            lm = left_hand[5]
            left_wrist_mcp = Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        right_wrist_mcp = None
        if right_hand is not None and len(right_hand) > 5:
            lm = right_hand[5]
            right_wrist_mcp = Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        return compute_arm_angles(
            left_shoulder=left_shoulder,
            left_elbow=left_elbow,
            left_wrist=left_wrist,
            left_hip=left_hip,
            right_shoulder=right_shoulder,
            right_elbow=right_elbow,
            right_wrist=right_wrist,
            right_hip=right_hip,
            left_hand_ref=left_wrist_mcp,
            right_hand_ref=right_wrist_mcp,
        )

    def _compute_motor_6(self, pose_results, right_hand) -> Motor6Command:
        """根据右手掌面生成 6 号电机指令。"""
        if pose_results.pose_landmarks is None:
            return Motor6Command(reason="pose_missing")

        if right_hand is None:
            return Motor6Command(reason="right_hand_missing")

        pose = pose_results.pose_landmarks.landmark
        hand = right_hand  # Tasks API：已是 list[NormalizedLandmark]

        required_hand_count = 18
        if len(hand) < required_hand_count:
            return Motor6Command(reason="right_hand_incomplete")

        def pose_point(idx) -> Point3D:
            lm = pose[idx]
            return Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        def hand_point(idx: int) -> Point3D:
            lm = hand[idx]
            return Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        return compute_right_wrist_motor_6(
            left_shoulder=pose_point(self._pose_landmark.LEFT_SHOULDER),
            right_shoulder=pose_point(self._pose_landmark.RIGHT_SHOULDER),
            left_hip=pose_point(self._pose_landmark.LEFT_HIP),
            right_hip=pose_point(self._pose_landmark.RIGHT_HIP),
            right_elbow=pose_point(self._pose_landmark.RIGHT_ELBOW),
            right_wrist=pose_point(self._pose_landmark.RIGHT_WRIST),
            hand_wrist=hand_point(0),
            thumb_tip=hand_point(4),
            index_mcp=hand_point(5),
            middle_mcp=hand_point(9),
            middle_tip=hand_point(12),
            middle_pip=hand_point(10),
            ring_mcp=hand_point(13),
            pinky_mcp=hand_point(17),
        )
