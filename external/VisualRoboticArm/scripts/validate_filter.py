# -*- coding: utf-8 -*-
"""滤波优化验证脚本。

使用 WIN_20260713_16_31_53_Pro.mp4 逐帧推理，
对比旧单帧阈值滤波器和新多帧确认滤波器的效果。

统计指标：
- 过滤后命令总数（电机命令发送次数）
- 静止区段发送次数（每段连续静止中意外发送的次数）
- 最大输出变化（单帧最大角度变化幅度）
"""

from __future__ import annotations

import argparse
import sys
import time
from collections import defaultdict
from copy import deepcopy
from dataclasses import dataclass, field
from math import isfinite
from pathlib import Path
from typing import Any

import cv2


PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from app.services import MediaPipePoseHandService
from app.pose_filter import PoseMotionFilter as NewPoseMotionFilter
from app.pose_comm import build_compact_pose_payload


# ============================================================
# 旧滤波器模拟（单帧 5° 阈值，原始逻辑）
# ============================================================

class OldPoseMotionFilter:
    """旧版单帧阈值滤波器（仅用于对比验证）。"""

    def __init__(self, movement_threshold_deg: float = 5.0):
        self.threshold = float(movement_threshold_deg)
        self._last_valid_angles: list[dict[str, Any]] = []
        self._last_valid_motor_6: dict[str, Any] | None = None
        self._initialized = False

    def reset(self):
        self._last_valid_angles.clear()
        self._last_valid_motor_6 = None
        self._initialized = False

    def filter_payload(self, payload: dict[str, Any]) -> dict[str, Any]:
        if not isinstance(payload, dict):
            return payload

        filtered = deepcopy(payload)
        has_pose = bool(payload.get("has_pose", False))

        raw_angles = payload.get("angles")
        current_angles = raw_angles if isinstance(raw_angles, list) else []

        old_motor6 = self._stabilize_motor_6_old(payload.get("motor_6"))
        filtered["motor_6"] = old_motor6

        if not current_angles:
            filtered["angles"] = deepcopy(self._last_valid_angles)
            filtered["pose_valid"] = False
            filtered["motion_allowed"] = False
            filtered["changed_motor_ids"] = [1, 2, 3, 4, 5, 6]  # 旧版无此字段
            return filtered

        next_angles: list[dict[str, Any]] = []
        motion_allowed = False

        for arm in current_angles:
            if not isinstance(arm, dict):
                continue
            side = _normalize_side(arm.get("side"))

            prev = _find_by_side(self._last_valid_angles, side)
            stabilized, state = self._stabilize_arm_old(arm, prev, has_pose)

            if stabilized is None:
                continue

            next_angles.append(stabilized)
            if state == "move":
                motion_allowed = True
                self._update_last(side, stabilized)

        if next_angles:
            self._last_valid_angles = deepcopy(next_angles)
            filtered["angles"] = next_angles
        else:
            filtered["angles"] = deepcopy(self._last_valid_angles)

        if old_motor6 and bool(old_motor6.get("motion_allowed")):
            motion_allowed = True

        filtered["motion_allowed"] = motion_allowed
        filtered["changed_motor_ids"] = [1, 2, 3, 4, 5, 6]
        return filtered

    def _stabilize_arm_old(self, arm, prev, has_pose):
        if not has_pose:
            return (deepcopy(prev), "invalid") if prev else (None, "invalid")
        if prev is None:
            return deepcopy(arm), "move"

        current_nums = _numeric_map(arm)
        prev_nums = _numeric_map(prev)
        if not prev_nums:
            return deepcopy(arm), "move"

        max_delta = 0.0
        for k, pv in prev_nums.items():
            cv_ = current_nums.get(k)
            if cv_ is None or not isfinite(cv_):
                return deepcopy(prev), "invalid"
            max_delta = max(max_delta, abs(cv_ - pv))

        if max_delta < self.threshold:
            return deepcopy(prev), "hold"
        return deepcopy(arm), "move"

    def _stabilize_motor_6_old(self, motor_6):
        if not isinstance(motor_6, dict) or not bool(motor_6.get("valid")):
            return self._last_valid_motor_6
        prev = self._last_valid_motor_6
        cur_angle = motor_6.get("signed_roll_deg", 0)
        if prev is None:
            self._last_valid_motor_6 = deepcopy(motor_6)
            self._last_valid_motor_6["motion_allowed"] = abs(cur_angle) >= self.threshold
            return self._last_valid_motor_6
        prev_angle = prev.get("signed_roll_deg", 0)
        delta = abs(cur_angle - prev_angle)
        if delta < self.threshold:
            held = deepcopy(prev)
            held["motion_allowed"] = False
            return held
        self._last_valid_motor_6 = deepcopy(motor_6)
        self._last_valid_motor_6["motion_allowed"] = True
        return self._last_valid_motor_6

    def _update_last(self, side, arm):
        for i, a in enumerate(self._last_valid_angles):
            if _normalize_side(a.get("side")) == side:
                self._last_valid_angles[i] = deepcopy(arm)
                return
        self._last_valid_angles.append(deepcopy(arm))


