"""关节角度 -> 电机输出的唯一映射表。

设计目标：
- 推理模块只输出标准关节角度字段；
- 通信模块只读取本文件，把角度字段翻译成电机发送语句；
- 调整电机编号、目标字段、方向文案时，只修改 `_MOTOR_MAPPINGS`。
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Dict, Optional, Tuple


@dataclass(frozen=True)
class MotorMapping:
    """单个电机的映射配置。

    输入字段：
    - angle_name：业务角度名称；
    - motor_id/pwm_index：外部控制端使用的电机编号和 PWM 序号；
    - source：角度来源，`angles` 表示左右臂角度列表，`motor_6` 表示兼容字段；
    - angle_field：从 source 中读取的角度字段，支持 `a.b.c` 嵌套路径；
    - side：从 `angles` 中提取哪一侧手臂；
    - target/joint/axis：发送给外部控制端的目标语义；
    - positive/negative/neutral：角度方向对应的控制语义和中文文案；
    - threshold_deg：小于该阈值时显示为 neutral；
    - angle_offset_deg：读取角度后叠加的偏移量，用于把推理角度转成电机控制角；
    - use_absolute_angle：True 时 `angle_deg` 输出绝对值，同时保留 `signed_angle_deg`；
    - send_enabled：True 时进入 `motor_commands` 发送列表。
    """

    angle_name: str
    motor_id: int
    description: str
    source: str
    angle_field: str
    side: str = "right"
    pwm_index: int | None = None
    target: str = ""
    joint: str = ""
    axis: str = ""
    positive_direction: str = "positive"
    negative_direction: str = "negative"
    neutral_direction: str = "hold"
    positive_text: str = "正向"
    negative_text: str = "反向"
    neutral_text: str = "保持"
    threshold_deg: float = 3.0
    angle_offset_deg: float = 0.0
    use_absolute_angle: bool = False
    send_enabled: bool = True

    @property
    def resolved_pwm_index(self) -> int:
        """返回实际 PWM 序号；未单独配置时与电机编号一致。"""
        return int(self.pwm_index if self.pwm_index is not None else self.motor_id)


# ---------------------------------------------------------------------------
# 映射配置（唯一修改点）
# ---------------------------------------------------------------------------
_MOTOR_MAPPINGS: Tuple[MotorMapping, ...] = (
    MotorMapping(
        angle_name="shoulder_flexion",
        motor_id=1,
        description="肩部上下（前屈/后伸）",
        source="angles",
        angle_field="shoulder_flex_deg",
        target="right_shoulder_flexion",
        joint="shoulder",
        axis="flexion",
        positive_direction="up",
        negative_direction="down",
        neutral_direction="hold",
        positive_text="上抬",
        negative_text="下垂",
        neutral_text="水平",
    ),
    MotorMapping(
        angle_name="shoulder_abduction",
        motor_id=2,
        description="肩部左右（外展/内收）",
        source="angles",
        angle_field="shoulder_abduction_deg",
        target="right_shoulder_abduction",
        joint="shoulder",
        axis="abduction",
        positive_direction="right",
        negative_direction="left",
        neutral_direction="hold",
        positive_text="右偏",
        negative_text="左偏",
        neutral_text="居中",
    ),
    MotorMapping(
        angle_name="elbow_flexion",
        motor_id=3,
        description="肘部上下（屈曲/伸展）",
        source="angles",
        angle_field="elbow_flex_deg",
        target="right_elbow_flexion",
        joint="elbow",
        axis="flexion",
        positive_direction="up",
        negative_direction="down",
        neutral_direction="hold",
        positive_text="上抬",
        negative_text="下垂",
        neutral_text="水平",
    ),
    MotorMapping(
        angle_name="elbow_abduction",
        motor_id=4,
        description="肘部左右（外展/内收）",
        source="angles",
        angle_field="elbow_abduction_deg",
        target="right_elbow_abduction",
        joint="elbow",
        axis="abduction",
        positive_direction="right",
        negative_direction="left",
        neutral_direction="hold",
        positive_text="右偏",
        negative_text="左偏",
        neutral_text="中位",
    ),
    MotorMapping(
        angle_name="wrist_pitch",
        motor_id=5,
        description="腕部上下（屈曲/伸展）",
        source="angles",
        angle_field="wrist_flex_deg",
        target="right_wrist_pitch",
        joint="wrist",
        axis="pitch",
        positive_direction="up",
        negative_direction="down",
        neutral_direction="hold",
        positive_text="上翻",
        negative_text="下翻",
        neutral_text="保持",
    ),
    MotorMapping(
        angle_name="wrist_roll",
        motor_id=6,
        description="腕部左右旋转",
        source="motor_6",
        angle_field="signed_roll_deg",
        target="right_wrist_roll",
        joint="wrist",
        axis="roll",
        positive_direction="right",
        negative_direction="left",
        neutral_direction="hold",
        positive_text="右旋转",
        negative_text="左旋转",
        neutral_text="保持",
    ),
)


# 内部索引，由 _MOTOR_MAPPINGS 自动构建。
_NAME_TO_MAPPING: Dict[str, MotorMapping] = {}
_ID_TO_MAPPING: Dict[int, MotorMapping] = {}

for _mapping in _MOTOR_MAPPINGS:
    _NAME_TO_MAPPING[_mapping.angle_name] = _mapping
    _ID_TO_MAPPING[_mapping.motor_id] = _mapping

# 兼容旧命名：外部如果仍查询 elbow_pronation，仍然返回 4 号电机。
_NAME_ALIASES: Dict[str, str] = {
    "elbow_pronation": "elbow_abduction",
}


def iter_motor_mappings() -> Tuple[MotorMapping, ...]:
    """返回所有电机映射，通信模块按该顺序生成发送 payload。"""
    return _MOTOR_MAPPINGS


def get_motor_mapping(motor_id: int) -> Optional[MotorMapping]:
    """根据电机编号获取完整映射配置。"""
    return _ID_TO_MAPPING.get(int(motor_id))


def get_motor_id(angle_name: str) -> Optional[int]:
    """根据角度名称获取对应电机编号。"""
    canonical = _NAME_ALIASES.get(angle_name, angle_name)
    mapping = _NAME_TO_MAPPING.get(canonical)
    return mapping.motor_id if mapping is not None else None


def get_angle_name(motor_id: int) -> Optional[str]:
    """根据电机编号获取对应角度名称。"""
    mapping = get_motor_mapping(motor_id)
    return mapping.angle_name if mapping is not None else None


def get_motor_description(motor_id: int) -> Optional[str]:
    """根据电机编号获取对应描述文字。"""
    mapping = get_motor_mapping(motor_id)
    return mapping.description if mapping is not None else None


def list_mappings() -> Dict[str, int]:
    """返回所有角度名称 -> 电机编号的映射副本。"""
    return {mapping.angle_name: mapping.motor_id for mapping in _MOTOR_MAPPINGS}


# ---------------------------------------------------------------------------
# 便捷常量（直接导入使用）
# ---------------------------------------------------------------------------

MOTOR_SHOULDER_FLEXION = 1
MOTOR_SHOULDER_ABDUCTION = 2
MOTOR_ELBOW_FLEXION = 3
MOTOR_ELBOW_ABDUCTION = 4
MOTOR_ELBOW_PRONATION = 4
MOTOR_WRIST_PITCH = 5
MOTOR_WRIST_ROLL = 6
