# -*- coding: utf-8 -*-
"""姿态 TCP 通信模块。

本模块位于“UI/过滤后的 payload -> 外部控制程序”之间，主要职责：
1. 将 dataclass/Enum 等 Python 对象转换为 JSON 可序列化结构；
2. 从完整推理 payload 中提取电机控制需要的紧凑字段；
3. 通过 `stage1_comm.protocol` 打包消息并用 QTcpSocket 发送；
4. 接收外部控制命令并回 ACK。

电机映射不在本文件硬编码，统一由 `solver.motor_mapping` 提供。
本文件只负责把映射表中的角度字段翻译成发送 payload。
"""

from __future__ import annotations

from dataclasses import asdict, is_dataclass
from enum import Enum
from typing import Any

from PySide6.QtCore import QObject, QTimer, Signal
from PySide6.QtNetwork import QAbstractSocket, QTcpSocket

from solver.motor_mapping import MotorMapping, iter_motor_mappings
from stage1_comm.protocol import build_message, decode_message, encode_message

_PROTOCOL_VERSION = "vision_motor_v1"
_CONTROL_STAGE = "mapped_motor_outputs"


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


def _nested_get(data: Any, field_path: str) -> Any:
    """按 `a.b.c` 路径从嵌套 dict 中取值。

    输入：
    - data：待读取的字典；
    - field_path：字段路径，如 `wrist_rotation.horizontal_deg`。

    输出：
    - 字段值；任一级缺失时返回 None。
    """
    current = data
    for part in field_path.split("."):
        if not isinstance(current, dict):
            return None
        current = current.get(part)
    return current


def _normalize_side(value: Any) -> str:
    """把 side 字段标准化为字符串。"""
    return str(getattr(value, "value", value) or "").lower()


def _extract_arm_angle(angles: Any, mapping: MotorMapping) -> float | None:
    """根据映射表从 `angles` 列表中提取角度。

    输入：
    - angles：推理 payload 中的左右臂角度列表；
    - mapping：单个电机映射配置。

    输出：
    - 映射字段对应的角度，无法提取时返回 None。
    """
    if not isinstance(angles, list):
        return None

    for arm in angles:
        if not isinstance(arm, dict):
            continue
        if _normalize_side(arm.get("side")) != mapping.side:
            continue
        return _round_deg(_nested_get(arm, mapping.angle_field))
    return None


def _extract_motor_angle(body: dict[str, Any], mapping: MotorMapping) -> tuple[float | None, dict[str, Any] | None]:
    """根据映射表从完整 payload 中提取角度和原始来源字典。

    输入：
    - body：过滤后的完整推理 payload；
    - mapping：单个电机映射配置。

    输出：
    - `(angle, source_dict)`：角度值和原始来源字典；来源不是 dict 时返回 None。
    """
    if mapping.source == "angles":
        return _extract_arm_angle(body.get("angles"), mapping), None

    source = body.get(mapping.source)
    if isinstance(source, dict):
        return _round_deg(_nested_get(source, mapping.angle_field)), source
    return None, None


def _direction_from_angle(angle: float | None, mapping: MotorMapping) -> tuple[str, str]:
    """根据角度正负和映射表生成方向枚举与中文文案。"""
    if angle is None:
        return mapping.neutral_direction, mapping.neutral_text
    if angle > mapping.threshold_deg:
        return mapping.positive_direction, mapping.positive_text
    if angle < -mapping.threshold_deg:
        return mapping.negative_direction, mapping.negative_text
    return mapping.neutral_direction, mapping.neutral_text


def _mapped_motor_command(body: dict[str, Any], mapping: MotorMapping) -> dict[str, Any]:
    """把单条映射配置转换为外部控制端电机命令。

    输入：
    - body：过滤后的完整推理 payload；
    - mapping：单个电机映射配置。

    输出：
    - 面向通信协议的电机命令字典。

    功能实现：
    - 本函数不写死任何关节字段；
    - 所有关节字段、电机编号、方向文案均来自 `solver.motor_mapping`；
    - 推理字段缺失时输出保持状态，不影响程序运行。
    """
    raw_angle, source_dict = _extract_motor_angle(body, mapping)
    signed_angle = _round_deg(raw_angle + mapping.angle_offset_deg) if raw_angle is not None else None
    direction, direction_text = _direction_from_angle(signed_angle, mapping)

    valid = signed_angle is not None
    source_valid = True
    source_motion_allowed = True
    reason = "ok" if valid else f"{mapping.angle_field}_missing"

    if isinstance(source_dict, dict):
        source_valid = _as_bool(source_dict.get("valid", True))
        source_motion_allowed = _as_bool(source_dict.get("motion_allowed", True))
        reason = str(source_dict.get("reason") or reason)

    if mapping.source == "angles" and not _as_bool(body.get("pose_valid", body.get("has_pose", True))):
        source_motion_allowed = False
        if valid:
            reason = "pose_not_valid"

    valid = valid and source_valid
    motion_allowed = valid and source_motion_allowed

    # 每帧始终发送全部有效电机，由 C++ 侧按 J0-J5 勾选决定实际执行哪些轴
    state = "send" if valid and mapping.send_enabled else "hold"

    angle_deg = abs(signed_angle) if (signed_angle is not None and mapping.use_absolute_angle) else signed_angle

    return {
        "motor_id": mapping.motor_id,
        "pwm_index": mapping.resolved_pwm_index,
        "angle_name": mapping.angle_name,
        "description": mapping.description,
        "target": mapping.target,
        "joint": mapping.joint,
        "axis": mapping.axis,
        "side": mapping.side,
        "source": mapping.source,
        "angle_field": mapping.angle_field,
        "state": state,
        "valid": valid,
        "motion_allowed": motion_allowed,
        "send_enabled": mapping.send_enabled,
        "direction": direction,
        "direction_text": direction_text,
        "angle_deg": angle_deg,
        "signed_angle_deg": signed_angle,
        "reason": reason,
    }


