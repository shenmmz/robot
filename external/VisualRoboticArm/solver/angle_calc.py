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
) -> Motor6Command:
    """根据右手掌面与拇指方向生成 6 号电机左右旋转指令。

    当前验证阶段只允许右手驱动 6 号电机。算法使用人体躯干坐标系作为参考：
    - 手掌法向量用于估计腕部相对“平举掌面”的滚转角；
    - 拇指方向用于给出左右旋转方向，便于现场快速校验方向是否需要反向。
    """
    frame = _build_body_frame(left_shoulder, right_shoulder, left_hip, right_hip)

    # 右手腕滚转要围绕前臂方向判断。这里使用 pose 的肘-腕向量作为旋转轴，
    # 手部 landmarks 只负责提供掌面和拇指方向，避免掌面计算受整条手臂骨架漂移影响。
    forearm_axis = _normalize(_to_vec(right_wrist) - _to_vec(right_elbow))
    if forearm_axis is None:
        return Motor6Command(reason="right_forearm_axis_invalid")

    palm_index = _to_vec(index_mcp) - _to_vec(hand_wrist)
    palm_pinky = _to_vec(pinky_mcp) - _to_vec(hand_wrist)
    palm_normal = _fit_plane_normal(
        [hand_wrist, index_mcp, middle_mcp, ring_mcp, pinky_mcp],
    )
    if palm_normal is None:
        return Motor6Command(reason="right_palm_plane_invalid")

    palm_normal = _orient_palm_normal(
        palm_normal,
        hand_wrist=hand_wrist,
        index_mcp=index_mcp,
        middle_mcp=middle_mcp,
        ring_mcp=ring_mcp,
        pinky_mcp=pinky_mcp,
    )

    thumb_vector = _normalize(_to_vec(thumb_tip) - _to_vec(hand_wrist))
    if thumb_vector is None:
        return Motor6Command(reason="right_thumb_vector_invalid")

    # 用户要求“以大拇指与手掌形成平面做旋转角度判断”。所以不能直接看拇指全局左右，
    # 必须先把拇指投影到掌面内，去掉拇指翘起/朝上的分量，再围绕前臂轴计算滚转。
    thumb_in_palm = _normalize(thumb_vector - float(np.dot(thumb_vector, palm_normal)) * palm_normal)
    if thumb_in_palm is None:
        return Motor6Command(reason="right_thumb_plane_projection_invalid")

    palm_axis = _project_to_plane(_to_vec(index_mcp) - _to_vec(pinky_mcp), palm_normal)
    if palm_axis is None:
        palm_axis = _project_to_plane(_to_vec(middle_mcp) - _to_vec(hand_wrist), palm_normal)
    if palm_axis is None:
        return Motor6Command(reason="right_wrist_roll_invalid")

    # 用右手自身结构给掌面法向做辅助定向，避免 SVD 拟合法向正负随机跳变。
    # thumb_in_palm 是拇指在掌面内的方向，palm_axis 是掌宽方向。
    palm_face_hint = _normalize(np.cross(thumb_in_palm, palm_axis))
    if palm_face_hint is not None and float(np.dot(palm_normal, palm_face_hint)) < 0.0:
        palm_normal = -palm_normal

    palm_up_axis = _project_to_plane(frame.up, forearm_axis)
    palm_normal_axis = _project_to_plane(palm_normal, forearm_axis)
    if palm_up_axis is None or palm_normal_axis is None:
        return Motor6Command(reason="right_wrist_roll_invalid")

    # 掌面平行地面只关心平面，不关心掌心朝上还是朝下。
    # 使用离水平基准更近的那一侧法向，避免把同一平面反面算成 100+ 度。
    if float(np.dot(palm_normal_axis, palm_up_axis)) < 0.0:
        palm_normal_axis = -palm_normal_axis

    # 物理基准：掌面平行地面时，掌面法向与人体上方一致，角度为 0。
    # 这里量的是“掌面离开平举基准的倾角”，而不是拇指翘起多少。
    raw_signed_roll_deg = _signed_angle_deg(palm_up_axis, palm_normal_axis, forearm_axis)
    if raw_signed_roll_deg is None:
        return Motor6Command(reason="right_wrist_roll_invalid")

    thumb_turn_deg = _signed_angle_deg(palm_axis, thumb_in_palm, palm_normal)
    if thumb_turn_deg is None:
        return Motor6Command(reason="right_thumb_plane_turn_invalid")

    # 新定义：
    # - 以平行于地面的掌面为基准；
    # - 以手肘为基准线（前臂轴）；
    # - 掌面向身体内侧翻转 -> left / inward；
    # - 掌面向身体外侧翻转 -> right / outward；
    # - raw_signed_roll_deg 只表示掌面与基准面的有符号夹角。
    roll_deadband_deg = 1.0
    roll_direction_deg = float(raw_signed_roll_deg)
    if abs(roll_direction_deg) < roll_deadband_deg:
        direction = Direction.UNKNOWN
        rotation_side = "neutral"
        signed_roll_deg = 0.0
    elif roll_direction_deg < 0.0:
        direction = Direction.LEFT
        rotation_side = "inward"
        signed_roll_deg = -abs(roll_direction_deg)
    else:
        direction = Direction.RIGHT
        rotation_side = "outward"
        signed_roll_deg = abs(roll_direction_deg)

    thumb_right_component = float(np.dot(thumb_in_palm, frame.right))
    thumb_direction = Direction.RIGHT if thumb_right_component >= 0.0 else Direction.LEFT
    thumb_plane_deg = abs(float(thumb_turn_deg))

    # 6号电机调试向量：表示掌面法向投影到前臂滚转平面后的方向。
    # 显式保存为局部变量，避免历史代码或后续维护时引用 roll_vector 触发 NameError。
    roll_vector = palm_normal_axis

    confidence = float(
        np.clip(
            min(abs(raw_signed_roll_deg) / 90.0, 1.0)
            * min(_norm(palm_index) * 10.0, 1.0)
            * min(_norm(palm_pinky) * 10.0, 1.0),
            0.0,
            1.0,
        )
    )

    return Motor6Command(
        valid=True,
        roll_deg=abs(float(raw_signed_roll_deg)),
        signed_roll_deg=float(signed_roll_deg),
        raw_signed_roll_deg=float(raw_signed_roll_deg),
        thumb_plane_deg=float(thumb_plane_deg),
        thumb_turn_deg=float(thumb_turn_deg),
        direction=direction,
        thumb_direction=thumb_direction,
        rotation_side=rotation_side,
        palm_normal=Point3D(
            x=float(palm_normal[0]),
            y=float(palm_normal[1]),
            z=float(palm_normal[2]),
        ),
        thumb_vector=Point3D(
            x=float(thumb_vector[0]),
            y=float(thumb_vector[1]),
            z=float(thumb_vector[2]),
        ),
        roll_vector=Point3D(
            x=float(roll_vector[0]),
            y=float(roll_vector[1]),
            z=float(roll_vector[2]),
        ),
        confidence=confidence,
        motion_allowed=True,
        reason="ok",
    )
