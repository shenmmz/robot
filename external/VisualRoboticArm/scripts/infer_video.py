"""使用项目正式 Pose + Hands 服务处理视频并保存逐帧绘制结果。"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

import cv2


PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from app.services import MediaPipePoseHandService


def parse_args() -> argparse.Namespace:
    """解析离线视频推理命令行参数。

    输入：由 argparse 从命令行读取输入视频、输出视频和可选最大帧数。
    输出：`argparse.Namespace`，其中 input/output 为 `Path`，max_frames 为整数。

    `--max-frames=0` 表示处理完整视频；正整数主要用于快速冒烟测试和抽帧验证。
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="源视频路径")
    parser.add_argument("output", type=Path, help="带骨架和角度标注的 MP4 输出路径")
    parser.add_argument("--max-frames", type=int, default=0, help="最多处理帧数；0 表示处理完整视频")
    return parser.parse_args()


def main() -> int:
    """执行完整的离线视频推理和绘制流程。

    输入：无直接参数，内部调用 `parse_args()` 读取命令行。

    输出：
    - 成功时返回进程退出码 0；
    - 输出文件使用源视频分辨率和帧率，编码格式为 MP4V；
    - 控制台打印处理帧数、Pose 有效帧、右掌法向量有效帧、速度和耗时。

    处理步骤：
    1. 打开输入视频并读取帧率、分辨率和总帧数；
    2. 创建 `MediaPipePoseHandService` 和相同尺寸的视频写入器；
    3. 逐帧调用 `process_bgr()`，写入已绘制的 BGR 图像并累计有效率；
    4. 无论正常结束还是发生异常，都在 finally 中释放模型、输入流和输出流。
    """
    args = parse_args()
    input_path = args.input.resolve()
    output_path = args.output.resolve()

    capture = cv2.VideoCapture(str(input_path))
    if not capture.isOpened():
        raise RuntimeError(f"无法打开输入视频：{input_path}")

    fps = float(capture.get(cv2.CAP_PROP_FPS))
    if fps <= 0.0:
        fps = 30.0
    width = int(capture.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
    total_frames = int(capture.get(cv2.CAP_PROP_FRAME_COUNT))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    writer = cv2.VideoWriter(
        str(output_path),
        cv2.VideoWriter_fourcc(*"mp4v"),
        fps,
        (width, height),
    )
    if not writer.isOpened():
        capture.release()
        raise RuntimeError(f"无法创建输出视频：{output_path}")

    service = MediaPipePoseHandService()
    started = time.perf_counter()
    processed_frames = 0
    pose_frames = 0
    right_palm_frames = 0

    try:
        while True:
            ok, frame = capture.read()
            if not ok:
                break

            annotated, payload = service.process_bgr(frame)
            writer.write(annotated)
            processed_frames += 1
            pose_frames += int(bool(payload.get("has_pose")))
            right_palm_frames += int(bool(payload.get("has_right_palm")))

            if processed_frames % 100 == 0 or processed_frames == total_frames:
                elapsed = time.perf_counter() - started
                speed = processed_frames / elapsed if elapsed > 0.0 else 0.0
                print(
                    f"处理帧={processed_frames}/{total_frames} "
                    f"Pose有效帧={pose_frames} 右掌有效帧={right_palm_frames} 速度={speed:.2f}fps",
                    flush=True,
                )
            if args.max_frames > 0 and processed_frames >= args.max_frames:
                break
    finally:
        service.close()
        capture.release()
        writer.release()

    elapsed = time.perf_counter() - started
    print(f"输出视频={output_path}")
    print(f"处理帧数={processed_frames}")
    print(f"Pose有效帧数={pose_frames}")
    print(f"右掌有效帧数={right_palm_frames}")
    print(f"处理耗时秒={elapsed:.2f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
