"""
电机映射：
- 6号电机：手腕翻转角（signed_roll_deg），左转负/右转正
- 5号电机：手腕上下角（wrist_pitch_deg），上翻正/下翻负
"""

from __future__ import annotations

from dataclasses import asdict, is_dataclass
from enum import Enum
from typing import Any

from PySide6.QtCore import QObject, QTimer, Signal
from PySide6.QtNetwork import QAbstractSocket, QTcpSocket

from stage1_comm.protocol import build_message, decode_message, encode_message

_PROTOCOL_VERSION = "vision_motor_v1"
_CONTROL_STAGE = "right_wrist_motor_6_only"


def _jsonable(value: Any) -> Any:
    if is_dataclass(value):
        return _jsonable(asdict(value))
    if isinstance(value, Enum):
        return value.value

    if isinstance(value, dict):
        return {str(key): _jsonable(item) for key, item in value.items()}
    if isinstance(value, (list, tuple)):
        return [_jsonable(item) for item in value]
    return value


def _as_bool(value: Any) -> bool:
    return bool(value) if value is not None else False


def _as_float(value: Any) -> float | None:
    if isinstance(value, bool):
        return None
    if isinstance(value, (int, float)):
        return float(value)
    return None


def _round_deg(value: Any) -> float | None:
    number = _as_float(value)
    if number is None:
        return None
    return round(number, 1)


def _motor_direction_text(direction: str) -> str:
    if direction == "left":
        return "左旋转"
    if direction == "right":
        return "右旋转"
    return "保持"


def _wrist_pitch_direction_text(pitch_deg: float | None) -> str:
    """手腕上下方向中文文本。上翻为正（伸展/dorsiflexion），下翻为负（屈曲/palmar flexion）。"""
    if pitch_deg is None:
        return "保持"
    threshold = 3.0
    if pitch_deg > threshold:
        return "上翻"
    if pitch_deg < -threshold:
        return "下翻"
    return "保持"


def _wrist_pitch_direction(pitch_deg: float | None) -> str:
    """手腕上下方向枚举值。"""
    if pitch_deg is None:
        return "hold"
    threshold = 3.0
    if pitch_deg > threshold:
        return "up"
    if pitch_deg < -threshold:
        return "down"
    return "hold"


def _compact_motor_6(motor: Any) -> dict[str, Any]:
    if not isinstance(motor, dict):
        return {
            "motor_id": 6,
            "pwm_index": 6,
            "target": "right_wrist_roll",
            "state": "hold",
            "valid": False,
            "motion_allowed": False,
            "direction": "unknown",
            "direction_text": "保持",
            "angle_deg": None,
            "signed_angle_deg": None,
            "reason": "motor_6_missing",
            "wrist_pitch_deg": None,
            "motor_5": "hold",
            "motor_5_direction_text": "保持",
        }

    direction = str(motor.get("direction") or "unknown")
    angle_deg = _round_deg(motor.get("angle_deg", motor.get("roll_deg")))
    signed_angle_deg = _round_deg(motor.get("signed_angle_deg", motor.get("signed_roll_deg")))
    wrist_pitch_deg = _round_deg(motor.get("wrist_pitch_deg"))
    pitch_direction = _wrist_pitch_direction(wrist_pitch_deg)
    pitch_direction_text = _wrist_pitch_direction_text(wrist_pitch_deg)
    valid = _as_bool(motor.get("valid"))
    motion_allowed = valid and _as_bool(motor.get("motion_allowed"))
    state = "send" if motion_allowed else "hold"
    reason = str(motor.get("reason") or ("ok" if motion_allowed else "hold"))

    return {
        "motor_id": 6,
        "pwm_index": 6,
        "target": "right_wrist_roll",
        "joint": "wrist",
        "axis": "roll",
        "state": state,
        "valid": valid,
        "motion_allowed": motion_allowed,
        "direction": direction,
        "direction_text": _motor_direction_text(direction),
        "rotation_side": str(motor.get("rotation_side") or "unknown"),
        "angle_deg": angle_deg,
        "signed_angle_deg": signed_angle_deg,
        "confidence": _round_deg(motor.get("confidence")),
        "reason": reason,
        "wrist_pitch_deg": wrist_pitch_deg,
        "motor_5": pitch_direction,
        "motor_5_direction_text": pitch_direction_text,
    }


def _motor_summary(command: dict[str, Any]) -> str:
    reason = command.get("reason") or "-"
    pitch_direction_text = command.get("motor_5_direction_text") or "保持"
    pitch_deg = command.get("wrist_pitch_deg")
    pitch_part = ""
    if pitch_deg is not None:
        pitch_part = f"\n5号电机: {pitch_direction_text} {pitch_deg}°"
    if not command.get("valid"):
        return f"6号电机：保持 | 右手未有效识别 | reason={reason}{pitch_part}"

    angle = command.get("angle_deg")
    angle_text = "-" if angle is None else f"{angle:g}°"
    direction_text = command.get("direction_text") or _motor_direction_text(str(command.get("direction") or "unknown"))
    if not command.get("motion_allowed"):
        return f"6号电机：保持 | {direction_text} {angle_text} | reason={reason}{pitch_part}"
    return f"6号电机：发送 | {direction_text} {angle_text}{pitch_part}"


def _compact_pose_payload(body: dict[str, Any], client_name: str) -> dict[str, Any]:
    command = _compact_motor_6(body.get("motor_6"))
    compact = {
        "protocol_version": _PROTOCOL_VERSION,
        "source": body.get("source", "vision"),
        "client": body.get("client", client_name),
        "frame_id": body.get("frame_id"),
        "control_stage": _CONTROL_STAGE,
        "pose_valid": _as_bool(body.get("pose_valid", body.get("has_right_hand"))),
        "motion_allowed": _as_bool(command.get("motion_allowed")),
        "filter_state": body.get("filter_state"),
        "filter_reason": body.get("filter_reason"),
        "motor_6": command,
        "motor_commands": [command] if command.get("valid") and command.get("motion_allowed") else [],
    }
    compact["summary"] = _motor_summary(command)
    return compact


