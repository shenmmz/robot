"""腕部角度计算模块。

负责手腕 Motor 5（上下 pitch）和 Motor 6（左右 roll）的计算。
包含 A/B/C 三路独立计算路径与多路置信度加权融合策略。

A 路：Pose 肘+腕 + Hands 关键点 → 躯干坐标系滚转角（完整路径）
B 路：纯 Hands（WRIST + 4 MCP）→ 世界坐标系滚转角（Pose 遮挡降级）
C 路：参考平面 ⟂ 旋转轴（纯局域几何，消除身体倾斜退化）
"""

from __future__ import annotations

from typing import Optional

import numpy as np

from app.models import Direction, Motor6Command, Point3D, Side
from solver.utils import (
    _EPS,
    BodyFrame,
    KalmanFilter1D,
    build_body_frame,
    fit_plane_normal,
    normalize,
    orient_palm_normal,
    project_to_plane,
    signed_angle_deg,
    to_vec,
    vec_norm,
)

# ================================================================
# 全局状态：遮挡帧计数 + 卡尔曼滤波器
# ================================================================

_elbow_occluded_frames = 0
_wrist_occluded_frames = 0
_thumb_occluded_frames = 0
_OCCLUSION_THRESHOLD = 0.5
_OCCLUSION_CONSECUTIVE_FRAMES = 3

_kalman_filter: KalmanFilter1D = KalmanFilter1D()
_kalman_occluded_frames = 0
_MAX_PREDICTION_FRAMES = 5

# 手腕翻转角度限制（度）
WRIST_ROLL_MAX_DEG = 120.0


