from __future__ import annotations

# -*- coding: utf-8 -*-
"""姿态结果稳定过滤模块。

推理服务每帧都会输出角度和电机命令，但真实相机画面存在抖动、遮挡和短暂丢点。
本模块位于“推理服务 -> UI/通信”之间，负责：
1. 当前帧无检测结果时，尽量保持上一帧有效姿态；
2. 当前帧角度变化小于阈值时，保持上一帧，避免电机微抖；
3. 当前帧角度变化超过阈值时，允许动作输出；
4. 给 payload 补充 `motion_allowed`、`filter_state`、`filter_reason` 等控制字段。
"""

from copy import deepcopy
from dataclasses import dataclass
from math import isfinite
from typing import Any


@dataclass(frozen=True)
class PoseFilterConfig:
    """过滤配置。

    输入字段：
    - movement_threshold_deg：角度变化阈值，低于该值时认为是抖动而不是有效动作。
    """
    movement_threshold_deg: float = 5.0


class PoseMotionFilter:
    """稳定推理 payload 的过滤器。

    输入：
    - 推理服务输出的 payload 字典。

    输出：
    - 字段结构不变、但经过保持/过滤处理的新 payload 字典。
    """

    def __init__(self, movement_threshold_deg: float = 5.0):
        """初始化过滤器。

        输入：
        - movement_threshold_deg：动作变化阈值，单位为度。

        输出：无。
        功能：创建左右臂和 6 号电机的上一帧有效状态缓存。
        """
        self._config = PoseFilterConfig(movement_threshold_deg=float(movement_threshold_deg))
        self._last_valid_by_side: dict[str, dict[str, Any]] = {}
        self._last_valid_angles: list[dict[str, Any]] = []
        self._last_valid_motor_6: dict[str, Any] | None = None

    def reset(self) -> None:
        """清空历史缓存。

        输入：无。
        输出：无。
        功能：重新连接相机或重置状态时调用，避免沿用旧动作。
        """
        self._last_valid_by_side.clear()
        self._last_valid_angles.clear()
        self._last_valid_motor_6 = None

    def filter_payload(self, payload: dict[str, Any]) -> dict[str, Any]:
        """过滤一帧推理 payload。

        输入：
        - payload：`MediaPipePoseHandService.process_bgr()` 输出的字典。

        输出：
        - filtered：深拷贝后的新字典，内部角度和电机命令可能被保持或更新。

        功能实现：
        - 先稳定 6 号电机命令；
        - 再逐侧稳定左右臂角度；
        - 最后统一写回 motion/filter 字段。
        """
        if not isinstance(payload, dict):
            return payload

        filtered = deepcopy(payload)
        # 6 号电机单独处理，因为它直接决定是否发送实际电机动作。
        motor_6 = self._stabilize_motor_6(payload.get("motor_6"))
        _apply_motor_6(filtered, motor_6)

        raw_angles = payload.get("angles")
        current_angles = raw_angles if isinstance(raw_angles, list) else []

        if not current_angles:
            # 没有人体结果时不清空显示，而是保持上一帧有效角度。
            filtered["angles"] = deepcopy(self._last_valid_angles)
            filtered["pose_valid"] = False
            filtered["motion_allowed"] = False
            filtered["filter_state"] = "hold_last"
            filtered["filter_reason"] = "no_detection"
            _apply_motor_6(filtered, motor_6)
            return filtered

        next_angles: list[dict[str, Any]] = []
        pose_valid = False
        motion_allowed = False
        seen_sides: set[str] = set()

        for arm in current_angles:
            if not isinstance(arm, dict):
                continue

            side = _normalize_side(arm.get("side"))
            if side is None:
                continue
            seen_sides.add(side)

            previous = self._last_valid_by_side.get(side)
            # 单侧手臂独立判断，避免一侧丢点影响另一侧显示。
            stabilized, state = self._stabilize_arm(payload, side, arm, previous)

            if stabilized is None:
                continue

            next_angles.append(stabilized)
            if state == "move":
                motion_allowed = True
                pose_valid = True
                self._last_valid_by_side[side] = deepcopy(stabilized)
            elif state == "hold":
                pose_valid = True
            elif state == "invalid" and previous is not None:
                pose_valid = False

        for side, previous in self._last_valid_by_side.items():
            if side not in seen_sides and previous is not None:
                next_angles.append(deepcopy(previous))

        if next_angles:
            filtered["angles"] = next_angles
            self._last_valid_angles = deepcopy(next_angles)
        else:
            filtered["angles"] = deepcopy(self._last_valid_angles)

        filtered["pose_valid"] = pose_valid
        filtered["motion_allowed"] = motion_allowed
        filtered["filter_state"] = "move" if motion_allowed else ("track" if pose_valid else "hold_last")
        filtered["filter_reason"] = "stable"
        # 统一根据稳定后的 6 号电机刷新 motor_commands。
        _apply_motor_6(filtered, motor_6)
        return filtered

    def _stabilize_motor_6(self, current: Any) -> dict[str, Any] | None:
        """稳定 6 号电机命令。

        输入：
        - current：当前帧 `motor_6` 字段。

        输出：
        - 稳定后的 motor_6 字典；输入无效时返回上一帧保持命令。
        """
        if not isinstance(current, dict):
            return self._held_motor_6("motor_6_missing")

        if int(current.get("motor_id", 0) or 0) != 6:
            return self._held_motor_6("motor_6_id_invalid")

        if _normalize_side(current.get("side")) != "right":
            return self._held_motor_6("motor_6_side_not_right")

        if not bool(current.get("valid", False)):
            return self._held_motor_6(str(current.get("reason") or "motor_6_invalid"))

        current_angle = _motor_6_angle(current)
        if current_angle is None:
            return self._held_motor_6("motor_6_angle_invalid")

        stabilized = deepcopy(current)
        # 标准化必备字段，保证通信模块可以按统一结构读取。
        stabilized["motor_id"] = 6
        stabilized["side"] = "right"
        stabilized["joint"] = stabilized.get("joint") or "wrist"
        stabilized["axis"] = stabilized.get("axis") or "roll"
        stabilized["valid"] = True

        physical_angle = current_angle
        stabilized["raw_signed_roll_deg"] = current.get("raw_signed_roll_deg", physical_angle)
        stabilized["baseline_signed_roll_deg"] = 0.0
        stabilized["signed_roll_deg"] = physical_angle
        stabilized["roll_deg"] = abs(physical_angle)
        if abs(physical_angle) < 1e-6:
            stabilized["direction"] = "unknown"
            stabilized["rotation_side"] = "neutral"

        previous = self._last_valid_motor_6
        if previous is None:
            # 第一帧没有历史值，低于阈值则先保持，高于阈值才允许动作。
            if abs(physical_angle) < self._config.movement_threshold_deg:
                stabilized["motion_allowed"] = False
                stabilized["reason"] = "near_flat_hold"
                stabilized["filter_state"] = "hold"
            else:
                stabilized["motion_allowed"] = True
                stabilized["reason"] = "ok"
                stabilized["filter_state"] = "move"
            self._last_valid_motor_6 = deepcopy(stabilized)
            return stabilized

        previous_angle = _motor_6_angle(previous)
        if previous_angle is None:
            stabilized["motion_allowed"] = True
            stabilized["reason"] = "ok"
            stabilized["filter_state"] = "move"
            self._last_valid_motor_6 = deepcopy(stabilized)
            return stabilized

        delta = abs(physical_angle - previous_angle)
        if delta < self._config.movement_threshold_deg:
            # 变化量过小视为抖动，默认保持上一条有效命令。
            previous_direction = str(previous.get("direction") or "")
            current_direction = str(stabilized.get("direction") or "")
            if (
                previous_direction in {"left", "right", "Direction.LEFT", "Direction.RIGHT"}
                and current_direction in {"left", "right", "Direction.LEFT", "Direction.RIGHT"}
                and previous_direction != current_direction
            ):
                stabilized["motion_allowed"] = False
                stabilized["reason"] = "direction_changed_below_threshold"
                stabilized["filter_state"] = "hold"
                stabilized["latest_roll_deg"] = current_angle
                stabilized["delta_deg"] = delta
                self._last_valid_motor_6 = deepcopy(stabilized)
                return stabilized

            held = deepcopy(previous)
            held["valid"] = True
            held["motion_allowed"] = False
            held["reason"] = "below_threshold"
            held["filter_state"] = "hold"
            held["latest_roll_deg"] = current_angle
            held["delta_deg"] = delta
            return held

        stabilized["motion_allowed"] = True
        stabilized["reason"] = "ok"
        stabilized["filter_state"] = "move"
        stabilized["delta_deg"] = delta
        self._last_valid_motor_6 = deepcopy(stabilized)
        return stabilized

    def _held_motor_6(self, reason: str) -> dict[str, Any]:
        """构造保持状态的 6 号电机命令。

        输入：
        - reason：保持原因。

        输出：
        - motor_6 字典，`motion_allowed=False`。
        """
        if self._last_valid_motor_6 is not None:
            held = deepcopy(self._last_valid_motor_6)
        else:
            held = {
                "motor_id": 6,
                "side": "right",
                "joint": "wrist",
                "axis": "roll",
                "mode": "palm_roll",
                "reference": "palm_flat",
                "roll_deg": None,
                "signed_roll_deg": None,
                "direction": "unknown",
                "thumb_direction": "unknown",
            }
        held["valid"] = False
        held["motion_allowed"] = False
        held["reason"] = reason
        held["filter_state"] = "hold_last"
        return held

    def _stabilize_arm(
        self,
        payload: dict[str, Any],
        side: str,
        current: dict[str, Any],
        previous: dict[str, Any] | None,
    ) -> tuple[dict[str, Any] | None, str]:
        """稳定单侧手臂角度。

        输入：
        - payload：当前帧完整 payload；
        - side：`left` 或 `right`；
        - current：当前帧该侧手臂角度；
        - previous：上一帧该侧有效角度。

        输出：
        - `(stabilized, state)`：
          - `move` 表示角度变化超过阈值，可更新；
          - `hold` 表示小幅变化，保持上一帧；
          - `invalid` 表示当前帧缺失关键数据。
        """
        if not payload.get("has_pose", False):
            if previous is not None:
                return deepcopy(previous), "invalid"
            return None, "invalid"

        hand_key = "has_right_hand" if side == "right" else "has_left_hand"
        if not payload.get(hand_key, False):
            if previous is not None:
                return deepcopy(previous), "invalid"
            return None, "invalid"

        if not _has_required_wrist_data(current):
            if previous is not None:
                return deepcopy(previous), "invalid"
            return None, "invalid"

        current_numeric = _numeric_map(current)
        if previous is None:
            return deepcopy(current), "move"

        previous_numeric = _numeric_map(previous)
        if not previous_numeric:
            return deepcopy(current), "move"

        missing_keys = [key for key in previous_numeric.keys() if key not in current_numeric]
        if missing_keys:
            return deepcopy(previous), "invalid"

        max_delta = 0.0
        for key, previous_value in previous_numeric.items():
            current_value = current_numeric[key]
            if not isfinite(current_value):
                return deepcopy(previous), "invalid"
            max_delta = max(max_delta, abs(current_value - previous_value))

        if max_delta < self._config.movement_threshold_deg:
            return deepcopy(previous), "hold"

        return deepcopy(current), "move"


