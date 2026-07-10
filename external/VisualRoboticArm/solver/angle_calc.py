"""角度计算模块（重新导出层）。

本模块已拆分为以下子模块：
- utils.py     : 几何工具函数（向量运算、平面拟合、卡尔曼滤波、躯干坐标系）
- shoulder.py  : 肩部角度计算（flexion / abduction / rotation）
- elbow.py     : 肘部角度计算（flexion + pronation）
- wrist.py     : 腕部角度计算（pitch / roll，含 A/B/C 三路融合 + 卡尔曼兜底）
- fusion.py    : 多路融合策略（加权融合 + 死区判定）
- motor_mapping.py : 角度名称 ↔ 电机编号映射

本文件保持对外兼容：旧代码 `from solver.angle_calc import compute_arm_angles` 仍然有效。
"""

from __future__ import annotations

from typing import Optional

import numpy as np

from app.models import ArmAngles, Direction, Motor6Command, Point3D, Side
from solver.utils import (
    BodyFrame,
    angle_between_deg,
    build_body_frame,
    normalize,
    project_to_plane,
    safe_zero_signed_angle,
    summary_rotation,
    to_vec,
)
from solver.shoulder import (
    compute_shoulder_abduction,
    compute_shoulder_flexion,
    compute_shoulder_rotation,
)
from solver.elbow import compute_elbow_abduction, compute_elbow_flexion, compute_elbow_pronation
from solver.wrist import compute_right_wrist_motor_6


# ================================================================
# 内部：单臂求解（调用 shoulder / elbow 子模块）
# ================================================================

def _solve_arm(
    *,
    side: Side,
    shoulder: Point3D,
    elbow: Point3D,
    wrist: Point3D,
    hand_ref: Point3D | None,
    hand_index_mcp: Point3D | None = None,
    hand_pinky_mcp: Point3D | None = None,
    frame: BodyFrame,
) -> ArmAngles:
    side_sign = 1.0 if side == Side.RIGHT else -1.0

    shoulder_vec = to_vec(elbow) - to_vec(shoulder)
    forearm_vec = to_vec(wrist) - to_vec(elbow)
    hand_vec = to_vec(hand_ref) - to_vec(wrist) if hand_ref is not None else None

    shoulder_unit = normalize(shoulder_vec)
    forearm_unit = normalize(forearm_vec)
    hand_unit = normalize(hand_vec) if hand_vec is not None else None

    # ---- 方向判定 ----
    vertical = Direction.UNKNOWN
    horizontal = Direction.UNKNOWN
    if shoulder_unit is not None:
        vertical_component = float(np.dot(shoulder_unit, frame.up))
        horizontal_component = float(side_sign * np.dot(shoulder_unit, frame.right))
        vertical = Direction.UP if vertical_component >= 0 else Direction.DOWN
        horizontal = Direction.RIGHT if horizontal_component >= 0 else Direction.LEFT

    shoulder_rotation_summary = summary_rotation(shoulder_unit, frame, side_sign)
    arm_rotation_summary = summary_rotation(forearm_unit, frame, side_sign)
    wrist_rotation_summary = summary_rotation(hand_unit, frame, side_sign) if hand_unit is not None else None

    # ---- 肩部角度（调用 shoulder 子模块） ----
    shoulder_flex_deg: Optional[float] = None
    shoulder_abduction_deg: Optional[float] = None
    if shoulder_unit is not None:
        shoulder_flex_deg = compute_shoulder_flexion(shoulder_unit, frame, side_sign)
        shoulder_abduction_deg = compute_shoulder_abduction(shoulder_unit, frame, side_sign)

    shoulder_rotation_deg: Optional[float] = None
    if shoulder_unit is not None and forearm_unit is not None:
        shoulder_rotation_deg = compute_shoulder_rotation(shoulder_unit, forearm_unit, frame)

    # ---- 肘部角度（调用 elbow 子模块） ----
    elbow_flex_deg: Optional[float] = None
    elbow_abduction_deg: Optional[float] = None
    elbow_pronation_deg: Optional[float] = None
    if forearm_unit is not None:
        elbow_flex_deg = compute_elbow_flexion(forearm_unit, frame)
        elbow_abduction_deg = compute_elbow_abduction(forearm_unit, frame)
        elbow_pronation_deg = compute_elbow_pronation(
            elbow=elbow,
            wrist=wrist,
            forearm_unit=forearm_unit,
            index_mcp=hand_index_mcp,
            pinky_mcp=hand_pinky_mcp,
            frame=frame,
        )

    # ---- 腕部屈曲（本模块内联） ----
    wrist_flex_deg: Optional[float] = None
    if forearm_unit is not None and hand_unit is not None:
        wrist_angle = angle_between_deg(-forearm_unit, hand_unit)
        if wrist_angle is not None:
            wrist_flex_deg = float(np.clip(180.0 - wrist_angle, 0.0, 180.0))

    return ArmAngles(
        side=side,
        shoulder_deg=shoulder_flex_deg,
        elbow_deg=elbow_flex_deg,
        wrist_deg=wrist_flex_deg,
        vertical=vertical,
        horizontal=horizontal,
        shoulder_rotation=shoulder_rotation_summary,
        arm_rotation=arm_rotation_summary,
        wrist_rotation=wrist_rotation_summary,
        shoulder_flex_deg=shoulder_flex_deg,
        shoulder_abduction_deg=shoulder_abduction_deg,
        shoulder_rotation_deg=shoulder_rotation_deg,
        elbow_flex_deg=elbow_flex_deg,
        elbow_abduction_deg=elbow_abduction_deg,
        elbow_pronation_deg=elbow_pronation_deg,
        wrist_flex_deg=wrist_flex_deg,
    )


# ================================================================
# 公共接口
# ================================================================

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
    left_index_mcp: Point3D | None = None,
    left_pinky_mcp: Point3D | None = None,
    right_index_mcp: Point3D | None = None,
    right_pinky_mcp: Point3D | None = None,
) -> list[ArmAngles]:
    """计算左右臂的肩/肘/腕角度。

    构建躯干坐标系后，分别求解左右臂。
    """
    frame = build_body_frame(left_shoulder, right_shoulder, left_hip, right_hip)

    return [
        _solve_arm(
            side=Side.LEFT,
            shoulder=left_shoulder,
            elbow=left_elbow,
            wrist=left_wrist,
            hand_ref=left_hand_ref,
            hand_index_mcp=left_index_mcp,
            hand_pinky_mcp=left_pinky_mcp,
            frame=frame,
        ),
        _solve_arm(
            side=Side.RIGHT,
            shoulder=right_shoulder,
            elbow=right_elbow,
            wrist=right_wrist,
            hand_ref=right_hand_ref,
            hand_index_mcp=right_index_mcp,
            hand_pinky_mcp=right_pinky_mcp,
            frame=frame,
        ),
    ]


# ================================================================
# 重新导出（对外兼容）
# ================================================================

__all__ = [
    "compute_arm_angles",
    "compute_right_wrist_motor_6",
    # 子模块公开函数
    "compute_shoulder_flexion",
    "compute_shoulder_abduction",
    "compute_shoulder_rotation",
    "compute_elbow_flexion",
    "compute_elbow_abduction",
    "compute_elbow_pronation",
]
