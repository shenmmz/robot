#!/usr/bin/env python3
"""
将 VisionMaster SDK 头文件从 GBK 转换为 UTF-8 编码。

用法:
    python convert_sdk_headers.py <src_dir> <dst_dir> [<src_dir2> <dst_dir2> ...]

逻辑:
    - 递归遍历 src_dir 中的所有文件
    - 尝试以 UTF-8 解码；成功则直接复制（已是 UTF-8）
    - UTF-8 解码失败则尝试 GBK 解码，成功则转换为 UTF-8 写入 dst_dir
    - 两者都失败则原样复制（二进制文件等）
    - 保持目录结构不变
"""
import os
import sys
import shutil

def convert_file(src, dst):
    """转换单个文件：GBK→UTF-8，或直接复制。"""
    os.makedirs(os.path.dirname(dst), exist_ok=True)

    with open(src, 'rb') as f:
        raw = f.read()

    # 先尝试 UTF-8（文件可能已经是 UTF-8）
    try:
        raw.decode('utf-8')
        # 已经是合法 UTF-8，直接复制
        shutil.copy2(src, dst)
        return 'utf-8'
    except UnicodeDecodeError:
        pass

    # 尝试 GBK
    try:
        text = raw.decode('gbk')
        # 转换为 UTF-8 写入
        with open(dst, 'wb') as f:
            f.write(text.encode('utf-8'))
        return 'gbk->utf-8'
    except (UnicodeDecodeError, Exception):
        pass

    # 无法识别编码，原样复制
    shutil.copy2(src, dst)
    return 'binary'

def convert_dir(src_dir, dst_dir):
    """递归转换目录中的所有文件。"""
    if not os.path.isdir(src_dir):
        print(f'  WARNING: source dir not found: {src_dir}')
        return

    stats = {'utf-8': 0, 'gbk->utf-8': 0, 'binary': 0}
    for root, dirs, files in os.walk(src_dir):
        for fname in files:
            src = os.path.join(root, fname)
            rel = os.path.relpath(src, src_dir)
            dst = os.path.join(dst_dir, rel)
            result = convert_file(src, dst)
            stats[result] = stats.get(result, 0) + 1

    print(f'  {src_dir}')
    print(f'    -> {dst_dir}')
    print(f'    UTF-8 (already): {stats.get("utf-8", 0)}, '
          f'GBK->UTF-8 (converted): {stats.get("gbk->utf-8", 0)}, '
          f'Other (copied): {stats.get("binary", 0)}')

def main():
    if len(sys.argv) < 3 or (len(sys.argv) - 1) % 2 != 0:
        print('Usage: python convert_sdk_headers.py <src1> <dst1> [<src2> <dst2> ...]')
        sys.exit(1)

    pairs = []
    args = sys.argv[1:]
    for i in range(0, len(args), 2):
        pairs.append((args[i], args[i + 1]))

    print('Converting VisionMaster SDK headers to UTF-8...')
    for src, dst in pairs:
        convert_dir(src, dst)
    print('Done.')

if __name__ == '__main__':
    main()
