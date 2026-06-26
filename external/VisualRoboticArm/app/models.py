# -*- coding: utf-8 -*-
from __future__ import annotations

"""
数据模型模块。

本模块定义了推理与展示过程中用到的“稳定数据结构”，主要目的：
- 统一字段含义，避免在各模块之间传 dict 时出现字段名不一致；
- 便于序列化（dataclasses.asdict）后通过通信模块发送给外部系统；
- 对外表达清晰：side/direction/角度/关节旋转信息都在这里有明确含义。
"""

from dataclasses import dataclass
from enum import Enum
from typing import Optional


class Direction(str, Enum):
    """方向枚举。

    约定：
    - UP/DOWN 对应图像坐标的垂直方向（y 轴），需要注意图像坐标系通常 y 向下为正；
    - LEFT/RIGHT 对应水平 x 轴方向；
    - UNKNOWN 表示没有足够关键点或无法判断方向。
    """
    UP = "up"
    DOWN = "down"
    LEFT = "left"
    RIGHT = "right"
    UNKNOWN = "unknown"


class Side(str, Enum):
    """左右侧枚举（手臂侧别）。"""
    LEFT = "left"
    RIGHT = "right"


@dataclass(frozen=True)
class Point3D:
    """归一化三维坐标点。

    在 MediaPipe 中关键点坐标通常是相对值（0~1），这里用 float 统一存储。
    """
    x: float
    y: float
    z: float


@dataclass(frozen=True)
class JointRotation:
    """单个关节的“旋转趋势”与估计角度。

    该结构用于表达一个“方向 + 角度”的组合：
    - vertical / vertical_deg：垂直方向上的倾向与对应角度（度）
    - horizontal / horizontal_deg：水平方向上的倾向与对应角度（度）
    """
    vertical: Direction
    vertical_deg: Optional[float]
    horizontal: Direction
    horizontal_deg: Optional[float]


@dataclass(frozen=True)
class ArmAngles:
    """单侧手臂的关键角度与旋转信息集合。

    字段说明：
    - shoulder_deg / elbow_deg / wrist_deg：以三点夹角近似的关节角度（度）
    - vertical / horizontal：手臂整体方向（根据肩/肘/腕坐标粗略判断）
    - shoulder_rotation / arm_rotation / wrist_rotation：更偏“趋势/旋转”的方向+角度组合
    """
    side: Side
    shoulder_deg: Optional[float]
    elbow_deg: Optional[float]
    wrist_deg: Optional[float]
    vertical: Direction
    horizontal: Direction
    shoulder_rotation: JointRotation
    arm_rotation: JointRotation
    wrist_rotation: Optional[JointRotation]
    shoulder_flex_deg: Optional[float] = None
    shoulder_abduction_deg: Optional[float] = None
    shoulder_rotation_deg: Optional[float] = None
    elbow_flex_deg: Optional[float] = None
    wrist_flex_deg: Optional[float] = None


@dataclass(frozen=True)
class Motor6Command:
    """6号电机命令。

    当前验证阶段只输出右手腕部的左右旋转信息：
    - motor_id 固定为 6；
    - valid 为真时，Qt 端才允许把该命令转成实际电机动作；
    - roll_deg 表示以“平举掌面”为基准的旋转角度；
    - direction 只使用 LEFT / RIGHT / UNKNOWN 三种值。
    """
    motor_id: int = 6
    side: Side = Side.RIGHT
    joint: str = "wrist"
    axis: str = "roll"
    valid: bool = False
    mode: str = "palm_roll"
    reference: str = "palm_flat"
    roll_deg: Optional[float] = None
    signed_roll_deg: Optional[float] = None
    raw_signed_roll_deg: Optional[float] = None
    thumb_plane_deg: Optional[float] = None
    thumb_turn_deg: Optional[float] = None
    direction: Direction = Direction.UNKNOWN
    thumb_direction: Direction = Direction.UNKNOWN
    rotation_side: str = "unknown"
    palm_normal: Optional[Point3D] = None
    thumb_vector: Optional[Point3D] = None
    roll_vector: Optional[Point3D] = None
    confidence: Optional[float] = None
    motion_allowed: bool = False
    reason: str = "not_ready"
