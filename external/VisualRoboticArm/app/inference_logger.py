# -*- coding: utf-8 -*-
from __future__ import annotations

from datetime import datetime
from pathlib import Path


class InferenceLogSession:
    """Collect one image-capture session's inference records and save them on stop."""

    def __init__(self, logs_root: Path | str | None = None):
        project_root = Path(__file__).resolve().parents[1]
        self._logs_root = Path(logs_root) if logs_root is not None else project_root / "Logs"
        self._started_at: datetime | None = None
        self._records: list[str] = []

    @property
    def is_active(self) -> bool:
        return self._started_at is not None

    def start(self) -> None:
        self._started_at = datetime.now()
        self._records = []

    def append_result(self, summary: str) -> None:
        if not self.is_active:
            return
        stamp = _format_record_time(datetime.now())
        result = summary.strip() or "无推理结果"
        self._records.append(f"{stamp}\n{result}")

    def save(self) -> Path | None:
        if self._started_at is None:
            return None

        day_dir = (
            self._logs_root
            / self._started_at.strftime("%y")
            / self._started_at.strftime("%m")
            / self._started_at.strftime("%d")
        )
        day_dir.mkdir(parents=True, exist_ok=True)
        file_path = day_dir / self._started_at.strftime("Log-%y%m%d-%H%M%S.txt")
        content = "\n\n".join(self._records)
        if content:
            content += "\n"
        file_path.write_text(content, encoding="utf-8")

        self._started_at = None
        self._records = []
        return file_path


def _format_record_time(moment: datetime) -> str:
    centiseconds = moment.microsecond // 10000
    return f"{moment:%Y-%m%d-%H：%M：%S}：{centiseconds:02d}"