class PalmRollTracker:
    """基于 MediaPipe Hands 掌面法向量跟踪手腕有符号滚转角。

    输入来源：
    - 同一只手、同一帧 Hands world landmarks 中的腕点和 4 个掌指关节点；
    - Pose 计算出的身体向上方向，用于建立手腕滚转零度参考；
    - Hands 检测和左右腕匹配得到的置信度。

    输出：
    - `Motor6Command`，包含限幅和平滑后的有符号 Roll、原始展开角、方向、
      掌面法向量、置信度以及是否允许运动。

    核心逻辑：
    1. 使用腕点与食指/中指/无名指/小指 MCP 点拟合掌面；
    2. 将掌面法向量和身体向上方向投影到手轴的垂直平面；
    3. 根据上一帧法向量解决 SVD 法向量正负二义性；
    4. 计算绕手轴的有符号角并做跨越正负 180 度的连续展开；
    5. 经过置信度门控、角度限幅、指数平滑和单帧最大步长限制后输出。
    """

    def __init__(
        self,
        side: Side,
        *,
        max_roll_deg: float = 90.0,
        smoothing_alpha: float = 0.35,
        max_step_deg: float = 25.0,
        min_confidence: float = 0.30,
    ) -> None:
        """初始化单手掌面 Roll 跟踪器。

        输入：
        - side：当前跟踪器所属侧，`Side.LEFT` 或 `Side.RIGHT`；
        - max_roll_deg：最终输出绝对值上限，单位为度；
        - smoothing_alpha：指数平滑系数，范围 0~1，越大越跟手；
        - max_step_deg：相邻输出帧允许变化的最大角度；
        - min_confidence：掌面结果进入控制链路的最低置信度。

        输出：无。构造完成后内部历史状态为空，第一帧有效结果直接作为初值。
        """
        self._side = side
        self._max_roll_deg = float(max_roll_deg)
        self._smoothing_alpha = float(np.clip(smoothing_alpha, 0.0, 1.0))
        self._max_step_deg = float(max_step_deg)
        self._min_confidence = float(min_confidence)
        self.reset()

    def reset(self) -> None:
        """清空法向量、原始展开角和滤波角历史。

        输入：无。
        输出：无。
        功能：在服务关闭、重新开始采集或切换视频时防止沿用上一段动作。
        """
        self._previous_normal: np.ndarray | None = None
        self._previous_raw_roll: float | None = None
        self._filtered_roll: float | None = None

    def missing(self, reason: str, wrist_pitch_deg: float | None = None) -> Motor6Command:
        """生成 Hands 缺失或掌面无效时的保持命令。

        输入：
        - reason：无效原因，写入命令供日志和界面诊断；
        - wrist_pitch_deg：当前腕部俯仰角，可为空。

        输出：
        - `valid=False`、`motion_allowed=False` 的 `Motor6Command`；
        - 若存在历史 Roll，仍写入角度字段用于显示和问题定位，但通信层不会发送动作。
        """
        angle = self._filtered_roll
        return Motor6Command(
            side=self._side,
            valid=False,
            mode="mediapipe_hands_palm_normal",
            reference="body_up_projected_to_hand_axis",
            roll_deg=abs(angle) if angle is not None else None,
            signed_roll_deg=angle,
            raw_signed_roll_deg=self._previous_raw_roll,
            direction=_direction_from_roll(angle),
            thumb_direction=_direction_from_roll(angle),
            rotation_side=_rotation_side_from_roll(angle),
            wrist_pitch_deg=wrist_pitch_deg,
            motion_allowed=False,
            reason=reason,
        )

    def update(
        self,
        *,
        hand_wrist: Point3D,
        index_mcp: Point3D,
        middle_mcp: Point3D,
        ring_mcp: Point3D,
        pinky_mcp: Point3D,
        world_up: np.ndarray,
        detection_confidence: float,
        wrist_pitch_deg: float | None = None,
    ) -> Motor6Command:
        """根据一帧 Hands 三维关键点更新掌面 Roll。

        输入：
        - hand_wrist：Hands 腕点 0；
        - index_mcp/middle_mcp/ring_mcp/pinky_mcp：掌面 4 个 MCP 点 5/9/13/17；
        - world_up：由 Pose 肩髋中点计算的身体向上单位向量；
        - detection_confidence：Hands 检测分数与 Pose 腕点匹配分数的融合值；
        - wrist_pitch_deg：Pose 方向算法得到的腕部俯仰角，仅随命令透传。

        输出：
        - 有效时返回 `valid=True` 的 `Motor6Command`，`signed_roll_deg` 为控制角；
        - 关键向量退化或置信度不足时调用 `missing()`，返回禁止运动的保持命令。

        处理步骤：
        1. 以腕点到 4 个 MCP 质心的方向作为手轴；
        2. 对 5 点执行 SVD 平面拟合并确定掌面法向量朝向；
        3. 把掌面法向量与身体向上向量投影到手轴垂直平面；
        4. 使用上一帧法向量消除随机翻面，再计算有符号 Roll；
        5. 评估掌面质量，执行角度展开、限幅、平滑和死区处理。
        """
        points = [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp]
        palm_center = _compute_hand_palm_center(index_mcp, middle_mcp, ring_mcp, pinky_mcp)
        hand_axis = normalize(palm_center - to_vec(hand_wrist))
        if hand_axis is None:
            return self.missing("hands_axis_invalid", wrist_pitch_deg)

        palm_normal = fit_plane_normal(points)
        if palm_normal is None:
            return self.missing("hands_palm_plane_invalid", wrist_pitch_deg)
        palm_normal = orient_palm_normal(
            palm_normal,
            hand_wrist=hand_wrist,
            index_mcp=index_mcp,
            middle_mcp=middle_mcp,
            ring_mcp=ring_mcp,
            pinky_mcp=pinky_mcp,
        )

        normal_axis = project_to_plane(palm_normal, hand_axis)
        reference_axis = project_to_plane(world_up, hand_axis)
        if normal_axis is None or reference_axis is None:
            return self.missing("hands_roll_projection_invalid", wrist_pitch_deg)

        # SVD 法向量存在正负二义性。相邻物理帧不应突变超过 90 度，
        # 因此选择与上一帧夹角更小的方向，消除算法层面的随机翻面。
        if self._previous_normal is not None and float(np.dot(normal_axis, self._previous_normal)) < 0.0:
            normal_axis = -normal_axis
            palm_normal = -palm_normal

        raw_roll = signed_angle_deg(reference_axis, normal_axis, hand_axis)
        if raw_roll is None:
            return self.missing("hands_roll_angle_invalid", wrist_pitch_deg)
        raw_roll = _unwrap_angle(raw_roll, self._previous_raw_roll)

        plane_quality = _palm_plane_quality(points)
        confidence = float(np.clip(detection_confidence, 0.0, 1.0)) * plane_quality
        if confidence < self._min_confidence:
            return self.missing("hands_palm_confidence_low", wrist_pitch_deg)

        limited_roll = float(np.clip(raw_roll, -self._max_roll_deg, self._max_roll_deg))
        if self._filtered_roll is None:
            filtered_roll = limited_roll
        else:
            smoothed = self._filtered_roll + self._smoothing_alpha * (limited_roll - self._filtered_roll)
            step = float(np.clip(smoothed - self._filtered_roll, -self._max_step_deg, self._max_step_deg))
            filtered_roll = self._filtered_roll + step

        filtered_roll = float(np.clip(filtered_roll, -self._max_roll_deg, self._max_roll_deg))
        if abs(filtered_roll) < 1.0:
            filtered_roll = 0.0

        self._previous_normal = normal_axis
        self._previous_raw_roll = raw_roll
        self._filtered_roll = filtered_roll

        direction = _direction_from_roll(filtered_roll)
        return Motor6Command(
            side=self._side,
            valid=True,
            mode="mediapipe_hands_palm_normal",
            reference="body_up_projected_to_hand_axis",
            roll_deg=abs(filtered_roll),
            signed_roll_deg=filtered_roll,
            raw_signed_roll_deg=raw_roll,
            direction=direction,
            thumb_direction=direction,
            rotation_side=_rotation_side_from_roll(filtered_roll),
            palm_normal=Point3D(
                x=float(palm_normal[0]),
                y=float(palm_normal[1]),
                z=float(palm_normal[2]),
            ),
            roll_vector=Point3D(
                x=float(normal_axis[0]),
                y=float(normal_axis[1]),
                z=float(normal_axis[2]),
            ),
            wrist_pitch_deg=wrist_pitch_deg,
            confidence=confidence,
            motion_allowed=True,
            reason="hands_palm_normal_ok",
        )


