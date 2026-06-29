from __future__ import annotations

"""3D pose angle solver.

Coordinate convention:
- torso frame: right / up / forward
- shoulder flexion: sagittal-plane elevation
- shoulder abduction: frontal-plane elevation
- shoulder rotation: twist around the upper-arm axis
- elbow flexion: 180 - angle(upper arm, forearm)
"""

from dataclasses import dataclass
from typing import Optional

import numpy as np

from app.models import ArmAngles, Direction, JointRotation, Motor6Command, Point3D, Side

_EPS = 1e-8


@dataclass(frozen=True)
class _BodyFrame:
    right: np.ndarray
    up: np.ndarray
    forward: np.ndarray


def _to_vec(point: Point3D) -> np.ndarray:
    return np.array([float(point.x), float(point.y), float(point.z)], dtype=np.float64)


def _norm(vec: np.ndarray) -> float:
    return float(np.linalg.norm(vec))


def _normalize(vec: np.ndarray, fallback: np.ndarray | None = None) -> np.ndarray | None:
    length = _norm(vec)
    if length <= _EPS:
        if fallback is None:
            return None
        return fallback.astype(np.float64, copy=True)
    return vec / length


def _project_to_plane(vec: np.ndarray, axis: np.ndarray) -> np.ndarray | None:
    axis_n = _normalize(axis)
    if axis_n is None:
        return None
    projected = vec - float(np.dot(vec, axis_n)) * axis_n
    return _normalize(projected)


def _fit_plane_normal(points: list[Point3D]) -> np.ndarray | None:
    """用多个手掌关键点拟合掌面法向量。

    这里只求几何法向量，不再强制朝向身体上方。
    """
    if len(points) < 3:
        return None

    matrix = np.array([_to_vec(point) for point in points], dtype=np.float64)
    centered = matrix - matrix.mean(axis=0)
    try:
        _, _, vh = np.linalg.svd(centered, full_matrices=False)
    except np.linalg.LinAlgError:
        return None

    normal = _normalize(vh[-1])
    if normal is None:
        return None

    return normal


def _orient_palm_normal(
    normal: np.ndarray,
    hand_wrist: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
) -> np.ndarray | None:
    """用手掌内部几何结构给掌面法向定向。"""
    wrist = _to_vec(hand_wrist)
    hints: list[np.ndarray] = []
    for a, b in (
        (index_mcp, middle_mcp),
        (middle_mcp, ring_mcp),
        (ring_mcp, pinky_mcp),
        (index_mcp, pinky_mcp),
    ):
        hint = np.cross(_to_vec(a) - wrist, _to_vec(b) - wrist)
        if _norm(hint) > _EPS:
            hints.append(hint)

    if not hints:
        return normal

    hint_sum = np.sum(hints, axis=0)
    hint_unit = _normalize(hint_sum)
    if hint_unit is None:
        return normal
    if float(np.dot(normal, hint_unit)) < 0.0:
        return -normal
    return normal


# ============================================================
# 方案二/方案三：B 路纯 Hands 计算 + 卡尔曼滤波状态
# ============================================================

# 方案二：关键点遮挡帧计数器（连续帧低于阈值才判定遮挡，避免单帧噪声误切换）
_elbow_occluded_frames = 0
_wrist_occluded_frames = 0
_thumb_occluded_frames = 0
_OCCLUSION_THRESHOLD = 0.5
_OCCLUSION_CONSECUTIVE_FRAMES = 3

# 方案三：卡尔曼滤波器（1D，用于 signed_roll_deg 短期遮挡预测）
class _KalmanFilter1D:
    """简单一维卡尔曼滤波器，用于手腕滚转角短期遮挡缓冲。"""

    def __init__(self, process_noise: float = 0.1, measurement_noise: float = 0.5):
        self.x = 0.0          # 状态估计
        self.p = 1.0          # 误差协方差
        self.q = process_noise      # 过程噪声
        self.r = measurement_noise  # 测量噪声
        self._initialized = False

    def update(self, measurement: float) -> float:
        if not self._initialized:
            self.x = measurement
            self.p = 1.0
            self._initialized = True
            return self.x
        # 预测
        self.p += self.q
        # 更新
        k = self.p / (self.p + self.r)
        self.x += k * (measurement - self.x)
        self.p *= (1.0 - k)
        return self.x

    def predict(self) -> float:
        if not self._initialized:
            return 0.0
        self.p += self.q
        return self.x


_kalman_filter = _KalmanFilter1D()
_kalman_occluded_frames = 0
_MAX_PREDICTION_FRAMES = 5


def _compute_hand_palm_center(
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
) -> np.ndarray:
    """方案二：计算手掌中心 —— 4 个 MCP 点（5/9/13/17）的质心。

    作为 B 路纯 Hands 方案中 forearm_axis 的终点近似。
    """
    vecs = [_to_vec(p) for p in (index_mcp, middle_mcp, ring_mcp, pinky_mcp)]
    return np.mean(vecs, axis=0)


