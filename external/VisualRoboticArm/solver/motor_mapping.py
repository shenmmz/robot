"""角度名称 ↔ 电机编号映射表。

定义语义角度名称到电机编号的双向映射，设计为易于修改的配置结构。
其他模块通过 `get_motor_id()` / `get_angle_name()` 接口使用，避免硬编码。
"""

from __future__ import annotations

from typing import Dict, Optional, Tuple

# ---------------------------------------------------------------------------
# 映射配置（唯一修改点）
# ---------------------------------------------------------------------------
# 格式: (角度名称, 电机编号, 描述)
_ANGLE_MAPPINGS: Tuple[Tuple[str, int, str], ...] = (
    ("shoulder_flexion",    1, "肩部上下（前屈/后伸）"),
    ("shoulder_abduction",  2, "肩部左右（外展/内收）"),
    ("elbow_flexion",       3, "肘部上下（屈曲/伸展）"),
    ("elbow_pronation",     4, "肘部左右（前臂旋前/旋后，预留）"),
    ("wrist_pitch",         5, "腕部上下（屈曲/伸展）"),
    ("wrist_roll",          6, "腕部左右（桡偏/尺偏/旋转）"),
)

# 内部字典，由 _ANGLE_MAPPINGS 自动构建
_NAME_TO_ID: Dict[str, int] = {}
_ID_TO_NAME: Dict[int, str] = {}
_ID_TO_DESC: Dict[int, str] = {}

for _name, _mid, _desc in _ANGLE_MAPPINGS:
    _NAME_TO_ID[_name] = _mid
    _ID_TO_NAME[_mid] = _name
    _ID_TO_DESC[_mid] = _desc


# ---------------------------------------------------------------------------
# 公共接口
# ---------------------------------------------------------------------------

def get_motor_id(angle_name: str) -> Optional[int]:
    """根据角度名称获取对应电机编号。

    输入：
    - angle_name：角度名称，如 "shoulder_flexion"、"wrist_roll"

    输出：
    - 电机编号（1-6），不存在时返回 None
    """
    return _NAME_TO_ID.get(angle_name)


def get_angle_name(motor_id: int) -> Optional[str]:
    """根据电机编号获取对应角度名称。

    输入：
    - motor_id：电机编号（1-6）

    输出：
    - 角度名称，不存在时返回 None
    """
    return _ID_TO_NAME.get(motor_id)


def get_motor_description(motor_id: int) -> Optional[str]:
    """根据电机编号获取对应描述文字。

    输入：
    - motor_id：电机编号（1-6）

    输出：
    - 描述文字，不存在时返回 None
    """
    return _ID_TO_DESC.get(motor_id)


def list_mappings() -> Dict[str, int]:
    """返回所有角度名称 → 电机编号的映射副本。"""
    return dict(_NAME_TO_ID)


# ---------------------------------------------------------------------------
# 便捷常量（直接导入使用）
# ---------------------------------------------------------------------------

MOTOR_SHOULDER_FLEXION = 1
MOTOR_SHOULDER_ABDUCTION = 2
MOTOR_ELBOW_FLEXION = 3
MOTOR_ELBOW_PRONATION = 4
MOTOR_WRIST_PITCH = 5
MOTOR_WRIST_ROLL = 6