class PoseCommClient(QObject):
    connected = Signal(str)
    disconnected = Signal()
    messageReceived = Signal(dict)
    statusChanged = Signal(str)
    errorOccurred = Signal(str)

    def __init__(
        self,
        parent: QObject | None = None,
        host: str = "",
        port: int = 0,
        client_name: str = "visual-arm",
        reconnect_interval_ms: int = 1000,
    ):
        super().__init__(parent)
        self._socket = QTcpSocket(self)
        self._socket.connected.connect(self._on_connected)
        self._socket.disconnected.connect(self._on_disconnected)
        self._socket.readyRead.connect(self._on_ready_read)
        self._socket.errorOccurred.connect(self._on_error)

        self._buffer = bytearray()
        self._pending_message: dict[str, Any] | None = None
        self._seq = 0
        self._host = host.strip()
        self._port = int(port) if port else 0
        self._client_name = client_name.strip() or "visual-arm"
        self._last_peer = ""

        self._reconnect_timer = QTimer(self)
        self._reconnect_timer.setInterval(max(250, int(reconnect_interval_ms)))
        self._reconnect_timer.timeout.connect(self._ensure_connected)

    def set_endpoint(self, host: str, port: int, client_name: str | None = None):
        self._host = host.strip()
        self._port = int(port)
        if client_name is not None and client_name.strip():
            self._client_name = client_name.strip()

    def start(self) -> bool:
        if not self._enabled():
            self.statusChanged.emit("姿态通讯未启用")
            return False
        if not self._reconnect_timer.isActive():
            self._reconnect_timer.start()
        return self._ensure_connected()

    def stop(self):
        self._reconnect_timer.stop()
        self._pending_message = None
        self._buffer.clear()
        self._last_peer = ""
        if self._socket.state() != QTcpSocket.SocketState.UnconnectedState:
            self._socket.abort()

    def is_connected(self) -> bool:
        return self._socket.state() == QTcpSocket.SocketState.ConnectedState

    def send_message(
        self,
        message_type: str,
        payload: dict[str, Any] | None = None,
        msg: str | None = None,
        **extra: Any,
    ) -> bool:
        if not self._enabled():
            return False

        message = build_message(message_type, self._next_seq(), "python", payload=payload, msg=msg, **extra)
        return self._send_or_queue(message)

    def send_pose(self, frame_id: int | None, payload: dict[str, Any]) -> bool:
        body = _jsonable(payload)
        if not isinstance(body, dict):
            body = {"data": body}

        if frame_id is not None:
            body.setdefault("frame_id", int(frame_id))
        body.setdefault("source", "vision")
        body.setdefault("client", self._client_name)

        compact = _compact_pose_payload(body, self._client_name)
        return self.send_message("pose", payload=compact, msg=compact.get("summary", "motor6 update"))

    def _enabled(self) -> bool:
        return bool(self._host and self._port > 0)

    def _next_seq(self) -> int:
        self._seq += 1
        return self._seq

    def _ensure_connected(self) -> bool:
        if not self._enabled():
            return False

        state = self._socket.state()
        if state in (
            QTcpSocket.SocketState.ConnectingState,
            QTcpSocket.SocketState.ConnectedState,
        ):
            return True

        self.statusChanged.emit(f"正在连接 Qt 主控 {self._host}:{self._port}")
        self._socket.connectToHost(self._host, self._port)
        return True

    def _send_or_queue(self, message: dict[str, Any]) -> bool:
        if self.is_connected():
            self._write_message(message)
            return True

        self._pending_message = message
        self._ensure_connected()
        return False

    def _write_message(self, message: dict[str, Any]):
        data = encode_message(message)
        self._socket.write(data)
        self._socket.flush()

    def _on_connected(self):
        self._last_peer = f"{self._socket.peerAddress().toString()}:{self._socket.peerPort()}"
        self.connected.emit(self._last_peer)
        self.statusChanged.emit(f"已连接 Qt 主控: {self._last_peer}")

        self._write_message(
            build_message(
                "hello",
                self._next_seq(),
                "python",
                payload={"name": self._client_name, "role": "python", "ver": 1},
                msg="hello",
            )
        )

        if self._pending_message is not None:
            self._write_message(self._pending_message)
            self._pending_message = None

    def _on_disconnected(self):
        self._last_peer = ""
        self.disconnected.emit()
        self.statusChanged.emit("姿态通讯已断开")

    def _on_error(self, error: QAbstractSocket.SocketError):
        if error == QAbstractSocket.SocketError.RemoteHostClosedError:
            return

        if error == QAbstractSocket.SocketError.ConnectionRefusedError:
            self.statusChanged.emit(f"等待 Qt 主控连接: {self._host}:{self._port}")
            return

        self.errorOccurred.emit(self._socket.errorString())

    def _on_ready_read(self):
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
            self._handle_message(message)

    def _handle_message(self, message: dict[str, Any]):
        message_type = message.get("type")
        if message_type == "cmd":
            payload = message.get("payload") or {}
            command = payload.get("text") or payload.get("name") or "unknown"
            self._write_message(
                build_message(
                    "ack",
                    self._next_seq(),
                    "python",
                    payload={"cmd": command, "ok": True},
                    msg=f"ack {command}",
                )
            )
        elif message_type == "shutdown":
            self.stop()
