from __future__ import annotations

"""
推理线程模块。

该模块用于把“Qt 摄像头帧（QImage）”从 UI 线程转移到后台线程处理，避免阻塞界面：
1) UI 线程在 videoFrameChanged 回调中得到 QImage；
2) submit() 只做一次拷贝并唤醒线程，不做重计算；
3) run() 内部使用 OpenCV/NumPy 做格式转换，再调用 MediaPipeHolisticService 推理；
4) 推理结果以处理后的 QImage + payload(dict) 的形式通过信号回传到 UI。

注意：
- 线程内部只保留“最新一帧”，旧帧会被覆盖，以降低延迟；
- 所有 Qt UI 更新必须在主线程做，所以这里仅 emit 信号。
"""

from PySide6.QtCore import QMutex, QThread, QWaitCondition, Signal
from PySide6.QtGui import QImage


class FrameProcessorThread(QThread):
    """摄像头帧处理线程。

    信号：
    - frame_ready(QImage, dict)：处理完成的画面与推理 payload
    - error(str)：初始化或推理过程中出现的错误（会导致线程退出）
    """
    frame_ready = Signal(QImage, dict)
    error = Signal(str)

    def __init__(self, parent=None):
        """初始化线程与同步原语。"""
        super().__init__(parent)
        self._mutex = QMutex()
        self._cond = QWaitCondition()
        self._latest = None

    def submit(self, image: QImage):
        """提交一帧待处理图像。

        该方法会：
        - 拷贝 QImage，避免 QVideoFrame 生命周期影响；
        - 覆盖旧帧，仅保留最新一帧；
        - 唤醒处理线程开始计算。
        """
        if image.isNull():
            return
        self._mutex.lock()
        try:
            self._latest = image.copy()
            self._cond.wakeOne()
        finally:
            self._mutex.unlock()

    def run(self):
        """线程主循环：等待帧 -> 转换 -> 推理 -> 回传。"""
        try:
            import cv2
            import numpy as np
        except Exception as exc:
            self.error.emit(f"OpenCV/NumPy 未安装或导入失败: {exc}")
            return

        try:
            from app.services import MediaPipePoseHandService
        except Exception as exc:
            self.error.emit(f"MediaPipe 服务初始化失败: {exc}")
            return

        service = None
        try:
            service = MediaPipePoseHandService()
            while not self.isInterruptionRequested():
                self._mutex.lock()
                try:
                    if self._latest is None:
                        self._cond.wait(self._mutex, 100)
                    image = self._latest
                    self._latest = None
                finally:
                    self._mutex.unlock()

                if image is None:
                    continue

                rgb = _qimage_to_rgb_numpy(image, np)
                bgr = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)
                annotated_bgr, payload = service.process_bgr(bgr)
                annotated_rgb = cv2.cvtColor(annotated_bgr, cv2.COLOR_BGR2RGB)
                qimg = _rgb_numpy_to_qimage(annotated_rgb, QImage)
                self.frame_ready.emit(qimg, payload)
        except Exception as exc:
            self.error.emit(str(exc))
        finally:
            if service is not None:
                service.close()


def _qimage_to_rgb_numpy(image: QImage, np_module):
    """把 QImage 转成 RGB 的 numpy.ndarray（H, W, 3），数据类型 uint8。"""
    img = image.convertToFormat(QImage.Format.Format_RGB888)
    w = img.width()
    h = img.height()
    bpl = img.bytesPerLine()
    ptr = img.bits()
    byte_count = bpl * h
    if hasattr(ptr, "setsize"):
        ptr.setsize(byte_count)
    arr = np_module.frombuffer(ptr, np_module.uint8, count=byte_count).reshape((h, bpl))
    arr = arr[:, : w * 3].reshape((h, w, 3))
    return arr.copy()


def _rgb_numpy_to_qimage(rgb, QImageCls):
    """把 RGB numpy.ndarray 转回 QImage（会 copy 一份，确保数据独立）。"""
    h, w, _ = rgb.shape
    qimg = QImageCls(rgb.data, w, h, w * 3, QImageCls.Format.Format_RGB888)
    return qimg.copy()
