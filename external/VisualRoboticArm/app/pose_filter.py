# -*- coding: utf-8 -*-
"""多帧确认姿态过滤器。

优化了原单帧 5° 阈值方案，引入以下机制：
1. 每个角度独立跟踪，±5° 以内抖动不更新（死区保持）；
2. 单帧大跳点（与上一原始帧差值 > 20°）视为离群，不更新；
3. 连续 3 帧同方向超过阈值才确认发送一次；
4. 仅标记实际变化的角度为 changed，通信层据此按需发送；
5. 检测点缺失（Pose/Hands/腕部数据无效）时保持不更新不发送；
6. reset() 在开始采集和重连相机时清空所有状态；
7. 输出 changed_motor_ids 供通信层筛选。

设计思路：
- PerAngleFilter 管理单个角度值的多帧状态；
- PoseMotionFilter 对 payload 中所有角度字段按侧×字段名创建独立过滤器；
- 输出 payload 中新增 changed_motor_ids 列表，通信层据此判断是否发送。
"""

from __future__ import annotations

import os
from copy import deepcopy
from math import isfinite
from typing import Any


def _env_int(name: str, default: int) -> int:
    try:
        return int(os.environ.get(name, str(default)))
    except ValueError:
        return default


# ---- 可配置常量 ----
# 死区阈值：角度变化绝对值低于此值时视为抖动，保持不更新。
DEADBAND_THRESHOLD_DEG: float = float(
    os.environ.get("VISUAL_ARM_DEADBAND_THRESHOLD_DEG", "2.0")
)
# 连续确认帧数：角度变化需连续多少帧同方向且超阈值才确认发送。
CONFIRM_FRAMES: int = _env_int("VISUAL_ARM_CONFIRM_FRAMES", 2)
# 离群跳点阈值：单帧角度与上一原始帧差值超过此值视为噪声跳点。
OUTLIER_JUMP_DEG: float = float(
    os.environ.get("VISUAL_ARM_OUTLIER_JUMP_DEG", "20.0")
)


class PerAngleFilter:
    """单个角度值的多帧确认滤波器。

    输入：
    - threshold_deg：死区阈值，低于此值保持不更新；
    - confirm_frames：连续超阈值且同方向的帧数阈值，达标后确认；
    - outlier_jump_deg：单帧跳变阈值，超过此值视为离群。

    输出：每次 update() 返回 (filtered_angle, changed)。
    """

    def __init__(
        self,
        threshold_deg: float = DEADBAND_THRESHOLD_DEG,
        confirm_frames: int = CONFIRM_FRAMES,
        outlier_jump_deg: float = OUTLIER_JUMP_DEG,
    ):
        self.threshold = float(threshold_deg)
        self.confirm_frames = max(1, int(confirm_frames))
        self.outlier_jump = float(outlier_jump_deg)

        self.confirmed_angle: float | None = None
        self.consecutive_count: int = 0
        self.last_direction: int = 0  # +1 / -1 / 0
        self.initialized: bool = False
        self.changed: bool = False
        self._prev_raw_angle: float | None = None

    def update(self, current_angle: float | None) -> tuple[float | None, bool]:
        """处理一帧角度。

        输入：
        - current_angle：当前帧原始角度，None 表示该字段缺失。

        输出：
        - (filtered_angle, changed)：
          filtered_angle 为经过滤波的输出值；
          changed 为 True 表示该角度本帧发生确认变化，应发送。
        """
        self.changed = False

        # ---- 缺失处理：保持上次确认值，不发送 ----
        if current_angle is None or not isfinite(current_angle):
            return self.confirmed_angle, False

        # ---- 首帧初始化：建立基线，不发送 ----
        if not self.initialized:
            self.confirmed_angle = current_angle
            self._prev_raw_angle = current_angle
            self.initialized = True
            return current_angle, False

        # ---- 离群跳点检测：单帧与上一原始帧差 > 阈值视为噪声 ----
        if self._prev_raw_angle is not None:
            raw_delta = abs(current_angle - self._prev_raw_angle)
            if raw_delta > self.outlier_jump:
                # 离群帧：丢弃原始值跟踪但保持确认值不变
                self._prev_raw_angle = current_angle
                self.consecutive_count = 0
                self.last_direction = 0
                return self.confirmed_angle, False

        self._prev_raw_angle = current_angle

        # ---- 死区判定：|delta| <= threshold，保持不更新 ----
        if self.confirmed_angle is None:
            self.confirmed_angle = current_angle
            return current_angle, False

        delta = current_angle - self.confirmed_angle

        if abs(delta) <= self.threshold:
            self.consecutive_count = 0
            self.last_direction = 0
            return self.confirmed_angle, False

        # ---- 超阈值：方向一致性检查 ----
        direction = 1 if delta > 0 else -1

        if direction != self.last_direction:
            # 方向反转：重置计数
            self.consecutive_count = 1
            self.last_direction = direction
        else:
            self.consecutive_count += 1

        if self.consecutive_count >= self.confirm_frames:
            # 连续确认达标：更新确认值，标记 changed，重置计数
            self.confirmed_angle = current_angle
            self.consecutive_count = 0
            self.changed = True
            return current_angle, True

        # 积累中：仍输出确认值
        return self.confirmed_angle, False

    def reset(self) -> None:
        """清空状态，用于相机重连或开始采集。"""
        self.confirmed_angle = None
        self.consecutive_count = 0
        self.last_direction = 0
        self.initialized = False
        self.changed = False
        self._prev_raw_angle = None


