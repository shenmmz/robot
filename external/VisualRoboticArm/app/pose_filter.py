from __future__ import annotations

from copy import deepcopy
from dataclasses import dataclass
from math import isfinite
from typing import Any


@dataclass(frozen=True)
class PoseFilterConfig:
    movement_threshold_deg: float = 5.0


class PoseMotionFilter:
    """Filter unstable pose outputs and hold the last valid pose."""

    def __init__(self, movement_threshold_deg: float = 5.0):
        self._config = PoseFilterConfig(movement_threshold_deg=float(movement_threshold_deg))
        self._last_valid_by_side: dict[str, dict[str, Any]] = {}
        self._last_valid_angles: list[dict[str, Any]] = []
        self._last_valid_motor_6: dict[str, Any] | None = None

    def reset(self) -> None:
        self._last_valid_by_side.clear()
        self._last_valid_angles.clear()
        self._last_valid_motor_6 = None

    def filter_payload(self, payload: dict[str, Any]) -> dict[str, Any]:
        if not isinstance(payload, dict):
            return payload

        filtered = deepcopy(payload)
        motor_6 = self._stabilize_motor_6(payload.get("motor_6"))
        _apply_motor_6(filtered, motor_6)

        raw_angles = payload.get("angles")
        current_angles = raw_angles if isinstance(raw_angles, list) else []

        if not current_angles:
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
        _apply_motor_6(filtered, motor_6)
        return filtered

    def _stabilize_motor_6(self, current: Any) -> dict[str, Any] | None:
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
    side = getattr(value, "value", value)
    if side in {"left", "right"}:
        return str(side)
    return None


def _has_required_wrist_data(arm: dict[str, Any]) -> bool:
    wrist = arm.get("wrist_rotation")
    if not isinstance(wrist, dict):
        return False
    return _is_number(wrist.get("vertical_deg")) and _is_number(wrist.get("horizontal_deg"))


def _numeric_map(value: Any, prefix: str = "") -> dict[str, float]:
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
    value = command.get("signed_roll_deg")
    if not _is_number(value):
        value = command.get("roll_deg")
    if not _is_number(value):
        return None
    return float(value)


def _is_number(value: Any) -> bool:
    if isinstance(value, bool):
        return False
    if isinstance(value, (int, float)):
        return isfinite(float(value))
    return False
