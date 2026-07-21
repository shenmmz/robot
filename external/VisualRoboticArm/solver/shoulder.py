"""肩部角度计算模块。

以肩关节点为原点，在躯干坐标系中计算上臂方向的两维角度：
- compute_shoulder_flexion   : 上下翻转角（上臂在矢状面内与正前方的有符号夹角），Motor 1
- compute_shoulder_abduction : 左右翻转角（上臂在水平面内与正前方的有符号夹角），Motor 2
- compute_shoulder_rotation  : 上臂绕自身长轴的旋转（内旋/外旋），保留供高级控制使用
"""

from __future__ import annotations

from typing import Optional

import numpy as np

from solver.utils import (
    BodyFrame,
    normalize,
    project_to_plane,
    signed_angle_deg,
)


def compute_shoulder_flexion(
    shoulder_unit: np.ndarray,
    frame: BodyFrame,
    side_sign: float,
) -> Optional[float]:
    """计算肩部上下翻转角（上臂俯仰角 / shoulder flexion-extension）。

    以肩关节为原点，将上臂方向投影到矢状面（frame.right 为法轴的 forward-up 平面），
    计算投影与正前方（frame.forward）的有符号夹角。
    - 以正前方水平伸出为 0°
    - 上抬为正（shoulder 指向 frame.up 方向），下垂为负
    - 乘以 side_sign（1.0 右侧，-1.0 左侧）修正左右侧别
    - 对应 Motor 1

    输入：
    - shoulder_unit：上臂方向单位向量（肩→肘）
    - frame：躯干坐标系
    - side_sign：1.0（右侧）或 -1.0（左侧）

    输出：
    - 上下翻转角度（度），范围 -90~+90；计算失败时返回 None
    """
    # 矢状面投影：去掉 frame.right（左右轴）分量
    sagittal_proj = project_to_plane(shoulder_unit, frame.right)
    if sagittal_proj is None:
        return None

    # 质量检查：投影模长过小表示上臂接近平行于 frame.right（手臂水平侧伸）
    sagittal_magnitude = float(np.abs(np.dot(shoulder_unit, sagittal_proj)))
    if sagittal_magnitude < 0.08:
        return None

    # 在矢状面内，以 frame.forward（前方）为 0°，frame.up（上方）为正方向
    # 旋转轴 frame.right：右手定则 → 拇指指向 right → 四指从 forward 旋向 up 为正
    flex_deg = signed_angle_deg(frame.forward, sagittal_proj, frame.right)
    if flex_deg is None:
        return None

    # 乘以 side_sign 修正左右侧别
    return float(np.clip(flex_deg * side_sign, -90.0, 90.0))


def compute_shoulder_abduction(
    shoulder_unit: np.ndarray,
    frame: BodyFrame,
    side_sign: float,
) -> Optional[float]:
    """计算肩部左右翻转角（上臂外展角 / shoulder abduction）。

    以肩关节为原点，将上臂方向投影到水平面（frame.up 为法轴的 forward-right 平面），
    计算投影与正前方（frame.forward）的有符号夹角。
    - 以正前方水平伸出为 0°
    - 向右偏为正（shoulder 指向 frame.right 方向），向左偏为负
    - 乘以 side_sign（1.0 右侧，-1.0 左侧）修正左右侧别
    - 对应 Motor 2

    输入：
    - shoulder_unit：上臂方向单位向量（肩→肘）
    - frame：躯干坐标系
    - side_sign：1.0（右侧）或 -1.0（左侧）

    输出：
    - 左右翻转角度（度），范围 -90~+90；计算失败时返回 None
    """
    # 水平面投影：去掉 frame.up（垂直轴）分量
    transverse_proj = project_to_plane(shoulder_unit, frame.up)
    if transverse_proj is None:
        return None

    # 质量检查：投影模长过小表示上臂接近平行于 frame.up（手臂自然下垂）
    transverse_magnitude = float(np.abs(np.dot(shoulder_unit, transverse_proj)))
    if transverse_magnitude < 0.08:
        return None

    # 在水平面内，以 frame.forward（前方）为 0°，frame.right（右方）为正方向
    # 旋转轴 frame.up：右手定则 → 拇指向上 → 四指从 forward 旋向 right 为正
    # 注意：frame.up 在图像坐标系（Y 向下）中为负 Y，导致 signed_angle_deg 符号反转，
    # 因此需要显式取反（等价于使用 -frame.up 作为旋转轴）
    abd_deg = signed_angle_deg(frame.forward, transverse_proj, frame.up)
    if abd_deg is None:
        return None

    # 修正符号：正值 = 向右偏；乘以 side_sign 修正左右侧别
    return float(np.clip(-abd_deg * side_sign, -90.0, 90.0))


def compute_shoulder_rotation(
    shoulder_unit: np.ndarray,
    forearm_unit: np.ndarray,
    frame: BodyFrame,
) -> Optional[float]:
    """计算肩部绕上臂长轴的旋转角度（内旋/外旋）。

    通过上臂-前臂平面法向量与躯干参考法向量的夹角计算。

    输入：
    - shoulder_unit：上臂方向单位向量（肩→肘）
    - forearm_unit：前臂方向单位向量（肘→腕）
    - frame：躯干坐标系

    输出：
    - 肩部旋转角度（度），计算失败时返回 None
    """
    arm_normal = normalize(np.cross(shoulder_unit, forearm_unit))
    if arm_normal is None:
        return None

    # 选择不平行于 shoulder_unit 的参考轴
    ref_axis = frame.forward
    if abs(float(np.dot(shoulder_unit, ref_axis))) > 0.85:
        ref_axis = frame.right

    ref_normal = normalize(np.cross(shoulder_unit, ref_axis))
    if ref_normal is None:
        ref_axis = frame.up
        ref_normal = normalize(np.cross(shoulder_unit, ref_axis))

    if ref_normal is None:
        return None

    # 使用 signed_angle_deg，返回有符号角度
    return signed_angle_deg(ref_normal, arm_normal, shoulder_unit)