# ---- 需要按帧独立跟踪的角度字段 ----
# 格式：(side, field_path, motor_id)
# motor_id 为 None 表示该字段不直接对应电机（如辅助字段）
_ARM_ANGLE_FIELDS: list[tuple[str, str, int | None]] = [
    ("right", "shoulder_flex_deg", 1),
    ("right", "shoulder_abduction_deg", 2),
    ("right", "shoulder_rotation_deg", None),
    ("right", "elbow_flex_deg", 3),
    ("right", "elbow_abduction_deg", 4),
    ("right", "elbow_pronation_deg", None),
    ("right", "wrist_flex_deg", 5),
    ("left", "shoulder_flex_deg", None),
    ("left", "shoulder_abduction_deg", None),
    ("left", "shoulder_rotation_deg", None),
    ("left", "elbow_flex_deg", None),
    ("left", "elbow_abduction_deg", None),
    ("left", "elbow_pronation_deg", None),
    ("left", "wrist_flex_deg", None),
]
# 嵌套字段：wrist_rotation.vertical_deg / horizontal_deg
_ARM_NESTED_FIELDS: list[tuple[str, str, str, int | None]] = [
    ("right", "wrist_rotation", "vertical_deg", None),
    ("right", "wrist_rotation", "horizontal_deg", 6),
    ("left", "wrist_rotation", "vertical_deg", None),
    ("left", "wrist_rotation", "horizontal_deg", None),
]

# Motor 6 独立角度字段（从 motor_6 字典提取）
_MOTOR6_ANGLE_FIELD: str = "signed_roll_deg"


def _normalize_side(value: Any) -> str | None:
    """标准化左右侧字段。"""
    side = getattr(value, "value", value)
    if side in {"left", "right"}:
        return str(side)
    return None


def _is_number(value: Any) -> bool:
    """判断值是否为有限数值。"""
    if isinstance(value, bool):
        return False
    if isinstance(value, (int, float)):
        return isfinite(float(value))
    return False


def _nested_get(data: dict[str, Any], path: str) -> Any:
    """按 a.b 路径从嵌套 dict 中取值。"""
    parts = path.split(".")
    current = data
    for part in parts:
        if not isinstance(current, dict):
            return None
        current = current.get(part)
    return current


def _nested_set(data: dict[str, Any], path: str, value: Any) -> None:
    """按 a.b 路径写入嵌套 dict。"""
    parts = path.split(".")
    current = data
    for part in parts[:-1]:
        if part not in current:
            current[part] = {}
        current = current[part]
    current[parts[-1]] = value


def _extract_arm_fields(
    arm: dict[str, Any],
) -> dict[str, float | None]:
    """从单侧手臂字典中提取所有数值角度字段。

    输入：单侧手臂角度字典。
    输出：{field_path: value | None}。
    """
    result: dict[str, float | None] = {}
    for _, field_path, _ in _ARM_ANGLE_FIELDS:
        raw = arm.get(field_path)
        result[field_path] = float(raw) if _is_number(raw) else None
    for _, parent, field, _ in _ARM_NESTED_FIELDS:
        raw = _nested_get(arm, f"{parent}.{field}")
        result[f"{parent}.{field}"] = float(raw) if _is_number(raw) else None
    return result


