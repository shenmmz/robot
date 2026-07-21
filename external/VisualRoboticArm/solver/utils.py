"""几何工具函数与共享数据结构。

提供向量运算、平面拟合、卡尔曼滤波、躯干坐标系构建等底层工具，
供 shoulder / elbow / wrist 模块复用。
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import numpy as np

from app.models import Direction, JointRotation, Point3D

_EPS = 1e-8


# ---------------------------------------------------------------------------
# 数据结构
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class BodyFrame:
    """躯干坐标系：right / up / forward 三轴单位向量。"""
    right: np.ndarray
    up: np.ndarray
    forward: np.ndarray


# ---------------------------------------------------------------------------
# 向量基础运算
# ---------------------------------------------------------------------------

def to_vec(point: Point3D) -> np.ndarray:
    """Point3D → numpy 3D 向量。"""
    return np.array([float(point.x), float(point.y), float(point.z)], dtype=np.float64)


def vec_norm(vec: np.ndarray) -> float:
    """向量模长。"""
    return float(np.linalg.norm(vec))


def normalize(vec: np.ndarray, fallback: np.ndarray | None = None) -> np.ndarray | None:
    """归一化向量，长度接近 0 时返回 fallback（默认 None）."""
    length = vec_norm(vec)
    if length <= _EPS:
        if fallback is None:
            return None
        return fallback.astype(np.float64, copy=True)
    return vec / length


def project_to_plane(vec: np.ndarray, axis: np.ndarray) -> np.ndarray | None:
    """将 vec 投影到以 axis 为法向的平面上并归一化。"""
    axis_n = normalize(axis)
    if axis_n is None:
        return None
    projected = vec - float(np.dot(vec, axis_n)) * axis_n
    return normalize(projected)


# ---------------------------------------------------------------------------
# 角度计算
# ---------------------------------------------------------------------------

def angle_between_deg(a: np.ndarray, b: np.ndarray) -> Optional[float]:
    """无符号夹角（度），0~180。"""
    a_n = normalize(a)
    b_n = normalize(b)
    if a_n is None or b_n is None:
        return None
    cosine = float(np.clip(np.dot(a_n, b_n), -1.0, 1.0))
    return float(np.degrees(np.arccos(cosine)))


def signed_angle_deg(a: np.ndarray, b: np.ndarray, axis: np.ndarray) -> Optional[float]:
    """有符号夹角（度），符号由右手定则相对 axis 判定。"""
    a_n = normalize(a)
    b_n = normalize(b)
    axis_n = normalize(axis)
    if a_n is None or b_n is None or axis_n is None:
        return None
    sine = float(np.dot(axis_n, np.cross(a_n, b_n)))
    cosine = float(np.clip(np.dot(a_n, b_n), -1.0, 1.0))
    return float(np.degrees(np.arctan2(sine, cosine)))


def safe_zero_signed_angle(a: np.ndarray, b: np.ndarray, axis: np.ndarray) -> float:
    """有符号夹角的安全版本，失败时返回 0.0。"""
    value = signed_angle_deg(a, b, axis)
    return 0.0 if value is None else float(value)


# ---------------------------------------------------------------------------
# 平面拟合
# ---------------------------------------------------------------------------

def fit_plane_normal(points: list[Point3D]) -> np.ndarray | None:
    """用多个点 SVD 拟合平面法向量（无方向定向）。"""
    if len(points) < 3:
        return None

    matrix = np.array([to_vec(point) for point in points], dtype=np.float64)
    centered = matrix - matrix.mean(axis=0)
    try:
        _, _, vh = np.linalg.svd(centered, full_matrices=False)
    except np.linalg.LinAlgError:
        return None

    normal = normalize(vh[-1])
    return normal


def orient_palm_normal(
    normal: np.ndarray,
    hand_wrist: Point3D,
    index_mcp: Point3D,
    middle_mcp: Point3D,
    ring_mcp: Point3D,
    pinky_mcp: Point3D,
) -> np.ndarray | None:
    """利用手掌内部点序给掌面法向量定向，消除 SVD 正负二义性。"""
    wrist = to_vec(hand_wrist)
    hints: list[np.ndarray] = []
    for a, b in (
        (index_mcp, middle_mcp),
        (middle_mcp, ring_mcp),
        (ring_mcp, pinky_mcp),
        (index_mcp, pinky_mcp),
    ):
        hint = np.cross(to_vec(a) - wrist, to_vec(b) - wrist)
        if vec_norm(hint) > _EPS:
            hints.append(hint)

    if not hints:
        return normal

    hint_sum = np.sum(hints, axis=0)
    hint_unit = normalize(hint_sum)
    if hint_unit is None:
        return normal

    if float(np.dot(normal, hint_unit)) < 0.0:
        return -normal
    return normal


# ---------------------------------------------------------------------------
# 卡尔曼滤波器
# ---------------------------------------------------------------------------

class KalmanFilter1D:
    """简单一维卡尔曼滤波器，用于手腕滚转角短期遮挡缓冲。"""

    def __init__(self, process_noise: float = 0.1, measurement_noise: float = 0.5):
        self.x = 0.0
        self.p = 1.0
        self.q = process_noise
        self.r = measurement_noise
        self._initialized = False

    def update(self, measurement: float) -> float:
        if not self._initialized:
            self.x = measurement
            self.p = 1.0
            self._initialized = True
            return self.x
        self.p += self.q
        k = self.p / (self.p + self.r)
        self.x += k * (measurement - self.x)
        self.p *= (1.0 - k)
        return self.x

    def predict(self) -> float:
        if not self._initialized:
            return 0.0
        self.p += self.q
        return self.x


# ---------------------------------------------------------------------------
# 躯干坐标系
# ---------------------------------------------------------------------------

def build_body_frame(
    left_shoulder: Point3D,
    right_shoulder: Point3D,
    left_hip: Point3D,
    right_hip: Point3D,
) -> BodyFrame:
    """从肩髋四点构建躯干坐标系。"""
    shoulder_mid = (to_vec(left_shoulder) + to_vec(right_shoulder)) * 0.5
    hip_mid = (to_vec(left_hip) + to_vec(right_hip)) * 0.5

    up = normalize(shoulder_mid - hip_mid, np.array([0.0, -1.0, 0.0], dtype=np.float64))
    if up is None:
        up = np.array([0.0, -1.0, 0.0], dtype=np.float64)

    right = normalize(to_vec(right_shoulder) - to_vec(left_shoulder), np.array([1.0, 0.0, 0.0], dtype=np.float64))
    if right is None:
        right = np.array([1.0, 0.0, 0.0], dtype=np.float64)

    forward = normalize(np.cross(right, up), np.array([0.0, 0.0, 1.0], dtype=np.float64))
    if forward is None:
        forward = np.array([0.0, 0.0, 1.0], dtype=np.float64)

    right = normalize(np.cross(up, forward), right)
    if right is None:
        right = np.array([1.0, 0.0, 0.0], dtype=np.float64)
    up = normalize(np.cross(forward, right), up)
    if up is None:
        up = np.array([0.0, -1.0, 0.0], dtype=np.float64)

    return BodyFrame(right=right, up=up, forward=forward)


# ---------------------------------------------------------------------------
# 辅助：方向摘要
# ---------------------------------------------------------------------------

def summary_rotation(vector: np.ndarray | None, frame: BodyFrame, side_sign: float) -> JointRotation:
    """将单位向量投影到躯干坐标系，生成 JointRotation 摘要。"""
    if vector is None:
        return JointRotation(
            vertical=Direction.UNKNOWN,
            vertical_deg=None,
            horizontal=Direction.UNKNOWN,
            horizontal_deg=None,
        )

    vec = normalize(vector)
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