def _unwrap_angle(angle: float, previous: float | None) -> float:
    """把 `[-180, 180]` 原始角展开为跨帧连续角。

    输入：当前原始角 angle、上一帧已展开角 previous，单位均为度。
    输出：与 previous 差值限制在 `[-180, 180]` 内的等价角；无历史时原样返回。
    """
    if previous is None:
        return float(angle)
    unwrapped = float(angle)
    while unwrapped - previous > 180.0:
        unwrapped -= 360.0
    while unwrapped - previous < -180.0:
        unwrapped += 360.0
    return unwrapped


def _palm_plane_quality(points: list[Point3D]) -> float:
    """根据掌面点集的空间形状计算质量分数。

    输入：腕点和 4 个 MCP 点，共 5 个 `Point3D`。
    输出：0~1 的质量分数；近共线、厚度过大或 SVD 失败时返回 0。

    逻辑：第二奇异值衡量掌面是否有足够宽度，第三奇异值衡量点集偏离平面的厚度。
    Hands world landmarks 是模型估计值，不会构成绝对刚性平面，因此非退化点集保留
    0.40 的基础质量，避免轻微深度噪声造成频繁丢帧。
    """
    matrix = np.array([to_vec(point) for point in points], dtype=np.float64)
    centered = matrix - matrix.mean(axis=0)
    try:
        _, singular_values, _ = np.linalg.svd(centered, full_matrices=False)
    except np.linalg.LinAlgError:
        return 0.0
    if len(singular_values) < 3 or singular_values[0] <= _EPS:
        return 0.0

    width_ratio = float(singular_values[1] / singular_values[0])
    thickness_ratio = float(singular_values[2] / max(singular_values[1], _EPS))
    if width_ratio < 0.04 or thickness_ratio > 0.80:
        return 0.0
    spread_quality = float(np.clip(width_ratio / 0.20, 0.0, 1.0))
    plane_quality = float(np.clip(1.0 - thickness_ratio * 2.5, 0.0, 1.0))
    # Hands 三维关键点是模型估计值而非绝对刚性平面；点集不退化时保留置信度下限。
    return 0.40 + 0.60 * spread_quality * plane_quality


def _direction_from_roll(angle: float | None) -> Direction:
    """把有符号 Roll 转成方向枚举：正为右、负为左、绝对值小于 1 度为未知。"""
    if angle is None or abs(angle) < 1.0:
        return Direction.UNKNOWN
    return Direction.RIGHT if angle > 0.0 else Direction.LEFT


def _rotation_side_from_roll(angle: float | None) -> str:
    """把 Roll 方向转换为 payload 使用的 `right/left/neutral` 字符串。"""
    direction = _direction_from_roll(angle)
    if direction == Direction.RIGHT:
        return "right"
    if direction == Direction.LEFT:
        return "left"
    return "neutral"


# ================================================================
# 辅助：手掌中心
# ================================================================

def _compute_hand_palm_center(
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
) -> np.ndarray:
    """计算手掌中心 —— 4 个 MCP 点（5/9/13/17）的质心。"""
    vecs = [to_vec(p) for p in (index_mcp, middle_mcp, ring_mcp, pinky_mcp)]
    return np.mean(vecs, axis=0)


# ================================================================
# B 路：纯 Hands 计算
# ================================================================

