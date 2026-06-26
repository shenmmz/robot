from __future__ import annotations

import sys
from pathlib import Path

from PySide6.QtCore import QObject, QProcess, Signal


class PythonProcessController(QObject):
    started = Signal(str)
    finished = Signal(int)
    crashed = Signal(int)
    stdoutLine = Signal(str)
    stderrLine = Signal(str)
    errorOccurred = Signal(str)

    def __init__(self, parent: QObject | None = None):
        super().__init__(parent)
        self._process = QProcess(self)
        self._process.setProcessChannelMode(QProcess.ProcessChannelMode.SeparateChannels)
        self._process.started.connect(self._on_started)
        self._process.finished.connect(self._on_finished)
        self._process.errorOccurred.connect(self._on_error)
        self._process.readyReadStandardOutput.connect(self._on_stdout_ready)
        self._process.readyReadStandardError.connect(self._on_stderr_ready)
        self._script_module = "stage1_comm.python_sub.main"
        self._working_dir = Path(__file__).resolve().parent.parent

    def is_running(self) -> bool:
        return self._process.state() != QProcess.ProcessState.NotRunning

    def start(self, host: str, port: int, name: str = "stage1-python") -> bool:
        if self.is_running():
            return True

        arguments = [
            "-u",
            "-m",
            self._script_module,
            "--host",
            host,
            "--port",
            str(port),
            "--name",
            name,
        ]
        self._process.setProgram(sys.executable)
        self._process.setArguments(arguments)
        self._process.setWorkingDirectory(str(self._working_dir))
        self._process.start()
        if not self._process.waitForStarted(3000):
            self.errorOccurred.emit(f"Python 进程启动失败: {self._process.errorString()}")
            return False
        return True

    def stop(self):
        if not self.is_running():
            return
        self._process.terminate()
        if not self._process.waitForFinished(1500):
            self._process.kill()
            self._process.waitForFinished(1500)

    def restart(self, host: str, port: int, name: str = "stage1-python") -> bool:
        self.stop()
        return self.start(host, port, name=name)

    def _drain_channel(self, reader) -> list[str]:
        text = bytes(reader()).decode("utf-8", errors="replace")
        return [line for line in text.splitlines() if line.strip()]

    def _on_started(self):
        self.started.emit("Python 进程已启动")

    def _on_finished(self, exit_code: int, exit_status: QProcess.ExitStatus):
        if exit_status == QProcess.ExitStatus.CrashExit or exit_code != 0:
            self.crashed.emit(exit_code)
        self.finished.emit(exit_code)

    def _on_error(self, error: QProcess.ProcessError):
        messages = {
            QProcess.ProcessError.FailedToStart: "Python 进程启动失败",
            QProcess.ProcessError.Crashed: "Python 进程崩溃",
            QProcess.ProcessError.Timedout: "Python 进程超时",
            QProcess.ProcessError.WriteError: "Python 进程写入失败",
            QProcess.ProcessError.ReadError: "Python 进程读取失败",
            QProcess.ProcessError.UnknownError: "Python 进程未知错误",
        }
        fallback = f"Python 进程错误: {getattr(error, 'name', str(error))}"
        self.errorOccurred.emit(messages.get(error, fallback))

    def _on_stdout_ready(self):
        for line in self._drain_channel(self._process.readAllStandardOutput):
            self.stdoutLine.emit(line)

    def _on_stderr_ready(self):
        for line in self._drain_channel(self._process.readAllStandardError):
            self.stderrLine.emit(line)
