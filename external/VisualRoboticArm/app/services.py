from __future__ import annotations

from dataclasses import asdict

import numpy as np

from app.models import Motor6Command, Point3D
from solver.angle_calc import compute_arm_angles, compute_right_wrist_motor_6


class MediaPipeHolisticService:
    def __init__(
        self,
        model_complexity: int = 1,
        smooth_landmarks: bool = True,
        refine_face_landmarks: bool = True,
    ):
        import cv2
        import importlib
        import os
        import sys

        try:
            import mediapipe as mp
        except ModuleNotFoundError as exc:
            raise ImportError("未安装 mediapipe。请先执行 pip install -r requirements.txt") from exc

        if not hasattr(mp, "solutions"):
            loaded_from = getattr(mp, "__file__", "") or repr(mp)
            suspect_root = ""
            if isinstance(loaded_from, str) and loaded_from:
                suspect_root = os.path.dirname(os.path.dirname(loaded_from))

            original_sys_path = list(sys.path)
            try:
                if suspect_root:
                    sys.path = [
                        p for p in sys.path if os.path.normcase(p) != os.path.normcase(suspect_root)
                    ]
                sys.modules.pop("mediapipe", None)
                try:
                    mp = importlib.import_module("mediapipe")
                except ModuleNotFoundError as exc:
                    raise ImportError(
                        "mediapipe 被第三方同名包遮蔽，但环境中未安装官方 mediapipe。"
                        f"当前导入来源: {loaded_from}。"
                        "请先执行 pip install -r requirements.txt（其中会安装 mediapipe==0.10.14），"
                        "并确保 third_party 不在 PYTHONPATH 或被优先导入。"
                    ) from exc
            finally:
                sys.path = original_sys_path

            if not hasattr(mp, "solutions"):
                version = getattr(mp, "__version__", "unknown")
                raise ImportError(
                    "当前 mediapipe 不包含 solutions（该项目依赖 solutions API）。"
                    f"当前版本: {version}，导入来源: {loaded_from}。"
                    "请安装 mediapipe==0.10.14，并确保 third_party 不在 PYTHONPATH 或被优先导入。"
                )

        self._cv2 = cv2
        self._mp = mp

        self._drawing = mp.solutions.drawing_utils
        self._styles = mp.solutions.drawing_styles
        self._holistic = mp.solutions.holistic.Holistic(
            static_image_mode=False,
            model_complexity=model_complexity,
            smooth_landmarks=smooth_landmarks,
            refine_face_landmarks=refine_face_landmarks,
        )

        self._pose_landmark = mp.solutions.pose.PoseLandmark

    def close(self):
        if self._holistic is not None:
            self._holistic.close()
            self._holistic = None

    def process_bgr(self, frame_bgr: np.ndarray) -> tuple[np.ndarray, dict]:
        frame_rgb = self._cv2.cvtColor(frame_bgr, self._cv2.COLOR_BGR2RGB)
        results = self._holistic.process(frame_rgb)

        annotated = frame_bgr.copy()
        self._draw(annotated, results)
        angles = self._compute_arm_angles(results)
        motor_6 = self._compute_motor_6(results)
        motor_6_payload = asdict(motor_6)

        payload = {
            "angles": [asdict(a) for a in angles if a is not None],
            "motor_6": motor_6_payload,
            "motor_commands": [motor_6_payload] if motor_6.valid else [],
            "control_stage": "right_wrist_motor_6_only",
            "has_pose": results.pose_landmarks is not None,
            "has_face": results.face_landmarks is not None,
            "has_left_hand": results.left_hand_landmarks is not None,
            "has_right_hand": results.right_hand_landmarks is not None,
        }
        return annotated, payload

    def _draw(self, image_bgr: np.ndarray, results):
        mp_holistic = self._mp.solutions.holistic

        if results.face_landmarks is not None:
            self._drawing.draw_landmarks(
                image_bgr,
                results.face_landmarks,
                mp_holistic.FACEMESH_CONTOURS,
                landmark_drawing_spec=None,
                connection_drawing_spec=self._styles.get_default_face_mesh_contours_style(),
            )

        if results.pose_landmarks is not None:
            self._drawing.draw_landmarks(
                image_bgr,
                results.pose_landmarks,
                mp_holistic.POSE_CONNECTIONS,
                landmark_drawing_spec=self._styles.get_default_pose_landmarks_style(),
            )

        if results.left_hand_landmarks is not None:
            self._drawing.draw_landmarks(
                image_bgr,
                results.left_hand_landmarks,
                mp_holistic.HAND_CONNECTIONS,
                landmark_drawing_spec=self._styles.get_default_hand_landmarks_style(),
                connection_drawing_spec=self._styles.get_default_hand_connections_style(),
            )

        if results.right_hand_landmarks is not None:
            self._drawing.draw_landmarks(
                image_bgr,
                results.right_hand_landmarks,
                mp_holistic.HAND_CONNECTIONS,
                landmark_drawing_spec=self._styles.get_default_hand_landmarks_style(),
                connection_drawing_spec=self._styles.get_default_hand_connections_style(),
            )
            self._draw_right_palm_plane(image_bgr, results.right_hand_landmarks)

    def _draw_right_palm_plane(self, image_bgr: np.ndarray, hand_landmarks):
        """绘制参与 6 号电机计算的右手掌面，便于现场确认掌面是否贴合。"""
        hand = hand_landmarks.landmark
        if len(hand) < 18:
            return

        height, width = image_bgr.shape[:2]

        def point(idx: int) -> tuple[int, int]:
            lm = hand[idx]
            x = int(np.clip(lm.x, 0.0, 1.0) * (width - 1))
            y = int(np.clip(lm.y, 0.0, 1.0) * (height - 1))
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

    def _compute_arm_angles(self, results):
        if results.pose_landmarks is None:
            return []

        pose = results.pose_landmarks.landmark

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

        left_wrist_mcp = None
        if results.left_hand_landmarks is not None:
            hand = results.left_hand_landmarks.landmark
            if len(hand) > 5:
                lm = hand[5]
                left_wrist_mcp = Point3D(x=float(lm.x), y=float(lm.y), z=float(lm.z))

        right_wrist_mcp = None
        if results.right_hand_landmarks is not None:
            hand = results.right_hand_landmarks.landmark
            if len(hand) > 5:
                lm = hand[5]
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

    def _compute_motor_6(self, results) -> Motor6Command:
        """只根据右手手掌姿态生成 6 号电机验证指令。"""
        if results.pose_landmarks is None:
            return Motor6Command(reason="pose_missing")

        if results.right_hand_landmarks is None:
            if results.left_hand_landmarks is not None:
                return Motor6Command(reason="right_hand_missing_left_hand_detected")
            return Motor6Command(reason="right_hand_missing")

        pose = results.pose_landmarks.landmark
        hand = results.right_hand_landmarks.landmark
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
            ring_mcp=hand_point(13),
            pinky_mcp=hand_point(17),
        )
