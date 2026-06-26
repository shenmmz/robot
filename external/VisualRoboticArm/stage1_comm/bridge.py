from __future__ import annotations

from typing import Any

from PySide6.QtCore import QObject, QTimer, Signal
from PySide6.QtNetwork import QAbstractSocket, QHostAddress, QTcpServer, QTcpSocket

from .protocol import build_message, decode_message, encode_message


class LocalCommBridge(QObject):
    clientConnected = Signal(str)
    clientDisconnected = Signal()
    messageReceived = Signal(dict)
    errorOccurred = Signal(str)
    statusChanged = Signal(str)

    def __init__(self, parent: QObject | None = None):
        super().__init__(parent)
        self._server = QTcpServer(self)
        self._server.newConnection.connect(self._on_new_connection)
        self._socket: QTcpSocket | None = None
        self._buffer = bytearray()
        self._listening = False
        self._host = "127.0.0.1"
        self._port = 5001
        self._seq = 0
        self._last_peer = ""

        self._watchdog = QTimer(self)
        self._watchdog.setInterval(1000)
        self._watchdog.timeout.connect(self._emit_status)

    def start_server(self, host: str = "127.0.0.1", port: int = 5001) -> bool:
        if self._server.isListening():
            return True

        self._host = host
        self._port = port

        if not self._server.listen(QHostAddress.LocalHost, port):
            self.errorOccurred.emit(f"监听失败: {self._server.errorString()}")
            return False

        self._listening = True
        self._watchdog.start()
        self.statusChanged.emit(f"服务器已启动: {host}:{port}")
        return True

    def stop_server(self):
        self._watchdog.stop()
        self._close_socket()
        if self._server.isListening():
            self._server.close()
        self._listening = False
        self.statusChanged.emit("服务器已停止")

    def is_listening(self) -> bool:
        return self._server.isListening()

    def is_connected(self) -> bool:
        return self._socket is not None and self._socket.state() == QTcpSocket.SocketState.ConnectedState

    def last_peer(self) -> str:
        return self._last_peer

    def send_json(self, payload: dict[str, Any]) -> bool:
        if not self.is_connected():
            self.errorOccurred.emit("发送失败: 当前没有已连接的客户端")
            return False
        assert self._socket is not None
        data = encode_message(payload)
        self._socket.write(data)
        self._socket.flush()
        return True

    def send_message(
        self,
        message_type: str,
        role: str = "qt",
        payload: dict[str, Any] | None = None,
        msg: str | None = None,
        **extra: Any,
    ) -> bool:
        self._seq += 1
        message = build_message(message_type, self._seq, role, payload=payload, msg=msg, **extra)
        return self.send_json(message)

    def _on_new_connection(self):
        socket = self._server.nextPendingConnection()
        if socket is None:
            return

        if self._socket is not None:
            self._close_socket()

        self._socket = socket
        self._buffer.clear()
        self._last_peer = f"{socket.peerAddress().toString()}:{socket.peerPort()}"
        socket.readyRead.connect(self._on_ready_read)
        socket.disconnected.connect(self._on_disconnected)
        socket.errorOccurred.connect(self._on_socket_error)
        self.clientConnected.emit(self._last_peer)
        self.statusChanged.emit(f"客户端已连接: {self._last_peer}")

    def _on_ready_read(self):
        if self._socket is None:
            return

        self._buffer.extend(bytes(self._socket.readAll()))
        while True:
            try:
                newline_index = self._buffer.index(0x0A)
            except ValueError:
                break
            raw = bytes(self._buffer[:newline_index])
            del self._buffer[: newline_index + 1]
            text = raw.decode("utf-8", errors="replace").strip()
            if not text:
                continue
            try:
                message = decode_message(text)
            except Exception as exc:
                self.errorOccurred.emit(f"JSON 解析失败: {exc}")
                continue
            self.messageReceived.emit(message)

    def _on_disconnected(self):
        self._close_socket(emit_signal=False)
        self.clientDisconnected.emit()
        self.statusChanged.emit("客户端已断开")

    def _on_socket_error(self, error: QAbstractSocket.SocketError):
        if error == QAbstractSocket.SocketError.RemoteHostClosedError:
            return
        if self._socket is None:
            return
        self.errorOccurred.emit(self._socket.errorString())

    def _close_socket(self, emit_signal: bool = True):
        if self._socket is None:
            return
        try:
            self._socket.readyRead.disconnect(self._on_ready_read)
        except Exception:
            pass
        try:
            self._socket.disconnected.disconnect(self._on_disconnected)
        except Exception:
            pass
        try:
            self._socket.errorOccurred.disconnect(self._on_socket_error)
        except Exception:
            pass
        self._socket.disconnectFromHost()
        self._socket.deleteLater()
        self._socket = None
        self._buffer.clear()
        self._last_peer = ""
        if emit_signal:
            self.clientDisconnected.emit()

    def _emit_status(self):
        if self.is_connected():
            self.statusChanged.emit(f"服务器运行中，客户端已连接: {self._last_peer}")
        elif self.is_listening():
            self.statusChanged.emit(f"服务器运行中，等待客户端: {self._host}:{self._port}")
