#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
make_icon.py
将 config/robot.jpeg 转换为 Windows 多尺寸 .ico 文件。

输出：resources/robot.ico（嵌入 16/24/32/48/64/128/256 像素，多尺寸打包，
     资源管理器/任务栏按 DPI 自动选择最清晰的那个）

用法（在项目根目录）：
    python tools/make_icon.py

前置依赖：Pillow（pip install Pillow）
说明：JPEG 无 alpha 通道，脚本会把接近纯白的背景像素设为透明，
     这样 icon 不会带白方块。若图像背景不是白色，调节 BG_THRESHOLD。
"""
from __future__ import annotations

import os
import sys
from PIL import Image

# ====== 路径配置 ======
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_JPEG = os.path.join(PROJECT_ROOT, "config", "robot.jpeg")
OUT_DIR = os.path.join(PROJECT_ROOT, "resources")
OUT_ICO = os.path.join(OUT_DIR, "robot.ico")

# icon 嵌入的尺寸（Windows 资源管理器/任务栏按这些尺寸选取）
ICON_SIZES = [(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]

# 背景透明度阈值：RGB 三通道均 >= 此值视为背景，alpha 置 0
# 240 是"接近纯白"的保守阈值；若图本身是白底黑线，线条灰度会保留 alpha
BG_THRESHOLD = 240


def make_white_transparent(img_rgba: Image.Image, threshold: int = BG_THRESHOLD) -> Image.Image:
    """将接近纯白的像素设为透明，让 icon 没有白方块背景。"""
    data = list(img_rgba.getdata())
    new_data = [
        (r, g, b, 0) if (r >= threshold and g >= threshold and b >= threshold)
        else (r, g, b, 255)
        for (r, g, b, _a) in data
    ]
    img_rgba.putdata(new_data)
    return img_rgba


def main() -> int:
    if not os.path.exists(SRC_JPEG):
        print(f"[ERROR] 源图不存在: {SRC_JPEG}", file=sys.stderr)
        return 1

    os.makedirs(OUT_DIR, exist_ok=True)

    img = Image.open(SRC_JPEG)
    print(f"[INFO] 源图: {SRC_JPEG}  size={img.size}  mode={img.mode}")

    # JPEG 是 RGB，统一转 RGBA 以便处理透明度
    img = img.convert("RGBA")

    # 浅色背景 → 透明
    img = make_white_transparent(img, BG_THRESHOLD)

    # 生成多尺寸并打包到 .ico
    # Pillow 会用 LANCZOS 高质量缩放，sizes 指定要嵌入的尺寸
    img.save(OUT_ICO, format="ICO", sizes=ICON_SIZES)

    size_kb = os.path.getsize(OUT_ICO) / 1024
    print(f"[OK] 生成: {OUT_ICO}  ({size_kb:.1f} KB)  sizes={ICON_SIZES}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