def _compute_wrist_roll_hands_only(
    *,
    hand_wrist: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
    world_up: np.ndarray | None = None,
) -> tuple[float | None, np.ndarray | None, np.ndarray | None, np.ndarray | None]:
    """方案二 B 路：纯 MediaPipe Hands 关键点计算手腕滚转角。

    完全不依赖 Pose 的任何关键点（肘、肩、髋等），仅使用 Hands 21 点中的：
    - WRIST(0)、INDEX_MCP(5)、MIDDLE_MCP(9)、RING_MCP(13)、PINKY_MCP(17)

    算法（参照 HiWonder 项目已验证方案）：
    1. 前臂轴近似：WRIST → 手掌中心（4 个 MCP 点质心）
    2. 掌面法向量：SVD 拟合 + _orient_palm_normal 定向（复用现有函数）
    3. 0° 参考：世界/躯干 UP 投影到前臂轴垂直面
    4. 有符号滚转角：palm_normal 投影到垂直面，与 0° 参考做 _signed_angle_deg

    Args:
        hand_wrist: Hands WRIST(0) 坐标
        index_mcp: Hands INDEX_FINGER_MCP(5) 坐标
        middle_mcp: Hands MIDDLE_FINGER_MCP(9) 坐标
        ring_mcp: Hands RING_FINGER_MCP(13) 坐标
        pinky_mcp: Hands PINKY_MCP(17) 坐标
        world_up: 参考上方向，默认世界坐标系 [0, 1, 0]

    Returns:
        (signed_roll_deg, forearm_axis, palm_normal_axis, palm_normal)
        各分量返回 None 表示计算失败
    """
    if world_up is None:
        world_up = np.array([0.0, 1.0, 0.0], dtype=np.float64)

    # 1. 前臂轴近似：WRIST → 手掌中心（4 个 MCP 点的质心）
    palm_center = _compute_hand_palm_center(index_mcp, middle_mcp, ring_mcp, pinky_mcp)
    wrist_vec = _to_vec(hand_wrist)
    forearm_axis = _normalize(palm_center - wrist_vec)
    if forearm_axis is None:
        return None, None, None, None

    # 2. 掌面法向量：SVD 拟合 + 定向（复用现有函数）
    palm_normal = _fit_plane_normal(
        [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp],
    )
    if palm_normal is None:
        return None, None, None, None

    palm_normal = _orient_palm_normal(
        palm_normal,
        hand_wrist=hand_wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
    )

    # 3. 0° 参考方向：UP 投影到 forearm_axis 垂直面
    palm_up_axis = _project_to_plane(world_up, forearm_axis)
    if palm_up_axis is None:
        return None, None, None, None

    # 4. 掌面法向投影到 forearm_axis 垂直面
    palm_normal_axis = _project_to_plane(palm_normal, forearm_axis)
    if palm_normal_axis is None:
        return None, None, None, None

    # 5. 对齐方向：确保 palm_normal_axis 朝向 palm_up_axis 同一侧
    if float(np.dot(palm_normal_axis, palm_up_axis)) < 0.0:
        palm_normal_axis = -palm_normal_axis

    # 6. 有符号滚转角
    signed_roll_deg = _signed_angle_deg(palm_up_axis, palm_normal_axis, forearm_axis)
    if signed_roll_deg is None:
        return None, None, None, None

    return float(signed_roll_deg), forearm_axis, palm_normal_axis, palm_normal