def _compute_wrist_roll_hands_only(
    *,
    hand_wrist: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
    world_up: np.ndarray | None = None,
) -> tuple[float | None, np.ndarray | None, np.ndarray | None, np.ndarray | None]:
    """B 路：纯 MediaPipe Hands 关键点计算手腕滚转角。

    完全不依赖 Pose 关键点，仅使用 Hands 21 点中的：
    WRIST(0)、INDEX_MCP(5)、MIDDLE_MCP(9)、RING_MCP(13)、PINKY_MCP(17)

    算法：
    1. 前臂轴近似：WRIST → 手掌中心（4 个 MCP 点质心）
    2. 掌面法向量：SVD 拟合 + orient_palm_normal 定向
    3. 0° 参考：世界/躯干 UP 投影到前臂轴垂直面
    4. 有符号滚转角：palm_normal 投影到垂直面，与 0° 参考做 signed_angle_deg

    输出：
    - `(signed_roll_deg, forearm_axis, palm_normal_axis, palm_normal)`
    """
    if world_up is None:
        world_up = np.array([0.0, 1.0, 0.0], dtype=np.float64)

    palm_center = _compute_hand_palm_center(index_mcp, middle_mcp, ring_mcp, pinky_mcp)
    wrist_vec = to_vec(hand_wrist)
    forearm_axis = normalize(palm_center - wrist_vec)
    if forearm_axis is None:
        return None, None, None, None

    palm_normal = fit_plane_normal(
        [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp],
    )
    if palm_normal is None:
        return None, None, None, None

    palm_normal = orient_palm_normal(
        palm_normal,
        hand_wrist=hand_wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
    )

    palm_up_axis = project_to_plane(world_up, forearm_axis)
    if palm_up_axis is None:
        return None, None, None, None

    palm_normal_axis = project_to_plane(palm_normal, forearm_axis)
    if palm_normal_axis is None:
        return None, None, None, None

    if float(np.dot(palm_normal_axis, palm_up_axis)) < 0.0:
        palm_normal_axis = -palm_normal_axis

    signed_roll_deg = signed_angle_deg(palm_up_axis, palm_normal_axis, forearm_axis)
    if signed_roll_deg is None:
        return None, None, None, None

    return float(signed_roll_deg), forearm_axis, palm_normal_axis, palm_normal


# ================================================================
# C 路：参考平面 ⟂ 旋转轴
# ================================================================

def _compute_wrist_roll_c_path(
    *,
    wrist: Point3D,
    middle_mcp: Point3D,
    index_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
    thumb_tip: Point3D,
    wrist_vis: float = 1.0,
    middle_mcp_vis: float = 1.0,
    index_mcp_vis: float = 1.0,
    ring_mcp_vis: float = 1.0,
    pinky_mcp_vis: float = 1.0,
    thumb_tip_vis: float = 1.0,
) -> dict | None:
    """C 路手腕翻转角：参考平面垂直于旋转轴（纯局域几何）。

    几何模型：
    - 旋转轴 r = middle_mcp - wrist
    - 参考平面 ⟂ r_hat，过腕关节点
    - 手掌平面 SVD 拟合 5 点 → n_palm
    - n_proj = n_palm 在参考平面上的投影
    - n_zero = Y_world 在参考平面上的投影
    - roll_deg = sign · atan2(|n_proj×n_zero|, n_proj·n_zero) · 180/π

    输出：
    - 字典，包含 `roll_deg`、`reason`、`confidence`、`rotation_side`、
      `n_palm`、`n_proj`、`n_zero` 等键。
    """
    if middle_mcp_vis <= 0.5:
        return {
            "roll_deg": 0.0,
            "reason": "c_unavailable_no_axis",
            "confidence": 0.0,
            "rotation_side": "neutral",
            "n_palm": None,
            "n_proj": None,
            "n_zero": None,
        }

    vis_map = {
        "wrist": wrist_vis,
        "index_mcp": index_mcp_vis,
        "middle_mcp": middle_mcp_vis,
        "ring_mcp": ring_mcp_vis,
        "pinky_mcp": pinky_mcp_vis,
    }
    visible_count = sum(1 for v in vis_map.values() if v > 0.5)
    if visible_count < 3:
        return None

    wrist_v = to_vec(wrist)
    middle_v = to_vec(middle_mcp)
    r_vec = middle_v - wrist_v
    r_hat = normalize(r_vec)
    if r_hat is None:
        return None

    Y_world = np.array([0.0, 1.0, 0.0], dtype=np.float64)
    axis_verticality = abs(float(np.dot(r_hat, Y_world)))
    near_vertical = axis_verticality > 0.95

    n_palm = fit_plane_normal([wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp])
    if n_palm is None:
        return None

    n_palm = orient_palm_normal(
        n_palm,
        hand_wrist=wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
    )

    n_proj_raw = n_palm - float(np.dot(n_palm, r_hat)) * r_hat
    n_proj = normalize(n_proj_raw)
    if n_proj is None:
        return None

    n_zero_raw = Y_world - float(np.dot(Y_world, r_hat)) * r_hat
    n_zero = normalize(n_zero_raw)
    if n_zero is None:
        return None

    cross_np_nz = np.cross(n_zero, n_proj)
    cross_norm = float(np.linalg.norm(cross_np_nz))
    dot_np_nz = float(np.clip(np.dot(n_proj, n_zero), -1.0, 1.0))
    theta_raw = float(np.degrees(np.arctan2(cross_norm, dot_np_nz)))

    sign_val = float(np.sign(np.dot(cross_np_nz, r_hat)))
    if abs(sign_val) < _EPS:
        sign_val = 1.0
    roll_deg = sign_val * theta_raw

    if abs(roll_deg) < 1.0:
        rotation_side = "neutral"
    elif roll_deg < 0.0:
        rotation_side = "outward"
    else:
        rotation_side = "inward"

    base_conf = {5: 0.85, 4: 0.6, 3: 0.3}.get(visible_count, 0.0)

    points_for_svd = [to_vec(p) for p in (wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp)]
    matrix = np.array(points_for_svd, dtype=np.float64)
    centered = matrix - matrix.mean(axis=0)
    try:
        _, s, _ = np.linalg.svd(centered, full_matrices=False)
        svd_residual = float(s[-1] / s.sum()) if s.sum() > _EPS else 1.0
    except (np.linalg.LinAlgError, IndexError):
        svd_residual = 1.0

    svd_factor = float(np.clip(1.0 - svd_residual * 5.0, 0.0, 1.0))
    confidence = base_conf * svd_factor
    if near_vertical:
        confidence *= 0.2
    confidence = float(np.clip(confidence, 0.0, 1.0))

    return {
        "roll_deg": roll_deg,
        "reason": "ok_c_path",
        "confidence": confidence,
        "rotation_side": rotation_side,
        "n_palm": n_palm,
        "n_proj": n_proj,
        "n_zero": n_zero,
    }


