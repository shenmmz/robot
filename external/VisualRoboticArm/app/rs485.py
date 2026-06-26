from __future__ import annotations

from dataclasses import dataclass

from PySide6.QtCore import QObject, QIODevice, Signal
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo


@dataclass(frozen=True)
class RS485PortInfo:
    port_name: str
    description: str = ""
    manufacturer: str = ""
    serial_number: str = ""
    system_location: str = ""

    @property
    def display_name(self) -> str:
        if self.description and self.description != self.port_name:
            return f"{self.description} ({self.port_name})"
        return self.port_name


@dataclass(frozen=True)
class RS485Config:
    port_name: str
    baud_rate: int = 115200
    data_bits: int = QSerialPort.Data8
    parity: int = QSerialPort.NoParity
    stop_bits: int = QSerialPort.OneStop
    flow_control: int = QSerialPort.NoFlowControl
    frame_terminator: bytes | None = b"\n"
    text_encoding: str = "utf-8"


class RS485Manager(QObject):
    """Minimal Qt-based RS485 helper."""

    port_opened = Signal(str)
    port_closed = Signal()
    bytes_received = Signal(bytes)
    frame_received = Signal(bytes)
    text_received = Signal(str)
    error_occurred = Signal(str)

    def __init__(self, parent: QObject | None = None, config: RS485Config | None = None):
        super().__init__(parent)
        self._port = QSerialPort(self)
        self._port.readyRead.connect(self._on_ready_read)
        self._port.errorOccurred.connect(self._on_error_occurred)
        self._buffer = bytearray()
        self._config = config or RS485Config(port_name="")

    @staticmethod
    def scan_ports() -> list[RS485PortInfo]:
        ports: list[RS485PortInfo] = []
        for info in QSerialPortInfo.availablePorts():
            ports.append(
                RS485PortInfo(
                    port_name=info.portName(),
                    description=info.description(),
                    manufacturer=info.manufacturer(),
                    serial_number=info.serialNumber(),
                    system_location=info.systemLocation(),
                )
            )
        return ports

    @property
    def is_open(self) -> bool:
        return self._port.isOpen()

    @property
    def current_port_name(self) -> str:
        return self._port.portName()

    @property
    def config(self) -> RS485Config:
        return self._config

    def open_port(self, config: RS485Config | None = None) -> bool:
        if config is not None:
            self._config = config

        if not self._config.port_name:
            self.error_occurred.emit("No RS485 port selected.")
            return False

        if self._port.isOpen():
            self.close_port()

        self._buffer.clear()
        self._port.setPortName(self._config.port_name)

        if not self._port.setBaudRate(self._config.baud_rate):
            self.error_occurred.emit(f"Failed to set baud rate: {self._port.errorString()}")
            return False
        if not self._port.setDataBits(self._config.data_bits):
            self.error_occurred.emit(f"Failed to set data bits: {self._port.errorString()}")
            return False
        if not self._port.setParity(self._config.parity):
            self.error_occurred.emit(f"Failed to set parity: {self._port.errorString()}")
            return False
        if not self._port.setStopBits(self._config.stop_bits):
            self.error_occurred.emit(f"Failed to set stop bits: {self._port.errorString()}")
            return False
        if not self._port.setFlowControl(self._config.flow_control):
            self.error_occurred.emit(f"Failed to set flow control: {self._port.errorString()}")
            return False

        if not self._port.open(QIODevice.OpenModeFlag.ReadWrite):
            self.error_occurred.emit(
                f"Open {self._config.port_name} failed: {self._port.errorString()}"
            )
            return False

        self.port_opened.emit(self._config.port_name)
        return True

    def close_port(self) -> None:
        if self._port.isOpen():
            self._port.close()
            self.port_closed.emit()
        self._buffer.clear()

    def clear_buffer(self) -> None:
        self._buffer.clear()

    def send(self, data: bytes | bytearray | memoryview) -> bool:
        if not self._port.isOpen():
            self.error_occurred.emit("RS485 port is not open.")
            return False

        payload = bytes(data)
        written = self._port.write(payload)
        if written < 0:
            self.error_occurred.emit(self._port.errorString())
            return False
        return True

    def send_text(
        self,
        text: str,
        *,
        encoding: str | None = None,
        suffix: bytes = b"\r\n",
    ) -> bool:
        codec = encoding or self._config.text_encoding
        payload = text.encode(codec)
        if suffix and not payload.endswith(suffix):
            payload += suffix
        return self.send(payload)

    def send_hex(self, hex_string: str) -> bool:
        cleaned = hex_string.replace("0x", " ").replace("0X", " ")
        cleaned = cleaned.replace(",", " ").replace(";", " ")
        try:
            payload = bytes.fromhex(cleaned)
        except ValueError as exc:
            self.error_occurred.emit(f"Invalid hex payload: {exc}")
            return False
        return self.send(payload)

    def _on_ready_read(self) -> None:
        chunk = bytes(self._port.readAll())
        if not chunk:
            return

        self.bytes_received.emit(chunk)

        terminator = self._config.frame_terminator
        if terminator is None:
            self.frame_received.emit(chunk)
            self._emit_text(chunk)
            return

        self._buffer.extend(chunk)
        term_len = len(terminator)
        if term_len == 0:
            self.frame_received.emit(chunk)
            self._emit_text(chunk)
            return

        while True:
            index = self._buffer.find(terminator)
            if index < 0:
                break

            frame = bytes(self._buffer[:index])
            del self._buffer[: index + term_len]
            self.frame_received.emit(frame)
            self._emit_text(frame)

    def _emit_text(self, payload: bytes) -> None:
        try:
            text = payload.decode(self._config.text_encoding)
        except UnicodeDecodeError:
            return
        self.text_received.emit(text)

    def _on_error_occurred(self, error: QSerialPort.SerialPortError) -> None:
        if error == QSerialPort.NoError:
            return
        message = self._port.errorString()
        if message:
            self.error_occurred.emit(message)


__all__ = ["RS485Config", "RS485Manager", "RS485PortInfo"]
