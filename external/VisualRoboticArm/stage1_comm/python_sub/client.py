from __future__ import annotations

import socket
import time
from dataclasses import dataclass
from typing import Any

from stage1_comm.protocol import build_message, decode_message, encode_message


@dataclass(slots=True)
class ClientConfig:
    host: str = "127.0.0.1"
    port: int = 5001
    name: str = "stage1-python"
    reconnect_delay: float = 1.0
    heartbeat_interval: float = 1.0
    data_interval: float = 2.0


class Stage1Client:
    def __init__(self, config: ClientConfig):
        self.config = config
        self._running = True
        self._seq = 0
        self._frame_id = 0

    def stop(self):
        self._running = False

    def run(self):
        print(
            f"[PY] 准备连接 {self.config.host}:{self.config.port} "
            f"(name={self.config.name})",
            flush=True,
        )
        while self._running:
            try:
                self._session()
            except KeyboardInterrupt:
                self._running = False
                break
            except Exception as exc:
                print(f"[PY][ERR] {exc}", flush=True)
            if self._running:
                time.sleep(self.config.reconnect_delay)
        print("[PY] 退出", flush=True)

    def _next_seq(self) -> int:
        self._seq += 1
        return self._seq

    def _send(self, sock: socket.socket, message_type: str, payload: dict[str, Any] | None = None, msg: str | None = None, **extra: Any):
        message = build_message(message_type, self._next_seq(), "python", payload=payload, msg=msg, **extra)
        sock.sendall(encode_message(message))
        return message

    def _connect(self) -> socket.socket:
        sock = socket.create_connection((self.config.host, self.config.port), timeout=3.0)
        sock.settimeout(0.1)
        print("[PY] 已连接到 Qt 主控", flush=True)
        return sock

    def _session(self):
        sock = self._connect()
        buffer = bytearray()
        last_heartbeat = 0.0
        last_data = 0.0
        self._send(sock, "hello", payload={"name": self.config.name, "role": "python", "ver": 1}, msg="hello")
        while self._running:
            now = time.monotonic()
            if now - last_heartbeat >= self.config.heartbeat_interval:
                self._send(sock, "heartbeat", payload={"alive": True, "name": self.config.name})
                last_heartbeat = now
            if now - last_data >= self.config.data_interval:
                self._frame_id += 1
                self._send(
                    sock,
                    "data",
                    payload={
                        "frame_id": self._frame_id,
                        "status": "ok",
                        "text": "stage1 demo data",
                        "confidence": 1.0,
                    },
                )
                last_data = now
            try:
                chunk = sock.recv(4096)
                if not chunk:
                    raise ConnectionError("peer closed")
                buffer.extend(chunk)
                while True:
                    try:
                        newline_index = buffer.index(0x0A)
                    except ValueError:
                        break
                    raw = bytes(buffer[:newline_index])
                    del buffer[: newline_index + 1]
                    text = raw.decode("utf-8", errors="replace").strip()
                    if not text:
                        continue
                    self._handle_frame(sock, text)
            except socket.timeout:
                continue
            except OSError as exc:
                raise ConnectionError(str(exc)) from exc
        try:
            sock.close()
        except Exception:
            pass

    def _handle_frame(self, sock: socket.socket, text: str):
        message = decode_message(text)
        print(f"[PY][RX] {text}", flush=True)

        message_type = message.get("type")
        if message_type == "cmd":
            payload = message.get("payload") or {}
            command = payload.get("text") or payload.get("name") or "unknown"
            reply = build_message(
                "ack",
                self._next_seq(),
                "python",
                payload={"cmd": command, "ok": True},
                msg=f"ack {command}",
            )
            sock.sendall(encode_message(reply))
        elif message_type == "shutdown":
            self.stop()
        elif message_type == "ack":
            pass

