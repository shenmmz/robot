from __future__ import annotations

import json
import time
from typing import Any


def build_message(
    message_type: str,
    seq: int,
    role: str,
    payload: dict[str, Any] | None = None,
    msg: str | None = None,
    **extra: Any,
) -> dict[str, Any]:
    message: dict[str, Any] = {
        "type": message_type,
        "seq": int(seq),
        "ts": time.time(),
        "role": role,
    }
    if payload is not None:
        message["payload"] = payload
    if msg is not None:
        message["msg"] = msg
    if extra:
        message.update(extra)
    return message


def encode_message(message: dict[str, Any]) -> bytes:
    text = json.dumps(message, ensure_ascii=False, separators=(",", ":"))
    return (text + "\n").encode("utf-8")


def decode_message(text: str) -> dict[str, Any]:
    obj = json.loads(text)
    if not isinstance(obj, dict):
        raise ValueError("JSON message must be an object")
    return obj
