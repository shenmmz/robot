"""多路角度融合策略模块。

提供轻量级加权融合函数，用于 A/B/C 三路手腕滚转角融合。
不依赖具体数据模型，仅做数值层面的融合 + 死区判定。
"""

from __future__ import annotations

from typing import List, Tuple


def weighted_fuse_roll(
    entries: List[Tuple[str, float, float]],
) -> Tuple[float, str, str] | None:
    """对多路滚转角做置信度加权融合。

    输入：
    - entries：`[(路径标识, 置信度权重, roll_deg), ...]`

    输出：
    - `(fused_roll_deg, dominant_path, active_paths_str)`
    - 无有效路径时返回 None
    """
    if not entries:
        return None

    total_weight = sum(w for _, w, _ in entries)
    if total_weight <= 0.0:
        return None

    fused_roll = sum(w * r for _, w, r in entries) / total_weight

    entries.sort(key=lambda x: x[1], reverse=True)
    dominant = entries[0][0]
    active = "".join(p for p, _, _ in entries)

    return fused_roll, dominant, active


def apply_deadband(
    roll_deg: float,
    threshold_deg: float = 1.0,
) -> Tuple[float, str, str]:
    """对滚转角施加死区判定，返回有符号角度、方向标签、旋转侧。

    输入：
    - roll_deg：原始滚转角（度）
    - threshold_deg：死区阈值（度）

    输出：
    - `(signed_roll_deg, direction_label, rotation_side)`
    - direction_label：`left` / `right` / `unknown`
    - rotation_side：`outward` / `inward` / `neutral`
    """
    if abs(roll_deg) < threshold_deg:
        return 0.0, "unknown", "neutral"

    if roll_deg < 0.0:
        return -abs(roll_deg), "left", "outward"
    else:
        return abs(roll_deg), "right", "inward"