# ================================================================
# 手腕上下识别：wrist pitch
# ================================================================

def _compute_wrist_pitch(
    forearm_axis: np.ndarray,
    hand_middle_tip: Point3D,
    hand_wrist: Point3D,
    frame_up: np.ndarray | None = None,
) -> tuple[float | None, np.ndarray | None]:
    """计算手腕前后倾角（pitch / flexion-extension）。

    - 手掌与前臂平直 → 0°
    - 手腕向上弯曲（手背方向/伸展）→ 正角度
    - 手腕向下弯曲（掌心方向/屈曲）→ 负角度
    """
    if frame_up is None:
        frame_up = np.array([0.0, 1.0, 0.0], dtype=np.float64)

    wrist_vec = to_vec(hand_wrist)
    palm_direction = normalize(to_vec(hand_middle_tip) - wrist_vec)
    if palm_direction is None:
        return None, None

    cos_angle = float(np.clip(np.dot(palm_direction, forearm_axis), -1.0, 1.0))

    up_proj = project_to_plane(frame_up, forearm_axis)
    if up_proj is None:
        pitch_abs = float(np.degrees(np.arccos(cos_angle)))
        return pitch_abs, palm_direction

    rotation_axis = normalize(np.cross(forearm_axis, up_proj))
    if rotation_axis is None:
        pitch_abs = float(np.degrees(np.arccos(cos_angle)))
        return pitch_abs, palm_direction

    sin_component = float(np.dot(rotation_axis, np.cross(forearm_axis, palm_direction)))
    pitch_deg = float(np.degrees(np.arctan2(sin_component, cos_angle)))
    return pitch_deg, palm_direction


# ================================================================
# 腕部主入口：A/B/C 三路 + 融合 + 卡尔曼兜底
# ================================================================