def _normalize_side(value: Any) -> str | None:
    """标准化左右侧字段。

    输入：可能是字符串或 Enum 的 side 值。
    输出：`left` / `right` / None。
    """
    side = getattr(value, "value", value)
    if side in {"left", "right"}:
        return str(side)
    return None


def _has_required_wrist_data(arm: dict[str, Any]) -> bool:
    """检查手臂角度中是否包含腕部旋转的必要数值。

    输入：单侧手臂角度字典。
    输出：True 表示 wrist_rotation.vertical_deg 与 horizontal_deg 都有效。
    """
    wrist = arm.get("wrist_rotation")
    if not isinstance(wrist, dict):
        return False
    return _is_number(wrist.get("vertical_deg")) and _is_number(wrist.get("horizontal_deg"))


def _numeric_map(value: Any, prefix: str = "") -> dict[str, float]:
    """递归提取嵌套结构中的数值字段。

    输入：
    - value：任意嵌套 dict/list/基础值；
    - prefix：递归路径前缀。

    输出：
    - `{字段路径: 数值}`，用于比较当前帧和上一帧最大角度变化。
    """
    numbers: dict[str, float] = {}
    if isinstance(value, dict):
        for key, item in value.items():
            child_prefix = f"{prefix}.{key}" if prefix else str(key)
            numbers.update(_numeric_map(item, child_prefix))
        return numbers

    if _is_number(value):
        numbers[prefix] = float(value)
    return numbers