def _mapped_motor_commands(body: dict[str, Any]) -> tuple[dict[str, dict[str, Any]], list[dict[str, Any]]]:
    """根据映射表生成全部电机输出和实际发送命令列表。"""
    motors: dict[str, dict[str, Any]] = {}
    commands: list[dict[str, Any]] = []
    for mapping in iter_motor_mappings():
        command = _mapped_motor_command(body, mapping)
        motors[f"motor_{mapping.motor_id}"] = command
        if command.get("state") == "send":
            commands.append(command)
    return motors, commands


def _mapped_motor_summary(motors: dict[str, dict[str, Any]]) -> str:
    """按映射表输出电机摘要文本，便于 UI/日志查看实际发送语句。"""
    lines: list[str] = []
    for mapping in iter_motor_mappings():
        command = motors.get(f"motor_{mapping.motor_id}", {})
        state_text = "发送" if command.get("state") == "send" else "保持"
        angle = command.get("angle_deg")
        angle_text = "-" if angle is None else f"{angle:g}°"
        direction_text = command.get("direction_text") or mapping.neutral_text
        lines.append(f"{mapping.motor_id}号电机：{state_text} | {direction_text} {angle_text}")
    return "\n".join(lines)


def _compact_pose_payload(body: dict[str, Any], client_name: str) -> dict[str, Any]:
    """将完整推理 payload 压缩为通信 payload。

    输入：
    - body：UI 层传入的完整 payload；
    - client_name：当前客户端名称。

    输出：
    - 外部控制端需要的紧凑 payload。

    功能实现：
    - 按 `solver.motor_mapping` 提取所有配置的电机输出；
    - 保留过滤状态和帧号；
    - 生成 summary 便于接收端日志显示。
    """
    motors, motor_commands = _mapped_motor_commands(body)
    compact = {
        "protocol_version": _PROTOCOL_VERSION,
        "source": body.get("source", "vision"),
        "client": body.get("client", client_name),
        "frame_id": body.get("frame_id"),
        "control_stage": _CONTROL_STAGE,
        "pose_valid": _as_bool(body.get("pose_valid", body.get("has_right_hand"))),
        "motion_allowed": bool(motor_commands),
        "filter_state": body.get("filter_state"),
        "filter_reason": body.get("filter_reason"),
        "motors": motors,
        "motor_commands": motor_commands,
    }
    # 保留 motor_1...motor_6 顶层字段，兼容已有接收端和日志查看习惯。
    compact.update(motors)
    compact["summary"] = _mapped_motor_summary(motors)
    return compact


def build_compact_pose_payload(payload: Any, client_name: str = "visual-arm") -> dict[str, Any]:
    """对外构建通信 payload。

    输入：
    - payload：过滤后的完整推理结果；
    - client_name：客户端名称。

    输出：
    - 已按映射表压缩后的电机通信 payload。
    """
    body = _jsonable(payload)
    if not isinstance(body, dict):
        body = {"data": body}
    body.setdefault("source", "vision")
    body.setdefault("client", client_name)
    return _compact_pose_payload(body, client_name)


def format_mapped_motor_summary(payload: Any) -> str:
    """按映射表生成 UI/日志用电机摘要。

    输入：
    - payload：过滤后的完整推理结果。

    输出：
    - 每个映射电机一行的中文摘要。
    """
    return build_compact_pose_payload(payload).get("summary", "")


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
        self._last_vison_items: list = []

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

        # 外部主控不需要完整 MediaPipe 字段，只发送紧凑电机 payload。
        compact = build_compact_pose_payload(body, self._client_name)
        cmds = compact.get("motor_commands", [])
        if cmds:
            items = [f"{cmd['motor_id']}:{cmd.get('signed_angle_deg', cmd.get('angle_deg', '?'))}" for cmd in cmds]
            if items != self._last_vison_items:
                self._last_vison_items = items
                print(f"[VISION] frame={frame_id} send {items}", flush=True)
        return self.send_message("pose", payload=compact, msg=compact.get("summary", "mapped motor update"))

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