def _normalize_side(value):
    side = getattr(value, "value", value)
    if side in {"left", "right"}:
        return str(side)
    return None


def _find_by_side(angles, side):
    for a in (angles or []):
        if _normalize_side(a.get("side")) == side:
            return a
    return None


def _numeric_map(value, prefix=""):
    nums: dict[str, float] = {}
    if isinstance(value, dict):
        for k, v in value.items():
            child = f"{prefix}.{k}" if prefix else str(k)
            nums.update(_numeric_map(v, child))
        return nums
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        if isfinite(float(value)):
            nums[prefix] = float(value)
    return nums


# ============================================================
# 统计数据结构
# ============================================================

@dataclass
class FilterStats:
    """滤波器单次运行的统计。"""
    total_frames: int = 0
    pose_valid_frames: int = 0
    right_palm_frames: int = 0
    total_commands_sent: int = 0
    per_motor_commands: dict[int, int] = field(default_factory=lambda: defaultdict(int))
    # 静止区段：连续 >= 10 帧 motion_allowed=False 视为一个静止区段
    stationary_segments: int = 0
    stationary_spurious_sends: int = 0
    # 单帧最大角度变化
    max_angle_delta: float = 0.0
    max_delta_motor: int = 0
    max_delta_frame: int = 0
    # 每次发送的角度变化
    all_deltas: list[float] = field(default_factory=list)


def extract_compact_command_counts(compact: dict[str, Any]) -> dict[int, bool]:
    """从紧凑通信 payload 提取各电机是否在本帧发送。"""
    motor_commands = compact.get("motor_commands", [])
    result: dict[int, bool] = {}
    for cmd in motor_commands:
        mid = cmd.get("motor_id", 0)
        result[mid] = True
    return result


def compute_fair_delta(
    compact: dict[str, Any],
    motor_id: int,
    prev_angle: dict[int, float],
) -> float | None:
    """从 compact payload 提取电机角度并与上一发送帧比较。"""
    motors = compact.get("motors", {})
    mkey = f"motor_{motor_id}"
    cmd = motors.get(mkey, {})
    angle = cmd.get("angle_deg") if isinstance(cmd, dict) else None
    if angle is None:
        return None
    prev = prev_angle.get(motor_id)
    if prev is None:
        return 0.0
    return abs(angle - prev)