def _apply_motor_6(payload: dict[str, Any], motor_6: dict[str, Any] | None) -> None:
    """把稳定后的 6 号电机命令写回 payload。

    输入：
    - payload：待修改的 payload 字典；
    - motor_6：稳定后的 6 号电机命令。

    输出：无，直接修改 payload。
    功能：根据 valid/motion_allowed 决定是否填充 motor_commands。
    """
    if motor_6 is None:
        payload["motor_commands"] = []
        return

    payload["motor_6"] = motor_6
    if bool(motor_6.get("valid")) and bool(motor_6.get("motion_allowed")):
        payload["motor_commands"] = [deepcopy(motor_6)]
        payload["motion_allowed"] = True
        payload["filter_state"] = motor_6.get("filter_state") or "move"
        payload["filter_reason"] = motor_6.get("reason") or "ok"
    else:
        payload["motor_commands"] = []
        payload["motion_allowed"] = False
        payload["filter_state"] = motor_6.get("filter_state") or "hold_last"
        payload["filter_reason"] = motor_6.get("reason") or "motor_6_hold"


def _motor_6_angle(command: dict[str, Any]) -> float | None:
    """提取 6 号电机用于过滤的角度。

    输入：motor_6 命令字典。
    输出：优先返回 signed_roll_deg，其次返回 roll_deg；无效时返回 None。
    """
    value = command.get("signed_roll_deg")
    if not _is_number(value):
        value = command.get("roll_deg")
    if not _is_number(value):
        return None
    return float(value)


def _is_number(value: Any) -> bool:
    """判断值是否为有限数值。

    输入：任意值。
    输出：True 表示 int/float 且不是 bool/NaN/Inf。
    """
    if isinstance(value, bool):
        return False
    if isinstance(value, (int, float)):
        return isfinite(float(value))
    return False
