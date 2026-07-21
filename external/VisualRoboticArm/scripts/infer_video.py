"""Run the production pose service on a video and save annotated frames."""

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
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="Source video path")
    parser.add_argument("output", type=Path, help="Annotated MP4 output path")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    input_path = args.input.resolve()
    output_path = args.output.resolve()

    capture = cv2.VideoCapture(str(input_path))
    if not capture.isOpened():
        raise RuntimeError(f"Unable to open input video: {input_path}")

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
        raise RuntimeError(f"Unable to create output video: {output_path}")

    service = MediaPipePoseHandService()
    started = time.perf_counter()
    processed_frames = 0
    pose_frames = 0

    try:
        while True:
            ok, frame = capture.read()
            if not ok:
                break

            annotated, payload = service.process_bgr(frame)
            writer.write(annotated)
            processed_frames += 1
            pose_frames += int(bool(payload.get("has_pose")))

            if processed_frames % 100 == 0 or processed_frames == total_frames:
                elapsed = time.perf_counter() - started
                speed = processed_frames / elapsed if elapsed > 0.0 else 0.0
                print(
                    f"frames={processed_frames}/{total_frames} "
                    f"pose={pose_frames} speed={speed:.2f}fps",
                    flush=True,
                )
    finally:
        service.close()
        capture.release()
        writer.release()

    elapsed = time.perf_counter() - started
    print(f"output={output_path}")
    print(f"processed_frames={processed_frames}")
    print(f"pose_frames={pose_frames}")
    print(f"elapsed_seconds={elapsed:.2f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
