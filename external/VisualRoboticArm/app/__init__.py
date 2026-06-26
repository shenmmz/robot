"""
app 包对外导出层。

默认只导入纯数据模型，避免 `solver.angle_calc -> app.models -> app.__init__`
时又提前加载 `app.services`，形成推理计算模块和业务服务模块之间的循环导入。
业务类通过懒加载方式继续对外导出，兼容后续移植时可能存在的 `from app import ...` 写法。
"""

from app.models import ArmAngles, Direction, Point3D, Side

__all__ = [
    "ArmAngles",
    "Direction",
    "Point3D",
    "Side",
    "MediaPipeHolisticService",
    "RS485Config",
    "RS485Manager",
    "RS485PortInfo",
    "FrameProcessorThread",
]


def __getattr__(name: str):
    """按需导入业务类，保持包入口兼容，同时避免推理模块初始化时循环导入。"""
    if name == "MediaPipeHolisticService":
        from app.services import MediaPipeHolisticService

        return MediaPipeHolisticService
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
