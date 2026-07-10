# -*- coding: utf-8 -*-
from __future__ import annotations

"""
项目入口模块。

职责：
1) 解析启动参数（用于与外部通信模块对接的 host/port/client 名称）。
2) 将参数写入环境变量，供业务代码在运行时读取。
3) 启动 Qt 事件循环（PySide6），创建并展示主窗口。

使用方式示例：
    python main.py
    python main.py --host 127.0.0.1 --port 5001 --name visual-arm

说明：
- 这里使用 argparse 的 parse_known_args，是为了允许未来扩展更多参数，
  同时不阻断 Qt/其他库可能添加的 argv 参数。
"""

import argparse
import os
import sys
from PySide6.QtWidgets import QApplication
from app.main_window import MainWindow


def _parse_comm_args(argv: list[str]) -> tuple[argparse.Namespace, list[str]]:
    """
    解析“通信相关”的命令行参数，并返回：
    - args：解析出的参数对象（包含 host/port/name）
    - new_argv：过滤后的 argv（保留未识别参数），用于传给 QApplication

    之所以要返回 new_argv：
    - QApplication 会读取 argv（例如处理 Qt 自身的参数），
      这里把我们自定义的参数先消费掉，避免 Qt 误解析或报错。
    """
    parser = argparse.ArgumentParser(add_help=False)
    # 通信端地址：优先 CLI，回退到环境变量 VISUAL_ARM_COMM_HOST，再回退默认值。
    parser.add_argument("--host", default=os.environ.get("VISUAL_ARM_COMM_HOST", "127.0.0.1"))
    # 通信端口号：优先 CLI，回退到环境变量 VISUAL_ARM_COMM_PORT，再回退默认值。
    parser.add_argument("--port", default=os.environ.get("VISUAL_ARM_COMM_PORT", "5001"))
    # 客户端名称/实例标识：当多实例运行时便于区分。
    parser.add_argument("--name", default=os.environ.get("VISUAL_ARM_COMM_CLIENT", "visual-arm"))
    # parse_known_args：只解析我们关心的参数，未知参数原样保留。
    args, rest = parser.parse_known_args(argv[1:])
    # 返回给 QApplication 的 argv，必须把脚本名 argv[0] 放回去。
    return args, [argv[0], *rest]


def main():
    """
    程序主入口：
    1) 解析通信参数并写入环境变量；
    2) 初始化 QApplication（Qt 事件循环）；
    3) 创建并展示主窗口；
    4) 进入 Qt 主循环，直到窗口关闭后退出进程。
    """
    args, argv = _parse_comm_args(sys.argv)
    # 通过环境变量将参数传递给后续模块，避免跨模块强耦合。
    os.environ["VISUAL_ARM_COMM_HOST"] = args.host
    os.environ["VISUAL_ARM_COMM_PORT"] = str(args.port)
    os.environ["VISUAL_ARM_COMM_CLIENT"] = args.name

    # QApplication 必须在所有 Qt 控件创建之前初始化。
    app = QApplication(argv)
    # 主窗口封装了相机枚举/采集/推理与界面渲染逻辑。
    window = MainWindow()
    window.show()
    # exec() 进入事件循环；sys.exit 确保将 Qt 返回码传给操作系统。
    sys.exit(app.exec())


if __name__ == "__main__":
    # 允许直接运行本文件启动程序；被作为模块导入时不会自动启动。
    main()