# ============================================================
# C 路：参考平面 ⟂ 旋转轴（纯 Hands，局域几何）
# ============================================================

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

    几何模型（详见 wrist_roll_optimized_feasibility.md）：
    - 旋转轴 r = middle_mcp - wrist，r_hat = r / |r|
    - 参考平面：垂直于 r_hat，过腕关节点
    - 手掌平面：SVD 拟合 wrist + 4 个 MCP 共 5 点，得法向量 n_palm
    - n_proj = n_palm - (n_palm · r_hat) · r_hat，归一化（手掌法向量在参考平面上的投影）
    - n_zero = Y_world - (Y_world · r_hat) · r_hat，归一化（世界 Y 在参考平面上的投影）
    - θ_raw = atan2(|n_proj × n_zero|, n_proj · n_zero)，范围 [0°, 180°]
    - sign = sign((n_proj × n_zero) · r_hat)
    - roll_deg = sign · θ_raw · 180/π

    方向判定（与 A 路映射一致）：
    - t = thumb_tip - wrist，t_proj = t - (t · r_hat) · r_hat
    - t_cross_n = (t_proj × n_zero) · r_hat
    - 拇指辅助判定掌心朝内/朝外，符号回退到 roll_deg 的正负

    可见性要求：
    - middle_mcp 必须可见（旋转轴必需），否则返回 reason="c_unavailable_no_axis"
    - 5 个 MCP 点 + wrist 中至少 3 个 visibility > 0.5

    Returns:
        dict with keys: roll_deg, reason, confidence, rotation_side,
                        n_palm, n_proj, n_zero
        或 None（关键点不足）
    """
    # ---- 可见性检查 ----
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

    # ---- 1. 旋转轴 r = middle_mcp - wrist ----
    wrist_v = _to_vec(wrist)
    middle_v = _to_vec(middle_mcp)
    r_vec = middle_v - wrist_v
    r_hat = _normalize(r_vec)
    if r_hat is None:
        return None

    Y_world = np.array([0.0, 1.0, 0.0], dtype=np.float64)

    # 旋转轴接近竖直时 n_zero 退化 → 后续降权
    axis_verticality = abs(float(np.dot(r_hat, Y_world)))
    near_vertical = axis_verticality > 0.95

    # ---- 2. 手掌平面 SVD 拟合 + 定向 ----
    n_palm = _fit_plane_normal([wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp])
    if n_palm is None:
        return None

    n_palm = _orient_palm_normal(
        n_palm,
        hand_wrist=wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
    )

    # ---- 3. n_proj: n_palm 在参考平面（⟂ r_hat）上的投影 ----
    n_proj_raw = n_palm - float(np.dot(n_palm, r_hat)) * r_hat
    n_proj = _normalize(n_proj_raw)
    if n_proj is None:
        return None

    # ---- 4. n_zero: Y_world 在参考平面上的投影 ----
    n_zero_raw = Y_world - float(np.dot(Y_world, r_hat)) * r_hat
    n_zero = _normalize(n_zero_raw)
    if n_zero is None:
        return None

    # ---- 5. θ_raw = atan2(|n_proj × n_zero|, n_proj · n_zero) ----
    cross_np_nz = np.cross(n_proj, n_zero)
    cross_norm = float(np.linalg.norm(cross_np_nz))
    dot_np_nz = float(np.clip(np.dot(n_proj, n_zero), -1.0, 1.0))
    theta_raw = float(np.degrees(np.arctan2(cross_norm, dot_np_nz)))

    # ---- 6. 符号：sign((n_proj × n_zero) · r_hat) ----
    sign_val = float(np.sign(np.dot(cross_np_nz, r_hat)))
    if abs(sign_val) < _EPS:
        sign_val = 1.0
    roll_deg = sign_val * theta_raw

    # ---- 7. 方向判定（与 A 路映射一致：roll<0 → "outward"，roll>0 → "inward"） ----
    # 拇指辅助验证
    if thumb_tip_vis > 0.5:
        t_vec = _to_vec(thumb_tip) - wrist_v
        t_proj_raw = t_vec - float(np.dot(t_vec, r_hat)) * r_hat
        t_proj = _normalize(t_proj_raw)
        if t_proj is not None:
            # t_cross_n = (t_proj × n_zero) · r_hat
            # > 0 → thumb 在 n_zero 逆时针侧
            t_cross_n = float(np.dot(np.cross(t_proj, n_zero), r_hat))
            # 使用 thumb 微调：若 thumb 位置与 roll 符号冲突，信任 roll_deg
            _thumb_hint = "outward" if t_cross_n > 0 else "inward"

    if abs(roll_deg) < 1.0:
        rotation_side = "neutral"
    elif roll_deg < 0.0:
        rotation_side = "outward"
    else:
        rotation_side = "inward"

    # ---- 8. 置信度：可见点数 + SVD 残差 ----
    base_conf = {5: 0.85, 4: 0.6, 3: 0.3}.get(visible_count, 0.0)

    # SVD 残差（最小奇异值占比越小 → 平面拟合越好）
    points_for_svd = [_to_vec(p) for p in (wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp)]
    matrix = np.array(points_for_svd, dtype=np.float64)
    centered = matrix - matrix.mean(axis=0)
    try:
        _, s, _ = np.linalg.svd(centered, full_matrices=False)
        if s.sum() > _EPS:
            svd_residual = float(s[-1] / s.sum())
        else:
            svd_residual = 1.0
    except (np.linalg.LinAlgError, IndexError):
        svd_residual = 1.0

    svd_factor = float(np.clip(1.0 - svd_residual * 5.0, 0.0, 1.0))
    confidence = base_conf * svd_factor

    # 旋转轴接近竖直 → n_zero 退化 → 降权
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


# ============================================================
# 手腕上下识别：wrist pitch / flexion-extension
# ============================================================

def _compute_wrist_pitch(
    forearm_axis: np.ndarray,
    hand_middle_tip: Point3D,
    hand_wrist: Point3D,
    frame_up: np.ndarray | None = None,
) -> tuple[float | None, np.ndarray | None]:
    """计算手腕前后倾角（pitch / flexion-extension）。

    手腕上下识别——判断手掌方向相对于前臂的上下偏离：
    - 手掌与前臂平直（手掌延长线与前臂轴重合）→ 0°
    - 手腕向上弯曲（手背方向，伸展/dorsiflexion）→ 正角度
    - 手腕向下弯曲（掌心方向，屈曲/palmar flexion）→ 负角度

    算法：
    1. palm_direction = normalize(middle_tip - wrist) —— 中指方向代表手掌指向
    2. 计算 palm_direction 相对于前臂反方向（wrist→elbow 方向）的偏离角
    3. 用 frame_up 投影到前臂垂直面作为"上"参考，判定符号

    Args:
        forearm_axis: 前臂轴单位向量（肘→腕方向）
        hand_middle_tip: Hands MIDDLE_FINGER_TIP(12) 的 3D 坐标
        hand_wrist: Hands WRIST(0) 的 3D 坐标
        frame_up: 躯干/世界 up 方向（用于有符号判定），默认为世界 [0,1,0]

    Returns:
        (pitch_deg, palm_direction) 或 (None, None)
    """
    if frame_up is None:
        frame_up = np.array([0.0, 1.0, 0.0], dtype=np.float64)

    wrist_vec = _to_vec(hand_wrist)
    palm_direction = _normalize(_to_vec(hand_middle_tip) - wrist_vec)
    if palm_direction is None:
        return None, None

    # 手掌平直时 palm_direction 与 forearm_axis（elbow→wrist）同向，cos ≈ 1 → 0°
    # 手腕上弯（手背方向/伸展）→ 正角度；下弯（掌心方向/屈曲）→ 负角度
    cos_angle = float(np.clip(np.dot(palm_direction, forearm_axis), -1.0, 1.0))

    # 手腕弯曲旋转轴：forearm_axis × up_proj（右手定则，正角度 = 向上弯）
    up_proj = _project_to_plane(frame_up, forearm_axis)
    if up_proj is None:
        # frame_up 平行于前臂，无法判定符号，返回绝对角度
        pitch_abs = float(np.degrees(np.arccos(cos_angle)))
        return pitch_abs, palm_direction

    rotation_axis = _normalize(np.cross(forearm_axis, up_proj))
    if rotation_axis is None:
        pitch_abs = float(np.degrees(np.arccos(cos_angle)))
        return pitch_abs, palm_direction

    sin_component = float(np.dot(rotation_axis, np.cross(forearm_axis, palm_direction)))
    pitch_deg = float(np.degrees(np.arctan2(sin_component, cos_angle)))

    return pitch_deg, palm_direction


def _angle_between_deg(a: np.ndarray, b: np.ndarray) -> Optional[float]:
    a_n = _normalize(a)
    b_n = _normalize(b)
    if a_n is None or b_n is None:
        return None
    cosine = float(np.clip(np.dot(a_n, b_n), -1.0, 1.0))
    return float(np.degrees(np.arccos(cosine)))


def _signed_angle_deg(a: np.ndarray, b: np.ndarray, axis: np.ndarray) -> Optional[float]:
    a_n = _normalize(a)
    b_n = _normalize(b)
    axis_n = _normalize(axis)
    if a_n is None or b_n is None or axis_n is None:
        return None
    sine = float(np.dot(axis_n, np.cross(a_n, b_n)))
    cosine = float(np.clip(np.dot(a_n, b_n), -1.0, 1.0))
    return float(np.degrees(np.arctan2(sine, cosine)))


def _safe_zero_signed_angle(a: np.ndarray, b: np.ndarray, axis: np.ndarray) -> float:
    value = _signed_angle_deg(a, b, axis)
    return 0.0 if value is None else float(value)


def _orient_palm_normal(
    normal: np.ndarray,
    hand_wrist: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
) -> np.ndarray | None:
    """利用手掌内部的点序给掌面法向定向。

    SVD 拟合得到的法向存在正负二义性。这里用掌心多个相邻 MCP 的叉积作为提示，
    让同一只右手在翻转前后仍能保持一致的法向符号，避免方向总是判成同一侧。
    """
    wrist = _to_vec(hand_wrist)
    hints: list[np.ndarray] = []
    for a, b in (
        (index_mcp, middle_mcp),
        (middle_mcp, ring_mcp),
        (ring_mcp, pinky_mcp),
        (index_mcp, pinky_mcp),
    ):
        hint = np.cross(_to_vec(a) - wrist, _to_vec(b) - wrist)
        if _norm(hint) > _EPS:
            hints.append(hint)

    if not hints:
        return normal

    hint_sum = np.sum(hints, axis=0)
    hint_unit = _normalize(hint_sum)
    if hint_unit is None:
        return normal

    if float(np.dot(normal, hint_unit)) < 0.0:
        return -normal
    return normal


def _build_body_frame(
    left_shoulder: Point3D,
    right_shoulder: Point3D,
    left_hip: Point3D,
    right_hip: Point3D,
) -> _BodyFrame:
    shoulder_mid = (_to_vec(left_shoulder) + _to_vec(right_shoulder)) * 0.5
    hip_mid = (_to_vec(left_hip) + _to_vec(right_hip)) * 0.5

    up = _normalize(shoulder_mid - hip_mid, np.array([0.0, -1.0, 0.0], dtype=np.float64))
    if up is None:
        up = np.array([0.0, -1.0, 0.0], dtype=np.float64)

    right = _normalize(_to_vec(right_shoulder) - _to_vec(left_shoulder), np.array([1.0, 0.0, 0.0], dtype=np.float64))
    if right is None:
        right = np.array([1.0, 0.0, 0.0], dtype=np.float64)

    forward = _normalize(np.cross(right, up), np.array([0.0, 0.0, 1.0], dtype=np.float64))
    if forward is None:
        forward = np.array([0.0, 0.0, 1.0], dtype=np.float64)

    right = _normalize(np.cross(up, forward), right)
    if right is None:
        right = np.array([1.0, 0.0, 0.0], dtype=np.float64)
    up = _normalize(np.cross(forward, right), up)
    if up is None:
        up = np.array([0.0, -1.0, 0.0], dtype=np.float64)

    return _BodyFrame(right=right, up=up, forward=forward)


def _summary_rotation(vector: np.ndarray | None, frame: _BodyFrame, side_sign: float) -> JointRotation:
    if vector is None:
        return JointRotation(
            vertical=Direction.UNKNOWN,
            vertical_deg=None,
            horizontal=Direction.UNKNOWN,
            horizontal_deg=None,
        )

    vec = _normalize(vector)
    if vec is None:
        return JointRotation(
            vertical=Direction.UNKNOWN,
            vertical_deg=None,
            horizontal=Direction.UNKNOWN,
            horizontal_deg=None,
        )

    vertical_component = float(np.dot(vec, frame.up))
    horizontal_component = float(side_sign * np.dot(vec, frame.right))
    depth_component = float(np.dot(vec, frame.forward))

    vertical = Direction.UP if vertical_component >= 0 else Direction.DOWN
    horizontal = Direction.RIGHT if horizontal_component >= 0 else Direction.LEFT

    vertical_deg = float(
        np.degrees(np.arctan2(np.sqrt(horizontal_component**2 + depth_component**2), abs(vertical_component)))
    )
    horizontal_deg = float(
        np.degrees(np.arctan2(np.sqrt(vertical_component**2 + depth_component**2), abs(horizontal_component)))
    )

    return JointRotation(
        vertical=vertical,
        vertical_deg=vertical_deg,
        horizontal=horizontal,
        horizontal_deg=horizontal_deg,
    )


def _solve_arm(
    *,
    side: Side,
    shoulder: Point3D,
    elbow: Point3D,
    wrist: Point3D,
    hand_ref: Point3D | None,
    frame: _BodyFrame,
) -> ArmAngles:
    side_sign = 1.0 if side == Side.RIGHT else -1.0

    shoulder_vec = _to_vec(elbow) - _to_vec(shoulder)
    forearm_vec = _to_vec(wrist) - _to_vec(elbow)
    hand_vec = _to_vec(hand_ref) - _to_vec(wrist) if hand_ref is not None else None

    shoulder_unit = _normalize(shoulder_vec)
    forearm_unit = _normalize(forearm_vec)
    hand_unit = _normalize(hand_vec) if hand_vec is not None else None

    vertical = Direction.UNKNOWN
    horizontal = Direction.UNKNOWN
    if shoulder_unit is not None:
        vertical_component = float(np.dot(shoulder_unit, frame.up))
        horizontal_component = float(side_sign * np.dot(shoulder_unit, frame.right))
        vertical = Direction.UP if vertical_component >= 0 else Direction.DOWN
        horizontal = Direction.RIGHT if horizontal_component >= 0 else Direction.LEFT

    shoulder_rotation = _summary_rotation(shoulder_unit, frame, side_sign)
    arm_rotation = _summary_rotation(forearm_unit, frame, side_sign)
    wrist_rotation = _summary_rotation(hand_unit, frame, side_sign) if hand_unit is not None else None

    down = -frame.up
    flex_axis = -side_sign * frame.right
    abd_axis = side_sign * frame.forward

    shoulder_flex_deg = None
    shoulder_abduction_deg = None
    if shoulder_unit is not None:
        flex_proj = _project_to_plane(shoulder_unit, flex_axis)
        abd_proj = _project_to_plane(shoulder_unit, abd_axis)
        if flex_proj is not None:
            shoulder_flex_deg = abs(_safe_zero_signed_angle(down, flex_proj, flex_axis))
        else:
            shoulder_flex_deg = 0.0
        if abd_proj is not None:
            shoulder_abduction_deg = abs(_safe_zero_signed_angle(down, abd_proj, abd_axis))
        else:
            shoulder_abduction_deg = 0.0

    shoulder_rotation_deg = None
    if shoulder_unit is not None and forearm_unit is not None:
        arm_normal = _normalize(np.cross(shoulder_unit, forearm_unit))
        if arm_normal is not None:
            ref_axis = frame.forward
            if abs(float(np.dot(shoulder_unit, ref_axis))) > 0.85:
                ref_axis = frame.right
            ref_normal = _normalize(np.cross(shoulder_unit, ref_axis))
            if ref_normal is None:
                ref_axis = frame.up
                ref_normal = _normalize(np.cross(shoulder_unit, ref_axis))
            if ref_normal is not None:
                shoulder_rotation_deg = _safe_zero_signed_angle(ref_normal, arm_normal, shoulder_unit)

    elbow_flex_deg = None
    if shoulder_unit is not None and forearm_unit is not None:
        elbow_angle = _angle_between_deg(-shoulder_unit, forearm_unit)
        if elbow_angle is not None:
            elbow_flex_deg = float(np.clip(180.0 - elbow_angle, 0.0, 180.0))

    wrist_flex_deg = None
    if forearm_unit is not None and hand_unit is not None:
        wrist_angle = _angle_between_deg(-forearm_unit, hand_unit)
        if wrist_angle is not None:
            wrist_flex_deg = float(np.clip(180.0 - wrist_angle, 0.0, 180.0))

    return ArmAngles(
        side=side,
        shoulder_deg=shoulder_flex_deg,
        elbow_deg=elbow_flex_deg,
        wrist_deg=wrist_flex_deg,
        vertical=vertical,
        horizontal=horizontal,
        shoulder_rotation=shoulder_rotation,
        arm_rotation=arm_rotation,
        wrist_rotation=wrist_rotation,
        shoulder_flex_deg=shoulder_flex_deg,
        shoulder_abduction_deg=shoulder_abduction_deg,
        shoulder_rotation_deg=shoulder_rotation_deg,
        elbow_flex_deg=elbow_flex_deg,
        wrist_flex_deg=wrist_flex_deg,
    )


def compute_arm_angles(
    *,
    left_shoulder: Point3D,
    left_elbow: Point3D,
    left_wrist: Point3D,
    left_hip: Point3D,
    right_shoulder: Point3D,
    right_elbow: Point3D,
    right_wrist: Point3D,
    right_hip: Point3D,
    left_hand_ref: Point3D | None = None,
    right_hand_ref: Point3D | None = None,
) -> list[ArmAngles]:
    frame = _build_body_frame(left_shoulder, right_shoulder, left_hip, right_hip)

    return [
        _solve_arm(
            side=Side.LEFT,
            shoulder=left_shoulder,
            elbow=left_elbow,
            wrist=left_wrist,
            hand_ref=left_hand_ref,
            frame=frame,
        ),
        _solve_arm(
            side=Side.RIGHT,
            shoulder=right_shoulder,
            elbow=right_elbow,
            wrist=right_wrist,
            hand_ref=right_hand_ref,
            frame=frame,
        ),
    ]


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
    # 手腕上下识别：中指指尖/近端指间关键点
    middle_tip: Point3D | None = None,
    middle_pip: Point3D | None = None,
    # 方案二：关键点可见性（0~1），默认 1.0 保持向后兼容
    right_elbow_visibility: float = 1.0,
    right_wrist_visibility: float = 1.0,
    thumb_tip_visibility: float = 1.0,
    # C 路：MCP 关键点可见性（用于 C 路置信度与可见性检查）
    hand_wrist_visibility: float = 1.0,
    index_mcp_visibility: float = 1.0,
    middle_mcp_visibility: float = 1.0,
    ring_mcp_visibility: float = 1.0,
    pinky_mcp_visibility: float = 1.0,
    # A 路置信度：肩关节可见性
    right_shoulder_visibility: float = 1.0,
) -> Motor6Command:
    """根据右手掌面与拇指方向生成 6 号电机左右旋转指令。

    方案二：Pose 关键点遮挡时自动降级为纯 Hands（B 路）计算。
    方案三：短暂遮挡期间使用卡尔曼滤波预测填充。
    C 路：参考平面 ⟂ 旋转轴（纯局域几何），与 A/B 路置信度加权融合。

    A 路（完整）：Pose 肘+腕 + Hands 关键点 → 躯干坐标系滚转角。
    B 路（纯 Hands）：WRIST + 4 个 MCP 点 → 世界坐标系滚转角。
    C 路（参考平面 ⟂ 旋转轴）：MCP 点局域几何 → 消除身体倾斜退化。
    兜底（卡尔曼）：A/B/C 均失效且连续遮挡 ≤5 帧时输出预测值。
    """
    global _elbow_occluded_frames, _wrist_occluded_frames
    global _thumb_occluded_frames, _kalman_filter, _kalman_occluded_frames

    frame = _build_body_frame(left_shoulder, right_shoulder, left_hip, right_hip)

    # ---- 方案二：关键点遮挡检测（连续帧计数，避免单帧噪声误切换） ----
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

    # ================================================================
    # A/B/C 三路独立计算（各自提取 roll_deg、confidence、rotation_side）
    # ================================================================

    # ---- A 路数据容器 ----
    a_roll_raw: float | None = None       # 有符号原始滚转角（用于融合）
    a_roll_signed: float | None = None    # deadband 后的绝对值
    a_confidence: float = 0.0
    a_rotation_side: str = "neutral"
    a_direction: Direction = Direction.UNKNOWN
    a_palm_normal: np.ndarray | None = None
    a_roll_vector: np.ndarray | None = None
    a_thumb_vector: np.ndarray | None = None
    a_thumb_turn_deg: float | None = None
    a_thumb_plane_deg: float | None = None
    a_thumb_direction: Direction = Direction.UNKNOWN
    a_wrist_pitch_deg: float | None = None
    a_forearm_axis: np.ndarray | None = None
    a_reason: str = "ok"

    # ---- B 路数据容器 ----
    b_roll_raw: float | None = None
    b_roll_signed: float | None = None
    b_confidence: float = 0.0
    b_rotation_side: str = "neutral"
    b_direction: Direction = Direction.UNKNOWN
    b_palm_normal: np.ndarray | None = None
    b_roll_vector: np.ndarray | None = None
    b_forearm_axis: np.ndarray | None = None
    b_wrist_pitch_deg: float | None = None

    # ---- C 路数据容器 ----
    c_result: dict | None = None

    # ====== A 路：Pose 肘+腕可用，走原有完整路径 ======
    if pose_forearm_ok:
        forearm_axis = _normalize(_to_vec(right_wrist) - _to_vec(right_elbow))
        if forearm_axis is not None:
            a_forearm_axis = forearm_axis
            palm_index = _to_vec(index_mcp) - _to_vec(hand_wrist)
            palm_pinky = _to_vec(pinky_mcp) - _to_vec(hand_wrist)
            palm_normal = _fit_plane_normal(
                [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp],
            )
            if palm_normal is not None:
                palm_normal = _orient_palm_normal(
                    palm_normal,
                    hand_wrist=hand_wrist,
                    index_mcp=index_mcp,
                    middle_mcp=middle_mcp,
                    ring_mcp=ring_mcp,
                    pinky_mcp=pinky_mcp,
                )
                a_palm_normal = palm_normal

                # 拇指不可用时跳过拇指相关计算，不阻断 signed_roll_deg 输出
                thumb_vector = _normalize(_to_vec(thumb_tip) - _to_vec(hand_wrist))
                a_thumb_vector = thumb_vector
                thumb_in_palm: np.ndarray | None = None

                if thumb_vector is not None:
                    thumb_in_palm = _normalize(
                        thumb_vector - float(np.dot(thumb_vector, palm_normal)) * palm_normal
                    )

                palm_axis = _project_to_plane(_to_vec(index_mcp) - _to_vec(pinky_mcp), palm_normal)
                if palm_axis is None:
                    palm_axis = _project_to_plane(_to_vec(middle_mcp) - _to_vec(hand_wrist), palm_normal)
                if palm_axis is not None:
                    # 用拇指辅助掌面定向（仅拇指可用时）
                    if thumb_in_palm is not None:
                        palm_face_hint = _normalize(np.cross(thumb_in_palm, palm_axis))
                        if palm_face_hint is not None and float(np.dot(palm_normal, palm_face_hint)) < 0.0:
                            palm_normal = -palm_normal
                            a_palm_normal = palm_normal

                    palm_up_axis = _project_to_plane(frame.up, forearm_axis)
                    palm_normal_axis = _project_to_plane(palm_normal, forearm_axis)
                    if palm_up_axis is not None and palm_normal_axis is not None:
                        if float(np.dot(palm_normal_axis, palm_up_axis)) < 0.0:
                            palm_normal_axis = -palm_normal_axis

                        raw_signed_roll_deg = _signed_angle_deg(palm_up_axis, palm_normal_axis, forearm_axis)
                        if raw_signed_roll_deg is not None:
                            a_roll_raw = float(raw_signed_roll_deg)
                            a_roll_vector = palm_normal_axis

                            # 拇指旋转角（仅拇指可用时计算）
                            if thumb_in_palm is not None:
                                thumb_turn_deg = _signed_angle_deg(palm_axis, thumb_in_palm, palm_normal)
                                if thumb_turn_deg is not None:
                                    a_thumb_turn_deg = float(thumb_turn_deg)
                                    a_thumb_plane_deg = abs(float(thumb_turn_deg))
                                    thumb_right_component = float(np.dot(thumb_in_palm, frame.right))
                                    a_thumb_direction = Direction.RIGHT if thumb_right_component >= 0.0 else Direction.LEFT

                            # A 路置信度：基于 elbow、shoulder、wrist 可见性
                            pose_vis_count = sum(1 for v in (
                                right_elbow_visibility, right_shoulder_visibility, right_wrist_visibility
                            ) if v > _OCCLUSION_THRESHOLD)
                            a_confidence = {3: 0.9, 2: 0.5}.get(pose_vis_count, 0.0)

                            # Deadband + rotation_side
                            roll_deadband_deg = 1.0
                            roll_direction_deg = float(raw_signed_roll_deg)
                            if abs(roll_direction_deg) < roll_deadband_deg:
                                a_direction = Direction.UNKNOWN
                                a_rotation_side = "neutral"
                                a_roll_signed = 0.0
                            elif roll_direction_deg < 0.0:
                                a_direction = Direction.LEFT
                                a_rotation_side = "outward"
                                a_roll_signed = -abs(roll_direction_deg)
                            else:
                                a_direction = Direction.RIGHT
                                a_rotation_side = "inward"
                                a_roll_signed = abs(roll_direction_deg)

                            a_reason = "ok" if thumb_ok else "ok_thumb_occluded"

                            # ---- 手腕上下识别（A 路） ----
                            _pitch_pt = middle_tip
                            if _pitch_pt is None and middle_pip is not None:
                                _pitch_pt = middle_pip
                            if _pitch_pt is not None:
                                pitch_val, _ = _compute_wrist_pitch(
                                    forearm_axis, _pitch_pt, hand_wrist, frame.up,
                                )
                                a_wrist_pitch_deg = pitch_val

    # ====== B 路：纯 Hands 计算（总是尝试，不依赖 Pose） ======
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

        # B 路置信度：基于 MCP 五点可见数
        mcp_vis_count = sum(1 for v in (
            hand_wrist_visibility, index_mcp_visibility, middle_mcp_visibility,
            ring_mcp_visibility, pinky_mcp_visibility,
        ) if v > _OCCLUSION_THRESHOLD)
        b_confidence = {5: 0.85, 4: 0.6, 3: 0.3}.get(mcp_vis_count, 0.0)

        roll_deadband_deg = 1.0
        roll_direction_deg = float(b_roll_raw)
        if abs(roll_direction_deg) < roll_deadband_deg:
            b_direction = Direction.UNKNOWN
            b_rotation_side = "neutral"
            b_roll_signed = 0.0
        elif roll_direction_deg < 0.0:
            b_direction = Direction.LEFT
            b_rotation_side = "outward"
            b_roll_signed = -abs(roll_direction_deg)
        else:
            b_direction = Direction.RIGHT
            b_rotation_side = "inward"
            b_roll_signed = abs(roll_direction_deg)

        # ---- 手腕上下识别（B 路） ----
        _pitch_pt = middle_tip
        if _pitch_pt is None and middle_pip is not None:
            _pitch_pt = middle_pip
        if _pitch_pt is not None:
            pitch_val, _ = _compute_wrist_pitch(
                b_forearm_axis, _pitch_pt, hand_wrist, frame.up,
            )
            b_wrist_pitch_deg = pitch_val

    # ====== C 路：参考平面 ⟂ 旋转轴（纯 Hands，局域几何） ======
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

    # ================================================================
    # A/B/C 置信度加权融合
    # ================================================================
    a_weight = a_confidence if a_roll_raw is not None else 0.0
    b_weight = b_confidence if b_roll_raw is not None else 0.0
    c_weight = (c_result.get("confidence", 0.0)
                if (c_result is not None and c_result.get("reason") == "ok_c_path")
                else 0.0)

    # 构建参与融合的路列表（按权重降序），用于确定主导路
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
        # 加权融合滚转角
        fused_roll_raw = sum(w * r for _, w, r in path_entries) / total_weight

        # 主导路（最高置信度）
        path_entries.sort(key=lambda x: x[1], reverse=True)
        dominant_path = path_entries[0][0]

        # 方向与 rotation_side 基于融合后的 roll
        roll_deadband_deg = 1.0
        if abs(fused_roll_raw) < roll_deadband_deg:
            fused_direction = Direction.UNKNOWN
            fused_rotation_side = "neutral"
            fused_signed_roll = 0.0
        elif fused_roll_raw < 0.0:
            fused_direction = Direction.LEFT
            fused_rotation_side = "outward"
            fused_signed_roll = -abs(fused_roll_raw)
        else:
            fused_direction = Direction.RIGHT
            fused_rotation_side = "inward"
            fused_signed_roll = abs(fused_roll_raw)

        # 辅助数据：取主导路的值
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
            fused_reason_prefix = "fused"
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
            fused_reason_prefix = "fused"
        else:  # dominant_path == "c"
            fused_palm_normal = c_result.get("n_palm") if c_result else None
            fused_roll_vector = c_result.get("n_proj") if c_result else None
            fused_thumb_vector = None
            fused_thumb_turn_deg = None
            fused_thumb_plane_deg = None
            fused_thumb_direction = Direction.UNKNOWN
            fused_wrist_pitch = None  # C 路无 wrist pitch
            fused_forearm = None
            fused_confidence = c_result.get("confidence", 0.0) if c_result else 0.0
            fused_reason_prefix = "fused"

        # 构建 reason 字符串
        active_paths = "".join(p for p, _, _ in path_entries)
        fused_reason = f"fused_{active_paths}" if len(active_paths) > 1 else f"fused_{active_paths}"

        # 手腕上下识别：优先 A 路，否则 B 路（C 路无 pitch）
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
                x=float(fused_palm_normal[0]),
                y=float(fused_palm_normal[1]),
                z=float(fused_palm_normal[2]),
            ) if fused_palm_normal is not None else None,
            thumb_vector=Point3D(
                x=float(fused_thumb_vector[0]),
                y=float(fused_thumb_vector[1]),
                z=float(fused_thumb_vector[2]),
            ) if fused_thumb_vector is not None else None,
            roll_vector=Point3D(
                x=float(fused_roll_vector[0]),
                y=float(fused_roll_vector[1]),
                z=float(fused_roll_vector[2]),
            ) if fused_roll_vector is not None else None,
            wrist_pitch_deg=wrist_pitch_deg,
            confidence=fused_confidence,
            motion_allowed=True,
            reason=fused_reason,
        )

    # ---- 方案三：卡尔曼滤波短期遮挡缓冲 ----
    if result is not None and result.valid:
        _kalman_filter.update(float(result.signed_roll_deg))
        _kalman_occluded_frames = 0
        return result

    # A/B/C 路均失败 → 卡尔曼预测兜底
    _kalman_occluded_frames += 1
    if _kalman_occluded_frames <= _MAX_PREDICTION_FRAMES:
        pred_roll = _kalman_filter.predict()
        pred_direction = Direction.UNKNOWN
        pred_side = "neutral"
        if abs(pred_roll) >= 1.0:
            if pred_roll < 0.0:
                pred_direction = Direction.LEFT
                pred_side = "outward"
            else:
                pred_direction = Direction.RIGHT
                pred_side = "inward"

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
