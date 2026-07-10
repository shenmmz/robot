# -*- coding: utf-8 -*-
"""姿态 TCP 通信模块。

本模块位于“UI/过滤后的 payload -> 外部控制程序”之间，主要职责：
1. 将 dataclass/Enum 等 Python 对象转换为 JSON 可序列化结构；
2. 从完整推理 payload 中提取电机控制需要的紧凑字段；
3. 通过 `stage1_comm.protocol` 打包消息并用 QTcpSocket 发送；
4. 接收外部控制命令并回 ACK。

电机映射：
- 6号电机：手腕翻转角（signed_roll_deg），左转负/右转正
- 5号电机：手腕上下角（wrist_pitch_deg），上翻正/下翻负
- 4号电机：肘部左右翻转（elbow_abduction_deg），左偏负/右偏正
- 3号电机：肘部上下翻转（elbow_flex_deg），上抬正/下垂负
- 2号电机：肩部左右（shoulder_abduction_deg）
- 1号电机：肩部上下（shoulder_flexion_deg）
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
    """把 Python 对象递归转换为 JSON 友好结构。

    输入：
    - value：可能是 dataclass、Enum、dict、list 或基础类型。

    输出：
    - 可被 `json.dumps()` 处理的数据结构。
    """
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
    """把任意值转换为 bool；None 统一视为 False。"""
    return bool(value) if value is not None else False


def _as_float(value: Any) -> float | None:
    """安全转换浮点数。

    输入：任意值。
    输出：float 或 None；bool 不作为数值处理。
    """
    if isinstance(value, bool):
        return None
    if isinstance(value, (int, float)):
        return float(value)
    return None


def _round_deg(value: Any) -> float | None:
    """角度保留 1 位小数，用于通信输出。"""
    number = _as_float(value)
    if number is None:
        return None
    return round(number, 1)


def _motor_direction_text(direction: str) -> str:
    """将 6 号电机方向枚举转换为中文文本。"""
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
    """压缩 6 号电机命令。

    输入：
    - motor：完整 payload 中的 `motor_6` 字段。

    输出：
    - 面向外部控制端的紧凑字典，包含方向、角度、状态和原因。

    功能实现：
    - 缺失/无效时输出保持状态；
    - 有效时提取手腕 roll 和 pitch；
    - `motion_allowed=True` 时状态为 send，否则为 hold。
    """
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
    # 最终是否允许发送动作由推理有效性和过滤层 motion_allowed 同时决定。
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


def _extract_right_arm_angle(angles: list | None, field: str) -> float | None:
    """从 angles 列表中提取右臂的指定角度字段。"""
    if not angles or not isinstance(angles, list):
        return None
    for a in angles:
        if isinstance(a, dict) and a.get("side") == "right":
            val = a.get(field)
            if val is not None:
                return _round_deg(val)
    return None


def _compact_motor_3(angles: list | None) -> dict[str, Any]:
    """Motor 3：肘部上下（elbow_flex_deg）。"""
    flex_deg = _extract_right_arm_angle(angles, "elbow_flex_deg")
    return {
        "motor_id": 3,
        "pwm_index": 3,
        "target": "right_elbow_flexion",
        "joint": "elbow",
        "axis": "flexion",
        "valid": flex_deg is not None,
        "angle_deg": flex_deg,
    }


def _compact_motor_4(angles: list | None) -> dict[str, Any]:
    """Motor 4：肘部左右翻转（elbow_abduction_deg）。"""
    abduction_deg = _extract_right_arm_angle(angles, "elbow_abduction_deg")
    direction = "hold"
    if abduction_deg is not None:
        threshold = 3.0
        if abduction_deg > threshold:
            direction = "right"   # 向右偏
        elif abduction_deg < -threshold:
            direction = "left"    # 向左偏
    return {
        "motor_id": 4,
        "pwm_index": 4,
        "target": "right_elbow_abduction",
        "joint": "elbow",
        "axis": "abduction",
        "valid": abduction_deg is not None,
        "angle_deg": abduction_deg,
        "direction": direction,
    }


def _motor_summary(command: dict[str, Any]) -> str:
    """生成发送给 UI/日志使用的电机摘要文本。

    输入：
    - command：`_compact_motor_6()` 输出的紧凑命令。

    输出：
    - 中文摘要字符串。
    """
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
    """将完整推理 payload 压缩为通信 payload。

    输入：
    - body：UI 层传入的完整 payload；
    - client_name：当前客户端名称。

    输出：
    - 外部控制端需要的紧凑 payload。

    功能实现：
    - 提取 3/4/6 号电机输出；
    - 保留过滤状态和帧号；
    - 生成 summary 便于接收端日志显示。
    """
    # 6 号电机来自 motor_6 字段；3/4 号电机从右臂 angles 中提取。
    command = _compact_motor_6(body.get("motor_6"))
    angles = body.get("angles")
    motor_3 = _compact_motor_3(angles)
    motor_4 = _compact_motor_4(angles)
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
        "motor_3": motor_3,
        "motor_4": motor_4,
        "motor_6": command,
        "motor_commands": [command] if command.get("valid") and command.get("motion_allowed") else [],
    }
    compact["summary"] = _motor_summary(command)
    return compact


class PoseCommClient(QObject):
    """姿态 TCP 客户端。

    输入：
    - host/port/client_name：外部控制服务地址与客户端标识。

    输出：
    - 通过 TCP 发送 `pose`、`hello`、`ack` 等协议消息。

    功能：
    - 管理 QTcpSocket 生命周期；
    - 周期性尝试连接；
    - 对外提供 `send_pose()` 发送过滤后的姿态/电机 payload。
    """
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
        """初始化 TCP 客户端。

        输入：
        - parent：Qt 父对象；
        - host/port：外部主控 TCP 地址；
        - client_name：本客户端标识；
        - reconnect_interval_ms：断线重连周期。

        输出：无。
        功能：创建 QTcpSocket、重连定时器和待发送消息缓存。
        """
        super().__init__(parent)
        self._socket = QTcpSocket(self)
        # Qt 网络信号统一绑定到本类私有槽函数，避免主窗口直接处理 socket 细节。
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
        """更新 TCP 连接目标。

        输入：
        - host/port：新的主控地址；
        - client_name：可选客户端名称。

        输出：无。
        """
        self._host = host.strip()
        self._port = int(port)
        if client_name is not None and client_name.strip():
            self._client_name = client_name.strip()

    def start(self) -> bool:
        """启动通信客户端。

        输入：无。
        输出：True 表示已开始连接流程，False 表示未配置地址。
        """
        if not self._enabled():
            self.statusChanged.emit("姿态通讯未启用")
            return False
        if not self._reconnect_timer.isActive():
            self._reconnect_timer.start()
        return self._ensure_connected()

    def stop(self):
        """停止通信客户端。

        输入：无。
        输出：无。
        功能：停止重连、清空缓存、断开 socket。
        """
        self._reconnect_timer.stop()
        self._pending_message = None
        self._buffer.clear()
        self._last_peer = ""
        if self._socket.state() != QTcpSocket.SocketState.UnconnectedState:
            self._socket.abort()

    def is_connected(self) -> bool:
        """返回当前 TCP socket 是否处于已连接状态。"""
        return self._socket.state() == QTcpSocket.SocketState.ConnectedState

    def send_message(
        self,
        message_type: str,
        payload: dict[str, Any] | None = None,
        msg: str | None = None,
        **extra: Any,
    ) -> bool:
        """发送一条协议消息。

        输入：
        - message_type：消息类型，如 `pose`、`hello`、`ack`；
        - payload：消息载荷；
        - msg：可读摘要；
        - extra：额外协议字段。

        输出：
        - True 表示已直接写入 socket；
        - False 表示未启用或已缓存等待连接。
        """
        if not self._enabled():
            return False

        # build_message 统一封装 seq/source/payload/msg 等协议字段。
        message = build_message(message_type, self._next_seq(), "python", payload=payload, msg=msg, **extra)
        return self._send_or_queue(message)

    def send_pose(self, frame_id: int | None, payload: dict[str, Any]) -> bool:
        """发送一帧姿态/电机结果。

        输入：
        - frame_id：当前摄像头帧编号；
        - payload：过滤后的完整推理结果。

        输出：
        - True 表示已发送，False 表示暂存或未启用。

        功能实现：
        - 先将 payload 转成 JSON 友好结构；
        - 再压缩为外部控制端关心的电机字段；
        - 最后通过 `send_message("pose", ...)` 发送。
        """
        body = _jsonable(payload)
        if not isinstance(body, dict):
            body = {"data": body}

        if frame_id is not None:
            body.setdefault("frame_id", int(frame_id))
        body.setdefault("source", "vision")
        body.setdefault("client", self._client_name)

        # 外部主控不需要完整 MediaPipe 字段，只发送紧凑电机 payload。
        compact = _compact_pose_payload(body, self._client_name)
        return self.send_message("pose", payload=compact, msg=compact.get("summary", "motor6 update"))

    def _enabled(self) -> bool:
        """判断通信端点是否配置完整。"""
        return bool(self._host and self._port > 0)

    def _next_seq(self) -> int:
        """生成递增消息序号。"""
        self._seq += 1
        return self._seq

    def _ensure_connected(self) -> bool:
        """确保 socket 正在连接或已经连接。

        输入：无。
        输出：True 表示连接流程可继续，False 表示端点未配置。
        """
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
        """发送消息或缓存为待发送消息。

        输入：完整协议消息字典。
        输出：True 表示已写入 socket，False 表示已缓存等待连接。
        """
        if self.is_connected():
            self._write_message(message)
            return True

        # 连接未建立时只保留最新一条姿态消息，避免离线积压。
        self._pending_message = message
        self._ensure_connected()
        return False

    def _write_message(self, message: dict[str, Any]):
        """将协议消息编码并写入 socket。

        输入：完整协议消息字典。
        输出：无。
        """
        data = encode_message(message)
        self._socket.write(data)
        self._socket.flush()

    def _on_connected(self):
        """socket 连接成功回调。

        功能：发送 hello，并补发最近一条缓存消息。
        """
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
        """socket 断开回调。"""
        self._last_peer = ""
        self.disconnected.emit()
        self.statusChanged.emit("姿态通讯已断开")

    def _on_error(self, error: QAbstractSocket.SocketError):
        """socket 错误回调。

        输入：Qt socket 错误枚举。
        输出：通过信号向 UI 报告可读状态。
        """
        if error == QAbstractSocket.SocketError.RemoteHostClosedError:
            return

        if error == QAbstractSocket.SocketError.ConnectionRefusedError:
            self.statusChanged.emit(f"等待 Qt 主控连接: {self._host}:{self._port}")
            return

        self.errorOccurred.emit(self._socket.errorString())

    def _on_ready_read(self):
        """读取并解析 socket 收到的协议行。

        输入：无，Qt 在 readyRead 信号触发时调用。
        输出：通过 messageReceived 信号或 ACK 消息反馈。
        """
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
