"""肘部角度计算模块。

以肘关节点为原点，在躯干坐标系中计算前臂方向的两维角度：
- compute_elbow_flexion  : 上下翻转角（前臂在矢状面内与水平面的有符号夹角），Motor 3
- compute_elbow_abduction: 左右翻转角（前臂在水平面内与正前方的有符号夹角），Motor 4
- compute_elbow_pronation: 前臂旋前旋后角度（前臂绕自身轴旋转），保留供高级控制使用
"""

from __future__ import annotations

from typing import Optional

import numpy as np

from app.models import Point3D
from solver.utils import (
    BodyFrame,
    normalize,
    project_to_plane,
    signed_angle_deg,
    to_vec,
)


def compute_elbow_flexion(
    forearm_unit: np.ndarray,
    frame: BodyFrame,
) -> Optional[float]:
    """计算肘部上下翻转角（前臂俯仰角 / elbow flexion-extension）。

    以肘关节为原点，将前臂方向投影到矢状面（frame.right 为法轴的 forward-up 平面），
    计算投影与水平基准（frame.forward）的有符号夹角。
    - 以平行地面（前臂水平前伸）为 0°
    - 上抬为正（forearm 指向 frame.up 方向），下垂为负
    - 对应 Motor 3

    输入：
    - forearm_unit：前臂方向单位向量（肘→腕）
    - frame：躯干坐标系

    输出：
    - 上下翻转角度（度），范围 -90~+90；计算失败时返回 None
    """
    # 矢状面投影：去掉 frame.right（左右轴）分量
    sagittal_proj = project_to_plane(forearm_unit, frame.right)
    if sagittal_proj is None:
        return None

    # 质量检查：投影模长过小表示 forearm 接近平行于 frame.right（手臂水平侧伸）
    # 此时矢状面投影不可靠，应返回 None 而非噪声极端值
    sagittal_magnitude = float(np.abs(np.dot(forearm_unit, sagittal_proj)))
    if sagittal_magnitude < 0.08:
        return None

    # 在矢状面内，以 frame.forward（前方）为 0°，frame.up（上方）为正方向
    # 旋转轴 frame.right：右手定则 → 拇指指向 right → 四指从 forward 旋向 up 为正
    flex_deg = signed_angle_deg(frame.forward, sagittal_proj, frame.right)
    if flex_deg is None:
        return None

    return float(np.clip(flex_deg, -90.0, 90.0))


def compute_elbow_abduction(
    forearm_unit: np.ndarray,
    frame: BodyFrame,
) -> Optional[float]:
    """计算肘部左右翻转角（前臂外展角 / elbow abduction）。

    以肘关节为原点，将前臂方向投影到水平面（frame.up 为法轴的 forward-right 平面），
    计算投影与正前方（frame.forward）的有符号夹角。
    - 以正前方水平伸出为 0°
    - 向右偏为正（forearm 指向 frame.right 方向），向左偏为负
    - 对应 Motor 4

    输入：
    - forearm_unit：前臂方向单位向量（肘→腕）
    - frame：躯干坐标系

    输出：
    - 左右翻转角度（度），范围 -90~+90；计算失败时返回 None
    """
    # 水平面投影：去掉 frame.up（垂直轴）分量
    transverse_proj = project_to_plane(forearm_unit, frame.up)
    if transverse_proj is None:
        return None

    # 质量检查：投影模长过小表示 forearm 接近平行于 frame.up（手臂自然下垂）
    # 此时水平面投影不可靠，应返回 None 而非噪声极端值
    transverse_magnitude = float(np.abs(np.dot(forearm_unit, transverse_proj)))
    if transverse_magnitude < 0.08:
        return None

    # 在水平面内，以 frame.forward（前方）为 0°，frame.right（右方）为正方向
    # 旋转轴 frame.up：右手定则 → 拇指向上 → 四指从 forward 旋向 right 为正
    # 注意：frame.up 在图像坐标系（Y 向下）中为负 Y，导致 signed_angle_deg 符号反转，
    # 因此需要显式取反（等价于使用 -frame.up 作为旋转轴）
    abd_deg = signed_angle_deg(frame.forward, transverse_proj, frame.up)
    if abd_deg is None:
        return None

    # 修正符号：正值 = 向右偏
    return float(np.clip(-abd_deg, -90.0, 90.0))


def compute_elbow_pronation(
    *,
    elbow: Point3D,
    wrist: Point3D,
    forearm_unit: np.ndarray,
    index_mcp: Point3D | None = None,
    pinky_mcp: Point3D | None = None,
    frame: BodyFrame,
) -> Optional[float]:
    """计算前臂旋前旋后角度（pronation-supination / Motor 4）。

    几何原理：
    1. 在前臂轴（肘→腕）的垂直平面上建立参考坐标系
    2. 利用手部横轴（index_mcp ↔ pinky_mcp）在该平面上的投影，
       与躯干参考方向（frame.right 投影）计算有符号夹角
    3. 正值 → 旋后（掌面朝上/内侧），负值 → 旋前（掌面朝下/外侧）

    输入：
    - elbow：肘部 Pose 关键点
    - wrist：腕部 Pose 关键点
    - forearm_unit：前臂方向单位向量（肘→腕）
    - index_mcp：食指 MCP 关键点，可选
    - pinky_mcp：小指 MCP 关键点，可选
    - frame：躯干坐标系

    输出：
    - 前臂旋转角度（度），范围约 -90~+90。
    - 正 = 旋后（supination，掌面朝上）
    - 负 = 旋前（pronation，掌面朝下）
    - None = 数据不足无法计算
    """
    # ---- 构建手部横轴 ----
    hand_transverse: np.ndarray | None = None

    if index_mcp is not None and pinky_mcp is not None:
        # 最优：index_mcp → pinky_mcp 连线（手部横轴）
        hand_transverse = normalize(to_vec(pinky_mcp) - to_vec(index_mcp))
    elif index_mcp is not None:
        # 降级：手腕 → index_mcp 连线近似横轴方向
        wrist_vec = to_vec(wrist)
        hand_transverse = normalize(to_vec(index_mcp) - wrist_vec)

    if hand_transverse is None:
        return None

    # ---- 投影到前臂轴垂直平面 ----
    transverse_proj = project_to_plane(hand_transverse, forearm_unit)
    if transverse_proj is None:
        return None

    # ---- 参考方向：躯干 right 轴投影 ----
    ref_proj = project_to_plane(frame.right, forearm_unit)
    if ref_proj is None:
        return None

    # ---- 有符号角度：ref → transverse，绕 forearm 轴 ----
    # 使用 forearm_unit 的负方向（elbow→wrist）作为旋转轴参考
    # signed_angle 右手定则：当 transverse 在 ref 的逆时针方向时为正
    pronation_deg = signed_angle_deg(ref_proj, transverse_proj, forearm_unit)
    if pronation_deg is None:
        return None

    # 钳制到 [-90, 90]
    return float(np.clip(pronation_deg, -90.0, 90.0))
