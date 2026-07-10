# -*- coding: utf-8 -*-
"""
app 包对外导出层。

默认只导入纯数据模型，避免 `solver.angle_calc -> app.models -> app.__init__`
时又提前加载 `app.services`，形成推理计算模块和业务服务模块之间的循环导入。
业务类通过懒加载方式继续对外导出，兼容后续移植时可能存在的 `from app import ...` 写法。
"""

from typing import TYPE_CHECKING

import torch

from app.models import ArmAngles, Direction, Point3D, Side

if TYPE_CHECKING:
    from app.rs485 import RS485Config, RS485Manager, RS485PortInfo
    from app.worker import FrameProcessorThread

__all__ = [
    "ArmAngles",
    "Direction",
    "Point3D",
    "Side",
    "MediaPipePoseHandService",
    "RS485Config",
    "RS485Manager",
    "RS485PortInfo",
    "FrameProcessorThread",
]


def _print_torch_startup_info() -> None:
    """打印 PyTorch 运行时信息。

    输入：无。
    输出：无，信息直接打印到控制台。
    功能：应用导入 app 包时提示 torch 版本、CUDA runtime 和当前使用 GPU/CPU。
    """
    try:
        cuda_available = torch.cuda.is_available()
        device_type = "GPU" if cuda_available else "CPU"
        print(f"[VisualRoboticArm] PyTorch version: {torch.__version__}")
        print(f"[VisualRoboticArm] PyTorch CUDA runtime: {torch.version.cuda or 'None'}")
        if cuda_available:
            gpu_name = torch.cuda.get_device_name(0)
            print(f"[VisualRoboticArm] PyTorch startup success: {device_type} ({gpu_name})")
        else:
            print(f"[VisualRoboticArm] PyTorch startup success: {device_type}")
    except Exception as exc:
        print(f"[VisualRoboticArm] PyTorch startup failed: {exc}")


_print_torch_startup_info()


def __getattr__(name: str):
    """按需导入业务类，保持包入口兼容，同时避免推理模块初始化时循环导入。"""
    if name == "MediaPipePoseHandService":
        from app.services import MediaPipePoseHandService

        return MediaPipePoseHandService
    if name in {"RS485Config", "RS485Manager", "RS485PortInfo"}:
        from app.rs485 import RS485Config, RS485Manager, RS485PortInfo

        return {
            "RS485Config": RS485Config,
            "RS485Manager": RS485Manager,
            "RS485PortInfo": RS485PortInfo,
        }[name]
    if name == "FrameProcessorThread":
        from app.worker import FrameProcessorThread

        return FrameProcessorThread
    raise AttributeError(f"module 'app' has no attribute {name!r}")