def run_filter_validation(
    video_path: Path,
    max_frames: int = 0,
) -> tuple[FilterStats, FilterStats]:
    """运行新旧滤波器对比验证。

    输入：
    - video_path：视频路径；
    - max_frames：最大处理帧数，0 表示完整视频。

    输出：
    - (old_stats, new_stats)：两套滤波器的统计结果。
    """
    print(f"加载视频: {video_path}")
    capture = cv2.VideoCapture(str(video_path))
    if not capture.isOpened():
        raise RuntimeError(f"无法打开视频: {video_path}")

    total_frames = int(capture.get(cv2.CAP_PROP_FRAME_COUNT))
    print(f"总帧数: {total_frames}")

    service = MediaPipePoseHandService()
    old_filter = OldPoseMotionFilter(movement_threshold_deg=5.0)
    new_filter = NewPoseMotionFilter()

    old_stats = FilterStats()
    new_stats = FilterStats()

    # 静止区段检测辅助
    old_stationary_count = 0
    new_stationary_count = 0
    old_in_stationary = False
    new_in_stationary = False

    # 角度变化跟踪（仅发送帧之间比较）
    old_sent_angles: dict[int, float] = {}
    new_sent_angles: dict[int, float] = {}

    started = time.perf_counter()
    frame_idx = 0

    try:
        while True:
            ok, frame = capture.read()
            if not ok:
                break

            annotated, payload = service.process_bgr(frame)
            frame_idx += 1

            # ---- 旧滤波器 ----
            old_payload = old_filter.filter_payload(payload)
            old_compact = build_compact_pose_payload(old_payload, "old-filter")
            old_cmds = extract_compact_command_counts(old_compact)
            old_cmd_count = sum(1 for v in old_cmds.values() if v)

            old_stats.total_commands_sent += old_cmd_count
            for mid in range(1, 7):
                if old_cmds.get(mid):
                    old_stats.per_motor_commands[mid] += 1

            # 静止区段检测
            if old_cmd_count == 0:
                if not old_in_stationary:
                    old_in_stationary = True
                    old_stationary_count = 0
                    old_stats.stationary_segments += 1
                old_stationary_count += 1
            else:
                if old_in_stationary:
                    old_in_stationary = False
                    if old_stationary_count >= 2:
                        pass  # 正常静止段
                # 检查是否为意外发送（前后帧都低活动）
                old_stationary_count = 0

            # 角度变化
            for mid in range(1, 7):
                delta = compute_fair_delta(old_compact, mid, old_sent_angles)
                if delta is not None:
                    old_stats.all_deltas.append(delta)
                    if delta > old_stats.max_angle_delta:
                        old_stats.max_angle_delta = delta
                        old_stats.max_delta_motor = mid
                        old_stats.max_delta_frame = frame_idx
                    if old_cmds.get(mid):
                        angle = old_compact.get("motors", {}).get(f"motor_{mid}", {}).get("angle_deg")
                        if angle is not None:
                            old_sent_angles[mid] = angle

            # ---- 新滤波器 ----
            new_payload = new_filter.filter_payload(payload)
            new_compact = build_compact_pose_payload(new_payload, "new-filter")
            new_cmds = extract_compact_command_counts(new_compact)
            new_cmd_count = sum(1 for v in new_cmds.values() if v)

            new_stats.total_commands_sent += new_cmd_count
            for mid in range(1, 7):
                if new_cmds.get(mid):
                    new_stats.per_motor_commands[mid] += 1

            # 静止区段检测
            if new_cmd_count == 0:
                if not new_in_stationary:
                    new_in_stationary = True
                    new_stationary_count = 0
                    new_stats.stationary_segments += 1
                new_stationary_count += 1
            else:
                if new_in_stationary:
                    new_in_stationary = False
                new_stationary_count = 0

            # 角度变化
            for mid in range(1, 7):
                delta = compute_fair_delta(new_compact, mid, new_sent_angles)
                if delta is not None:
                    new_stats.all_deltas.append(delta)
                    if delta > new_stats.max_angle_delta:
                        new_stats.max_angle_delta = delta
                        new_stats.max_delta_motor = mid
                        new_stats.max_delta_frame = frame_idx
                    if new_cmds.get(mid):
                        angle = new_compact.get("motors", {}).get(f"motor_{mid}", {}).get("angle_deg")
                        if angle is not None:
                            new_sent_angles[mid] = angle

            # 统计推理有效性
            old_stats.total_frames += 1
            new_stats.total_frames += 1
            old_stats.pose_valid_frames += int(bool(payload.get("has_pose")))
            new_stats.pose_valid_frames += int(bool(payload.get("has_pose")))
            old_stats.right_palm_frames += int(bool(payload.get("has_right_palm")))
            new_stats.right_palm_frames += int(bool(payload.get("has_right_palm")))

            # 静止区段中意外发送检测
            if old_in_stationary and old_cmd_count > 0:
                old_stats.stationary_spurious_sends += old_cmd_count
            if new_in_stationary and new_cmd_count > 0:
                new_stats.stationary_spurious_sends += new_cmd_count

            if frame_idx % 100 == 0 or frame_idx == total_frames:
                elapsed = time.perf_counter() - started
                speed = frame_idx / elapsed if elapsed > 0 else 0
                print(
                    f"[{frame_idx}/{total_frames}] 旧命令={old_stats.total_commands_sent} "
                    f"新命令={new_stats.total_commands_sent} 速度={speed:.1f}fps",
                    flush=True,
                )

            if max_frames > 0 and frame_idx >= max_frames:
                break

    finally:
        service.close()
        capture.release()

    elapsed = time.perf_counter() - started
    print(f"\n验证完成，耗时 {elapsed:.1f}s")
    return old_stats, new_stats


