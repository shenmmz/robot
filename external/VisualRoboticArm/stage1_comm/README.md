# Stage1 通讯框架

这是一个最小 Qt + Python 通讯测试框架，用来验证后续主项目中的进程隔离、生命周期管理和消息协议。

## 运行

```bash
python -m stage1_comm.main
```

Qt 主控会先启动本机 TCP 服务，然后通过 `QProcess` 拉起 Python 子进程。

## Python 单独运行

```bash
python -m stage1_comm.python_sub.main --host 127.0.0.1 --port 5001
```

## 目标

- Python 崩溃不影响 Qt 主控
- Qt 与 Python 可独立开发、独立调试
- QProcess 管理生命周期简单可靠
- 接口和消息格式保持稳定，便于后续迁移到真实项目

## 迁移约定

- Qt 侧保留 `LocalCommBridge` 和 `PythonProcessController` 的职责边界
- Python 侧保留 `hello / heartbeat / data / cmd / ack` 的消息类型
- 后续接入主项目时，只替换消息载荷，不改变连接方式和生命周期管理方式
