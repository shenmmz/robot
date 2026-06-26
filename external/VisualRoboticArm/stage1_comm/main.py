from __future__ import annotations

import json
import time
from pathlib import Path

from PySide6.QtCore import Qt, QTimer
from PySide6.QtWidgets import (
    QApplication,
    QCheckBox,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPlainTextEdit,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

from .bridge import LocalCommBridge
from .process_controller import PythonProcessController


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Stage1 通讯框架")
        self.resize(980, 640)

        self.bridge = LocalCommBridge(self)
        self.python = PythonProcessController(self)
        self._last_message_at = 0.0

        self._build_ui()
        self._connect_signals()

        self.bridge.start_server("127.0.0.1", self.port_spin.value())
        self._append_log("SYS", "Qt 主控已启动，通讯服务已就绪")
        if self.auto_start_check.isChecked():
            QTimer.singleShot(300, self.start_python)

        self.watchdog = QTimer(self)
        self.watchdog.setInterval(500)
        self.watchdog.timeout.connect(self._refresh_timeout_state)
        self.watchdog.start()

    def _build_ui(self):
        central = QWidget(self)
        self.setCentralWidget(central)

        top_box = QGroupBox("通讯配置")
        top_layout = QGridLayout(top_box)
        top_layout.addWidget(QLabel("Host"), 0, 0)
        self.host_label = QLabel("127.0.0.1")
        top_layout.addWidget(self.host_label, 0, 1)
        top_layout.addWidget(QLabel("Port"), 0, 2)
        self.port_spin = QSpinBox()
        self.port_spin.setRange(1024, 65535)
        self.port_spin.setValue(5001)
        top_layout.addWidget(self.port_spin, 0, 3)
        self.auto_start_check = QCheckBox("启动时自动拉起 Python")
        self.auto_start_check.setChecked(True)
        top_layout.addWidget(self.auto_start_check, 1, 0, 1, 4)

        control_box = QGroupBox("进程控制")
        control_layout = QHBoxLayout(control_box)
        self.start_python_button = QPushButton("启动 Python")
        self.stop_python_button = QPushButton("停止 Python")
        self.restart_python_button = QPushButton("重启 Python")
        self.send_button = QPushButton("发送命令")
        self.clear_button = QPushButton("清空日志")
        control_layout.addWidget(self.start_python_button)
        control_layout.addWidget(self.stop_python_button)
        control_layout.addWidget(self.restart_python_button)
        control_layout.addWidget(self.send_button)
        control_layout.addWidget(self.clear_button)

        message_box = QGroupBox("测试消息")
        message_layout = QHBoxLayout(message_box)
        self.command_edit = QLineEdit()
        self.command_edit.setPlaceholderText("输入要发给 Python 的命令，默认 ping")
        self.command_edit.setText("ping")
        message_layout.addWidget(self.command_edit, 1)

        status_box = QGroupBox("状态")
        status_layout = QGridLayout(status_box)
        self.server_status_label = QLabel("未启动")
        self.python_status_label = QLabel("未启动")
        self.link_status_label = QLabel("未连接")
        self.last_message_label = QLabel("-")
        status_layout.addWidget(QLabel("Qt 服务"), 0, 0)
        status_layout.addWidget(self.server_status_label, 0, 1)
        status_layout.addWidget(QLabel("Python"), 1, 0)
        status_layout.addWidget(self.python_status_label, 1, 1)
        status_layout.addWidget(QLabel("链路"), 2, 0)
        status_layout.addWidget(self.link_status_label, 2, 1)
        status_layout.addWidget(QLabel("最近消息"), 3, 0)
        status_layout.addWidget(self.last_message_label, 3, 1)

        self.log_view = QPlainTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setMaximumBlockCount(2000)

        layout = QVBoxLayout(central)
        layout.addWidget(top_box)
        layout.addWidget(control_box)
        layout.addWidget(message_box)
        layout.addWidget(status_box)
        layout.addWidget(self.log_view, 1)

        self.statusBar().showMessage("就绪")

    def _connect_signals(self):
        self.start_python_button.clicked.connect(self.start_python)
        self.stop_python_button.clicked.connect(self.stop_python)
        self.restart_python_button.clicked.connect(self.restart_python)
        self.send_button.clicked.connect(self.send_command)
        self.clear_button.clicked.connect(self.log_view.clear)

        self.bridge.clientConnected.connect(self._on_client_connected)
        self.bridge.clientDisconnected.connect(self._on_client_disconnected)
        self.bridge.messageReceived.connect(self._on_message_received)
        self.bridge.errorOccurred.connect(lambda text: self._append_log("BRIDGE", text))
        self.bridge.statusChanged.connect(lambda text: self._set_server_status(text))

        self.python.started.connect(lambda text: self._set_python_status(text))
        self.python.finished.connect(lambda code: self._set_python_status(f"Python 已退出，code={code}"))
        self.python.crashed.connect(lambda code: self._set_python_status(f"Python 崩溃，code={code}"))
        self.python.stdoutLine.connect(lambda line: self._append_log("PY", line))
        self.python.stderrLine.connect(lambda line: self._append_log("PY-ERR", line))
        self.python.errorOccurred.connect(lambda text: self._set_python_status(text))

    def _set_server_status(self, text: str):
        self.server_status_label.setText(text)
        self.statusBar().showMessage(text, 3000)

    def _set_python_status(self, text: str):
        self.python_status_label.setText(text)
        self.statusBar().showMessage(text, 3000)

    def _set_link_status(self, text: str):
        self.link_status_label.setText(text)

    def _append_log(self, tag: str, text: str):
        stamp = time.strftime("%H:%M:%S")
        self.log_view.appendPlainText(f"[{stamp}] [{tag}] {text}")

    def _on_client_connected(self, peer: str):
        self._last_message_at = time.monotonic()
        self._set_link_status(f"已连接 {peer}")
        self._append_log("QT", f"客户端已连接: {peer}")

    def _on_client_disconnected(self):
        self._set_link_status("未连接")
        self._append_log("QT", "客户端已断开")

    def _on_message_received(self, message: dict):
        self._last_message_at = time.monotonic()
        self.last_message_label.setText(f"{message.get('type', '-') } / seq {message.get('seq', '-')}")
        self._append_log("RX", json.dumps(message, ensure_ascii=False))

        message_type = message.get("type")
        if message_type == "hello":
            self.bridge.send_message(
                "ack",
                payload={"server": "qt", "ok": True},
                msg="hello ok",
            )
        elif message_type == "cmd":
            payload = message.get("payload") or {}
            command_name = payload.get("text") or payload.get("name") or "unknown"
            self._append_log("QT", f"收到命令: {command_name}")
            self.bridge.send_message(
                "ack",
                payload={"cmd": command_name, "ok": True},
                msg="command ok",
            )

    def _refresh_timeout_state(self):
        if not self.bridge.is_connected():
            return
        elapsed = time.monotonic() - self._last_message_at
        if elapsed > 3.0:
            self._set_link_status(f"连接超时 {elapsed:.1f}s")

    def start_python(self, checked: bool = False):
        host = self.host_label.text()
        port = self.port_spin.value()
        if self.python.is_running():
            self._append_log("SYS", "Python 已在运行")
            return
        script_name = "stage1_comm.python_sub.main"
        if self.python.start(host, port, name="vision-sub"):
            self._append_log("SYS", f"已启动 Python 子进程: {script_name}")

    def stop_python(self, checked: bool = False):
        if not self.python.is_running():
            self._append_log("SYS", "Python 当前未运行")
            return
        self.python.stop()
        self._append_log("SYS", "已请求停止 Python 子进程")

    def restart_python(self, checked: bool = False):
        self.stop_python()
        QTimer.singleShot(300, self.start_python)

    def send_command(self, checked: bool = False):
        text = self.command_edit.text().strip() or "ping"
        if not self.bridge.is_connected():
            QMessageBox.warning(self, "提示", "当前没有连接的 Python 客户端")
            return
        if self.bridge.send_message("cmd", payload={"text": text}, msg=text):
            self._append_log("TX", text)

    def closeEvent(self, event):
        self.watchdog.stop()
        self.stop_python()
        self.bridge.stop_server()
        super().closeEvent(event)


def main():
    app = QApplication([])
    app.setApplicationName("Stage1Comm")
    window = MainWindow()
    window.show()
    raise SystemExit(app.exec())