def print_report(old: FilterStats, new: FilterStats) -> None:
    """打印对比报告。"""
    reduction = old.total_commands_sent - new.total_commands_sent
    reduction_pct = (reduction / old.total_commands_sent * 100) if old.total_commands_sent > 0 else 0

    print("\n" + "=" * 72)
    print("                滤波优化验证报告")
    print("=" * 72)
    print()

    print(f"{'指标':<30} {'旧滤波器':>15} {'新滤波器':>15} {'变化':>10}")
    print("-" * 72)

    print(f"{'处理帧数':<30} {old.total_frames:>15} {new.total_frames:>15} {'':>10}")
    print(f"{'Pose 有效帧':<30} {old.pose_valid_frames:>15} {new.pose_valid_frames:>15} {'':>10}")
    print(f"{'右掌有效帧':<30} {old.right_palm_frames:>15} {new.right_palm_frames:>15} {'':>10}")
    print(f"{'命令总数':<30} {old.total_commands_sent:>15} {new.total_commands_sent:>15} {reduction:>+10}")
    print(f"{'命令削减率':<30} {'':>15} {'':>15} {f'{reduction_pct:.1f}%':>10}")

    print()
    print(f"{'各电机命令数':<30} {'旧滤波器':>15} {'新滤波器':>15} {'削减':>10}")
    print("-" * 72)
    for mid in range(1, 7):
        oc = old.per_motor_commands.get(mid, 0)
        nc = new.per_motor_commands.get(mid, 0)
        diff = oc - nc
        print(f"{'M' + str(mid):<30} {oc:>15} {nc:>15} {diff:>+10}")

    print()
    print(f"{'静止区段数':<30} {old.stationary_segments:>15} {new.stationary_segments:>15} {'':>10}")
    print(f"{'静止区段意外发送':<30} {old.stationary_spurious_sends:>15} {new.stationary_spurious_sends:>15} {'':>10}")

    print()
    print(f"{'最大单帧角度变化':<30} {'旧滤波器':>15} {'新滤波器':>15}")
    print("-" * 72)
    print(f"{'角度(°)':<30} {old.max_angle_delta:>15.1f} {new.max_angle_delta:>15.1f}")
    print(f"{'电机':<30} {'M' + str(old.max_delta_motor):>15} {'M' + str(new.max_delta_motor):>15}")
    print(f"{'帧号':<30} {old.max_delta_frame:>15} {new.max_delta_frame:>15}")

    if old.all_deltas and new.all_deltas:
        old_avg = sum(old.all_deltas) / len(old.all_deltas) if old.all_deltas else 0
        new_avg = sum(new.all_deltas) / len(new.all_deltas) if new.all_deltas else 0
        print(f"{'平均变化幅度(°)':<30} {old_avg:>15.1f} {new_avg:>15.1f}")

    # 有效命令密度（命令/有效帧）
    if old.total_frames > 0 and new.total_frames > 0:
        old_density = old.total_commands_sent / old.total_frames
        new_density = new.total_commands_sent / new.total_frames
        print(f"{'命令密度(次/帧)':<30} {old_density:>15.3f} {new_density:>15.3f}")

    print()
    print("=" * 72)

    # 总结
    print("\n总结:")
    checks = []
    if new.total_commands_sent < old.total_commands_sent:
        checks.append("✓ 命令总数减少")
    else:
        checks.append("✗ 命令总数未减少")

    if new.stationary_spurious_sends < old.stationary_spurious_sends:
        checks.append("✓ 静止区段意外发送减少")
    else:
        checks.append("✗ 静止区段意外发送未减少")

    if new.max_angle_delta <= old.max_angle_delta:
        checks.append("✓ 最大单帧跳变受控")
    else:
        checks.append("✗ 最大单帧跳变增大")

    for c in checks:
        print(f"  {c}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--video",
        type=Path,
        default=PROJECT_ROOT / "WIN_20260713_16_31_53_Pro.mp4",
        help="验证视频路径",
    )
    parser.add_argument("--max-frames", type=int, default=0, help="最大帧数，0=全部")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    video_path = args.video.resolve()

    if not video_path.exists():
        print(f"错误: 视频不存在 {video_path}")
        return 1

    old_stats, new_stats = run_filter_validation(video_path, max_frames=args.max_frames)
    print_report(old_stats, new_stats)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