def compute_right_wrist_motor_6(
    *,
    left_shoulder: Point3D,
    right_shoulder: Point3D,
    left_hip: Point3D,
    right_hip: Point3D,
    right_elbow: Point3D,
    right_wrist: Point3D,
    hand_wrist: Point3D,
    thumb_tip: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
    middle_tip: Point3D | None = None,
    middle_pip: Point3D | None = None,
    right_elbow_visibility: float = 1.0,
    right_wrist_visibility: float = 1.0,
    thumb_tip_visibility: float = 1.0,
    hand_wrist_visibility: float = 1.0,
    index_mcp_visibility: float = 1.0,
    middle_mcp_visibility: float = 1.0,
    ring_mcp_visibility: float = 1.0,
    pinky_mcp_visibility: float = 1.0,
    right_shoulder_visibility: float = 1.0,
) -> Motor6Command:
    """根据右手掌面与拇指方向生成 6 号电机左右旋转指令。

    输入：
    - 躯干点：左右肩、左右髋，用于构建躯干坐标系；
    - 右臂点：右肘、右腕，用于确定前臂轴；
    - 手部点：腕、拇指、食指/中指/无名指/小指 MCP，用于拟合掌面；
    - visibility：各关键点可见性，用于遮挡判断和置信度计算。

    输出：
    - `Motor6Command`，包含 6 号电机 roll 角、方向、置信度和动作允许状态。

    功能实现：
    - A/B/C 三路独立估计手腕滚转角；
    - 按置信度做加权融合；
    - 短时遮挡时使用卡尔曼预测保持平滑；
    - 遮挡过久时返回无效命令。

    A 路（完整）：Pose 肘+腕 + Hands 关键点 → 躯干坐标系滚转角。
    B 路（纯 Hands）：WRIST + 4 个 MCP 点 → 世界坐标系滚转角。
    C 路（参考平面 ⟂ 旋转轴）：MCP 点局域几何 → 消除身体倾斜退化。
    兜底（卡尔曼）：A/B/C 均失效且连续遮挡 ≤5 帧时输出预测值。
    """
    global _elbow_occluded_frames, _wrist_occluded_frames
    global _thumb_occluded_frames, _kalman_filter, _kalman_occluded_frames

    frame = build_body_frame(left_shoulder, right_shoulder, left_hip, right_hip)

    # ---- 遮挡检测（连续帧计数） ----
    if right_elbow_visibility < _OCCLUSION_THRESHOLD:
        _elbow_occluded_frames += 1
    else:
        _elbow_occluded_frames = 0
    if right_wrist_visibility < _OCCLUSION_THRESHOLD:
        _wrist_occluded_frames += 1
    else:
        _wrist_occluded_frames = 0
    if thumb_tip_visibility < _OCCLUSION_THRESHOLD:
        _thumb_occluded_frames += 1
    else:
        _thumb_occluded_frames = 0

    elbow_ok = _elbow_occluded_frames < _OCCLUSION_CONSECUTIVE_FRAMES
    wrist_ok = _wrist_occluded_frames < _OCCLUSION_CONSECUTIVE_FRAMES
    thumb_ok = _thumb_occluded_frames < _OCCLUSION_CONSECUTIVE_FRAMES
    pose_forearm_ok = elbow_ok and wrist_ok

    # ---- 数据容器 ----
    a_roll_raw: float | None = None
    a_confidence: float = 0.0
    a_palm_normal: np.ndarray | None = None
    a_roll_vector: np.ndarray | None = None
    a_thumb_vector: np.ndarray | None = None
    a_thumb_turn_deg: float | None = None
    a_thumb_plane_deg: float | None = None
    a_thumb_direction: Direction = Direction.UNKNOWN
    a_wrist_pitch_deg: float | None = None
    a_forearm_axis: np.ndarray | None = None

    b_roll_raw: float | None = None
    b_confidence: float = 0.0
    b_palm_normal: np.ndarray | None = None
    b_roll_vector: np.ndarray | None = None
    b_forearm_axis: np.ndarray | None = None
    b_wrist_pitch_deg: float | None = None

    c_result: dict | None = None

    # ====== A 路：Pose 肘+腕可用 ======
    if pose_forearm_ok:
        forearm_axis = normalize(to_vec(right_wrist) - to_vec(right_elbow))
        if forearm_axis is not None:
            a_forearm_axis = forearm_axis
            palm_normal = fit_plane_normal(
                [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp],
            )
            if palm_normal is not None:
                palm_normal = orient_palm_normal(
                    palm_normal,
                    hand_wrist=hand_wrist,
                    index_mcp=index_mcp,
                    middle_mcp=middle_mcp,
                    ring_mcp=ring_mcp,
                    pinky_mcp=pinky_mcp,
                )
                a_palm_normal = palm_normal

                thumb_vector = normalize(to_vec(thumb_tip) - to_vec(hand_wrist))
                a_thumb_vector = thumb_vector
                thumb_in_palm: np.ndarray | None = None

                if thumb_vector is not None:
                    thumb_in_palm = normalize(
                        thumb_vector - float(np.dot(thumb_vector, palm_normal)) * palm_normal
                    )

                palm_axis = project_to_plane(to_vec(index_mcp) - to_vec(pinky_mcp), palm_normal)
                if palm_axis is None:
                    palm_axis = project_to_plane(to_vec(middle_mcp) - to_vec(hand_wrist), palm_normal)
                if palm_axis is not None:
                    if thumb_in_palm is not None:
                        palm_face_hint = normalize(np.cross(thumb_in_palm, palm_axis))
                        if palm_face_hint is not None and float(np.dot(palm_normal, palm_face_hint)) < 0.0:
                            palm_normal = -palm_normal
                            a_palm_normal = palm_normal

                    palm_up_axis = project_to_plane(frame.up, forearm_axis)
                    palm_normal_axis = project_to_plane(palm_normal, forearm_axis)
                    if palm_up_axis is not None and palm_normal_axis is not None:
                        if float(np.dot(palm_normal_axis, palm_up_axis)) < 0.0:
                            palm_normal_axis = -palm_normal_axis

                        raw_signed_roll_deg = signed_angle_deg(palm_up_axis, palm_normal_axis, forearm_axis)
                        if raw_signed_roll_deg is not None:
                            a_roll_raw = float(raw_signed_roll_deg)
                            a_roll_vector = palm_normal_axis

                            if thumb_in_palm is not None:
                                thumb_turn_deg = signed_angle_deg(palm_axis, thumb_in_palm, palm_normal)
                                if thumb_turn_deg is not None:
                                    a_thumb_turn_deg = float(thumb_turn_deg)
                                    a_thumb_plane_deg = abs(float(thumb_turn_deg))
                                    thumb_right_component = float(np.dot(thumb_in_palm, frame.right))
                                    a_thumb_direction = (
                                        Direction.RIGHT if thumb_right_component >= 0.0 else Direction.LEFT
                                    )

                            pose_vis_count = sum(1 for v in (
                                right_elbow_visibility, right_shoulder_visibility, right_wrist_visibility
                            ) if v > _OCCLUSION_THRESHOLD)
                            a_confidence = {3: 0.9, 2: 0.5}.get(pose_vis_count, 0.0)

                            _pitch_pt = middle_tip
                            if _pitch_pt is None and middle_pip is not None:
                                _pitch_pt = middle_pip
                            if _pitch_pt is not None:
                                pitch_val, _ = _compute_wrist_pitch(
                                    forearm_axis, _pitch_pt, hand_wrist, frame.up,
                                )
                                a_wrist_pitch_deg = pitch_val

    # ====== B 路：纯 Hands ======
    b_roll_raw, b_forearm_axis, b_normal_axis, b_palm_normal = _compute_wrist_roll_hands_only(
        hand_wrist=hand_wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
        world_up=frame.up,
    )
    if b_roll_raw is not None and b_forearm_axis is not None and b_normal_axis is not None:
        b_roll_vector = b_normal_axis
        b_palm_normal = b_palm_normal

        mcp_vis_count = sum(1 for v in (
            hand_wrist_visibility, index_mcp_visibility, middle_mcp_visibility,
            ring_mcp_visibility, pinky_mcp_visibility,
        ) if v > _OCCLUSION_THRESHOLD)
        b_confidence = {5: 0.85, 4: 0.6, 3: 0.3}.get(mcp_vis_count, 0.0)

        _pitch_pt = middle_tip
        if _pitch_pt is None and middle_pip is not None:
            _pitch_pt = middle_pip
        if _pitch_pt is not None:
            pitch_val, _ = _compute_wrist_pitch(
                b_forearm_axis, _pitch_pt, hand_wrist, frame.up,
            )
            b_wrist_pitch_deg = pitch_val

    # ====== C 路：参考平面 ⟂ 旋转轴 ======
    c_result = _compute_wrist_roll_c_path(
        wrist=hand_wrist,
        middle_mcp=middle_mcp,
        index_mcp=index_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
        thumb_tip=thumb_tip,
        wrist_vis=hand_wrist_visibility,
        middle_mcp_vis=middle_mcp_visibility,
        index_mcp_vis=index_mcp_visibility,
        ring_mcp_vis=ring_mcp_visibility,
        pinky_mcp_vis=pinky_mcp_visibility,
        thumb_tip_vis=thumb_tip_visibility,
    )

    # ====== A/B/C 置信度加权融合 ======
    a_weight = a_confidence if a_roll_raw is not None else 0.0
    b_weight = b_confidence if b_roll_raw is not None else 0.0
    c_weight = (c_result.get("confidence", 0.0)
                if (c_result is not None and c_result.get("reason") == "ok_c_path")
                else 0.0)

    path_entries = []
    if a_weight > 0 and a_roll_raw is not None:
        path_entries.append(("a", a_weight, a_roll_raw))
    if b_weight > 0 and b_roll_raw is not None:
        path_entries.append(("b", b_weight, b_roll_raw))
    if c_weight > 0 and c_result is not None:
        path_entries.append(("c", c_weight, c_result["roll_deg"]))

    total_weight = sum(w for _, w, _ in path_entries)
    result: Motor6Command | None = None

    if total_weight > _EPS and path_entries:
        fused_roll_raw = -sum(w * r for _, w, r in path_entries) / total_weight

        path_entries.sort(key=lambda x: x[1], reverse=True)
        dominant_path = path_entries[0][0]

        roll_deadband_deg = 1.0
        if abs(fused_roll_raw) < roll_deadband_deg:
            fused_direction = Direction.UNKNOWN
            fused_rotation_side = "neutral"
            fused_signed_roll = 0.0
        elif fused_roll_raw < 0.0:
            fused_direction = Direction.RIGHT
            fused_rotation_side = "inward"
            fused_signed_roll = -abs(fused_roll_raw)
        else:
            fused_direction = Direction.LEFT
            fused_rotation_side = "outward"
            fused_signed_roll = abs(fused_roll_raw)

        # 辅助数据取主导路
        if dominant_path == "a":
            fused_palm_normal = a_palm_normal
            fused_roll_vector = a_roll_vector
            fused_thumb_vector = a_thumb_vector
            fused_thumb_turn_deg = a_thumb_turn_deg
            fused_thumb_plane_deg = a_thumb_plane_deg
            fused_thumb_direction = a_thumb_direction
            fused_wrist_pitch = a_wrist_pitch_deg
            fused_forearm = a_forearm_axis
            fused_confidence = a_confidence
        elif dominant_path == "b":
            fused_palm_normal = b_palm_normal
            fused_roll_vector = b_roll_vector
            fused_thumb_vector = None
            fused_thumb_turn_deg = None
            fused_thumb_plane_deg = None
            fused_thumb_direction = Direction.UNKNOWN
            fused_wrist_pitch = b_wrist_pitch_deg
            fused_forearm = b_forearm_axis
            fused_confidence = b_confidence
        else:  # c
            fused_palm_normal = c_result.get("n_palm") if c_result else None
            fused_roll_vector = c_result.get("n_proj") if c_result else None
            fused_thumb_vector = None
            fused_thumb_turn_deg = None
            fused_thumb_plane_deg = None
            fused_thumb_direction = Direction.UNKNOWN
            fused_wrist_pitch = None
            fused_forearm = None
            fused_confidence = c_result.get("confidence", 0.0) if c_result else 0.0

        active_paths = "".join(p for p, _, _ in path_entries)
        fused_reason = f"fused_{active_paths}"

        wrist_pitch_deg = a_wrist_pitch_deg if a_wrist_pitch_deg is not None else b_wrist_pitch_deg

        result = Motor6Command(
            valid=True,
            roll_deg=abs(float(fused_roll_raw)),
            signed_roll_deg=float(fused_signed_roll),
            raw_signed_roll_deg=float(fused_roll_raw),
            thumb_plane_deg=float(fused_thumb_plane_deg) if fused_thumb_plane_deg is not None else None,
            thumb_turn_deg=float(fused_thumb_turn_deg) if fused_thumb_turn_deg is not None else None,
            direction=fused_direction,
            thumb_direction=fused_thumb_direction,
            rotation_side=fused_rotation_side,
            palm_normal=Point3D(
                x=float(fused_palm_normal[0]),  # type: ignore[union-attr]
                y=float(fused_palm_normal[1]),  # type: ignore[union-attr]
                z=float(fused_palm_normal[2]),  # type: ignore[union-attr]
            ) if fused_palm_normal is not None else None,
            thumb_vector=Point3D(
                x=float(fused_thumb_vector[0]),  # type: ignore[union-attr]
                y=float(fused_thumb_vector[1]),  # type: ignore[union-attr]
                z=float(fused_thumb_vector[2]),  # type: ignore[union-attr]
            ) if fused_thumb_vector is not None else None,
            roll_vector=Point3D(
                x=float(fused_roll_vector[0]),  # type: ignore[union-attr]
                y=float(fused_roll_vector[1]),  # type: ignore[union-attr]
                z=float(fused_roll_vector[2]),  # type: ignore[union-attr]
            ) if fused_roll_vector is not None else None,
            wrist_pitch_deg=wrist_pitch_deg,
            confidence=fused_confidence,
            motion_allowed=True,
            reason=fused_reason,
        )

    # ---- 卡尔曼滤波短期遮挡缓冲 ----
    if result is not None and result.valid:
        _kalman_filter.update(float(result.signed_roll_deg))
        _kalman_occluded_frames = 0
        return result

    _kalman_occluded_frames += 1
    if _kalman_occluded_frames <= _MAX_PREDICTION_FRAMES:
        pred_roll = _kalman_filter.predict()
        pred_direction = Direction.UNKNOWN
        pred_side = "neutral"
        if abs(pred_roll) >= 1.0:
            if pred_roll < 0.0:
                pred_direction = Direction.RIGHT
                pred_side = "inward"
            else:
                pred_direction = Direction.LEFT
                pred_side = "outward"

        return Motor6Command(
            valid=True,
            roll_deg=abs(float(pred_roll)),
            signed_roll_deg=float(pred_roll),
            raw_signed_roll_deg=float(pred_roll),
            thumb_plane_deg=None,
            thumb_turn_deg=None,
            direction=pred_direction,
            thumb_direction=Direction.UNKNOWN,
            rotation_side=pred_side,
            palm_normal=None,
            thumb_vector=None,
            roll_vector=None,
            confidence=None,
            motion_allowed=False,
            reason=f"kalman_prediction_frame_{_kalman_occluded_frames}",
        )

    return Motor6Command(reason="right_wrist_occluded_too_long")