def _has_required_arm_data(arm: dict[str, Any]) -> bool:
    """检查手臂字典中是否有腕部旋转关键数据。"""
    wrist = arm.get("wrist_rotation")
    if not isinstance(wrist, dict):
        return False
    return _is_number(wrist.get("vertical_deg")) and _is_number(wrist.get("horizontal_deg"))


class PoseMotionFilter:
    """多帧确认姿态过滤器。

    输入：
    - 推理 payload（含 angles 列表和 motor_6 字典）。

    输出：
    - filtered payload（角度值经滤波保持，附加 changed_motor_ids）。
    """

    def __init__(self):
        """初始化过滤器。

        输入：无（阈值从环境变量或默认值读取）。
        输出：无。
        """
        self._angle_filters: dict[str, PerAngleFilter] = {}
        self._motor6_filter: PerAngleFilter | None = None
        self._last_valid_angles: list[dict[str, Any]] = []
        self._last_valid_motor_6: dict[str, Any] | None = None
        self._init_filters()

    def _init_filters(self) -> None:
        """创建所有角度字段的 PerAngleFilter 实例。"""
        self._angle_filters.clear()
        # 手臂平铺字段
        for side, field_path, _ in _ARM_ANGLE_FIELDS:
            key = f"{side}.{field_path}"
            self._angle_filters[key] = PerAngleFilter()
        # 手臂嵌套字段
        for side, parent, field, _ in _ARM_NESTED_FIELDS:
            key = f"{side}.{parent}.{field}"
            self._angle_filters[key] = PerAngleFilter()
        # Motor 6
        self._motor6_filter = PerAngleFilter()

    def reset(self) -> None:
        """清空所有滤波器状态。

        调用时机：开始采集（on_start_clicked）或重连相机。

        输入：无。
        输出：无。
        """
        for filt in self._angle_filters.values():
            filt.reset()
        if self._motor6_filter is not None:
            self._motor6_filter.reset()
        self._last_valid_angles.clear()
        self._last_valid_motor_6 = None

    def filter_payload(self, payload: dict[str, Any]) -> dict[str, Any]:
        """过滤一帧推理 payload。

        输入：
        - payload：推理服务输出的字典。

        输出：
        - filtered：深拷贝后的新字典，角度值经滤波保持，
          附加 `changed_motor_ids` 和 `motion_allowed` 字段。
        """
        if not isinstance(payload, dict):
            return payload

        # 确保所有 filter 已初始化（reset 后需要重建）
        if not self._angle_filters or self._motor6_filter is None:
            self._init_filters()

        filtered = deepcopy(payload)
        has_pose = bool(payload.get("has_pose", False))
        has_right_hand = bool(payload.get("has_right_hand", False))
        has_left_hand = bool(payload.get("has_left_hand", False))

        raw_angles = payload.get("angles")
        current_angles = raw_angles if isinstance(raw_angles, list) else []

        # ---- 处理 Motor 6 ----
        motor_6 = payload.get("motor_6")
        motor6_changed = False
        if isinstance(motor_6, dict) and bool(motor_6.get("valid", False)):
            raw_angle = motor_6.get(_MOTOR6_ANGLE_FIELD)
            if _is_number(raw_angle):
                _, changed = self._motor6_filter.update(float(raw_angle))
                motor6_changed = changed

        # 稳定 motor_6 字典
        if isinstance(motor_6, dict):
            self._last_valid_motor_6 = deepcopy(motor_6)
        stabilized_motor6 = deepcopy(self._last_valid_motor_6) if self._last_valid_motor_6 else None

        # ---- 无检测处理 ----
        if not current_angles:
            filtered["angles"] = deepcopy(self._last_valid_angles)
            filtered["pose_valid"] = False
            filtered["motion_allowed"] = False
            filtered["filter_state"] = "hold_last"
            filtered["filter_reason"] = "no_detection"
            filtered["changed_motor_ids"] = []
            filtered["motor_6"] = stabilized_motor6
            return filtered

        # ---- 逐臂处理 ----
        changed_motor_ids: set[int] = set()
        next_angles: list[dict[str, Any]] = []
        pose_valid = False
        seen_sides: set[str] = set()

        for arm in current_angles:
            if not isinstance(arm, dict):
                continue

            side = _normalize_side(arm.get("side"))
            if side is None:
                continue
            seen_sides.add(side)

            hand_valid = has_right_hand if side == "right" else has_left_hand

            if not has_pose:
                # Pose 无效 → 保持上一帧值
                prev = _find_arm_by_side(self._last_valid_angles, side)
                if prev:
                    next_angles.append(deepcopy(prev))
                continue

            if not hand_valid:
                prev = _find_arm_by_side(self._last_valid_angles, side)
                if prev:
                    next_angles.append(deepcopy(prev))
                continue

            if not _has_required_arm_data(arm):
                prev = _find_arm_by_side(self._last_valid_angles, side)
                if prev:
                    next_angles.append(deepcopy(prev))
                continue

            # 有效检测：逐字段滤波
            stabilized_arm = deepcopy(arm)
            arm_changed = False

            # 平铺字段
            for _, field_path, motor_id in _ARM_ANGLE_FIELDS:
                if _normalize_side(arm.get("side")) != side:
                    continue
                raw = arm.get(field_path)
                raw_val = float(raw) if _is_number(raw) else None
                key = f"{side}.{field_path}"
                filt = self._angle_filters.get(key)
                if filt is None:
                    continue
                filtered_val, changed = filt.update(raw_val)
                if filtered_val is not None:
                    stabilized_arm[field_path] = filtered_val
                if changed and motor_id is not None:
                    changed_motor_ids.add(motor_id)
                    arm_changed = True

            # 嵌套字段
            for _, parent, field, motor_id in _ARM_NESTED_FIELDS:
                if _normalize_side(arm.get("side")) != side:
                    continue
                raw = _nested_get(arm, f"{parent}.{field}")
                raw_val = float(raw) if _is_number(raw) else None
                key = f"{side}.{parent}.{field}"
                filt = self._angle_filters.get(key)
                if filt is None:
                    continue
                filtered_val, changed = filt.update(raw_val)
                if filtered_val is not None:
                    _nested_set(stabilized_arm, f"{parent}.{field}", filtered_val)
                if changed and motor_id is not None:
                    changed_motor_ids.add(motor_id)
                    arm_changed = True

            if arm_changed:
                pose_valid = True

            next_angles.append(stabilized_arm)

        # Motor 6 changed 追加
        if motor6_changed:
            changed_motor_ids.add(6)

        # 补充未出现侧的上一帧数据
        for side, prev_arm in [
            ("right", _find_arm_by_side(self._last_valid_angles, "right")),
            ("left", _find_arm_by_side(self._last_valid_angles, "left")),
        ]:
            if side not in seen_sides and prev_arm is not None:
                next_angles.append(deepcopy(prev_arm))

        # 写入结果
        if next_angles:
            filtered["angles"] = next_angles
            self._last_valid_angles = deepcopy(next_angles)
        else:
            filtered["angles"] = deepcopy(self._last_valid_angles)

        motion_allowed = bool(changed_motor_ids)

        filtered["pose_valid"] = pose_valid or motion_allowed
        filtered["motion_allowed"] = motion_allowed
        filtered["filter_state"] = "move" if motion_allowed else ("track" if next_angles else "hold_last")
        filtered["filter_reason"] = "stable"
        filtered["changed_motor_ids"] = sorted(changed_motor_ids)

        # Motor 6 命令
        if stabilized_motor6 is not None:
            stabilized_motor6["motion_allowed"] = motor6_changed
            stabilized_motor6["reason"] = "ok" if motor6_changed else "hold"
        filtered["motor_6"] = stabilized_motor6
        filtered["motor_commands"] = []

        return filtered


def _find_arm_by_side(angles: list[dict[str, Any]], side: str) -> dict[str, Any] | None:
    """从角度列表中查找指定侧的手臂。"""
    for arm in angles:
        if _normalize_side(arm.get("side")) == side:
            return arm
    return None
