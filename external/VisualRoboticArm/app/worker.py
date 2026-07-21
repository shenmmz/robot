# -*- coding: utf-8 -*-
from __future__ import annotations

"""
推理线程模块。

该模块用于把“Qt 摄像头帧（QImage）”从 UI 线程转移到后台线程处理，避免阻塞界面：
1) UI 线程在 videoFrameChanged 回调中得到 QImage；
2) submit() 只做一次拷贝并唤醒线程，不做重计算；
3) run() 内部使用 OpenCV/NumPy 做格式转换，再调用 MediaPipePoseHandService 推理；
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
        """初始化线程与同步原语。

        输入：
        - parent：Qt 父对象，可为 None。

        输出：无。
        功能：创建互斥锁、条件变量和最新帧缓存。
        """
        super().__init__(parent)
        # QMutex/QWaitCondition 用于让 UI 线程提交帧、后台线程等待帧。
        self._mutex = QMutex()
        self._cond = QWaitCondition()
        # 仅缓存最新一帧，避免推理速度低于采集速度时积压延迟。
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
            # copy() 让后台线程持有独立图像，避免 QVideoFrame 生命周期结束后数据失效。
            self._latest = image.copy()
            # 唤醒 run() 中等待新帧的后台线程。
            self._cond.wakeOne()
        finally:
            self._mutex.unlock()

    def run(self):
        """线程主循环：等待帧 -> 转换 -> 推理 -> 回传。

        输入：无，线程启动后由 Qt 调用。
        输出：通过 `frame_ready` 或 `error` 信号异步返回。
        功能实现：
        - 在线程内导入 OpenCV/NumPy 和推理服务；
        - 等待 UI 线程提交最新 QImage；
        - QImage RGB -> OpenCV BGR -> 推理 -> BGR/RGB/QImage 回传。
        """
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
            # 服务对象在线程生命周期内只创建一次，避免每帧重复初始化 MediaPipe。
            service = MediaPipePoseHandService()
            while not self.isInterruptionRequested():
                self._mutex.lock()
                try:
                    if self._latest is None:
                        # 没有新帧时最多等待 100ms，便于及时响应线程中断。
                        self._cond.wait(self._mutex, 100)
                    image = self._latest
                    # 取走最新帧后立即清空缓存，下一帧可继续覆盖。
                    self._latest = None
                finally:
                    self._mutex.unlock()

                if image is None:
                    continue

                # Qt QImage 已转成 RGB888；推理服务要求 OpenCV BGR 输入。
                rgb = _qimage_to_rgb_numpy(image, np)
                bgr = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)
                # 核心调用：进入 app.services.MediaPipePoseHandService.process_bgr()。
                annotated_bgr, payload = service.process_bgr(bgr)
                # Qt 预览需要 RGB QImage，因此推理后的 BGR 画面要转回 RGB。
                annotated_rgb = cv2.cvtColor(annotated_bgr, cv2.COLOR_BGR2RGB)
                qimg = _rgb_numpy_to_qimage(annotated_rgb, QImage)
                # 所有 UI 更新都回到主线程处理，这里只通过信号传递数据。
                self.frame_ready.emit(qimg, payload)
        except Exception as exc:
            self.error.emit(str(exc))
        finally:
            if service is not None:
                service.close()


def _qimage_to_rgb_numpy(image: QImage, np_module):
    """把 QImage 转成 RGB 的 numpy.ndarray。

    输入：
    - image：Qt QImage；
    - np_module：NumPy 模块对象，在线程内导入后传入。

    输出：
    - shape 为 (H, W, 3)、dtype 为 uint8 的 RGB 数组。

    功能：处理 QImage 的 bytesPerLine 对齐问题，并 copy 成独立数组。
    """
    img = image.convertToFormat(QImage.Format.Format_RGB888)
    w = img.width()
    h = img.height()
    bpl = img.bytesPerLine()
    ptr = img.bits()
    byte_count = bpl * h
    if hasattr(ptr, "setsize"):
        ptr.setsize(byte_count)
    # QImage 每行可能有 padding，需要先按 bytesPerLine reshape，再截取有效 RGB 区域。
    arr = np_module.frombuffer(ptr, np_module.uint8, count=byte_count).reshape((h, bpl))
    arr = arr[:, : w * 3].reshape((h, w, 3))
    return arr.copy()


def _rgb_numpy_to_qimage(rgb, QImageCls):
    """把 RGB numpy.ndarray 转回 QImage。

    输入：
    - rgb：shape 为 (H, W, 3) 的 RGB 数组；
    - QImageCls：QImage 类对象，便于测试时替换。

    输出：
    - 独立持有数据的 QImage。
    """
    h, w, _ = rgb.shape
    # QImage 默认只引用外部内存，所以最后必须 copy()，否则 numpy 数组释放后会悬空。
    qimg = QImageCls(rgb.data, w, h, w * 3, QImageCls.Format.Format_RGB888)
    return qimg.copy()
