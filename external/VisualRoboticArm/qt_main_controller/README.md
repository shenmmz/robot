# VisualRoboticArm Qt 通讯骨架说明

此工程是一个最小 C++ Qt 主控骨架，只用于验证 Qt 主控与 Python 视觉子程序之间的进程隔离和 TCP/JSON 通讯。

它不负责板卡、RS485、ZMC、PWM 或电机控制。后续正式项目中，应只复用通讯协议、Python 生命周期管理和 pose 数据解析入口，电机控制逻辑由正式 C++ 主程序实现。

## 当前职责边界

- Qt 主控启动本地 TCP 服务，默认监听 `127.0.0.1:5001`。
- Qt 主控可通过 `QProcess` 启动 Python 视觉程序。
- Python 视觉程序连接回 Qt，并持续推送 `pose` 数据。
- Qt 收到数据后只显示、记录和验证协议，不下发任何电机命令。
- Python 崩溃或退出不会导致 Qt 主控崩溃。

## 目录结构

```text
qt_main_controller/
  CMakeLists.txt
  README.md
  src/
    main.cpp
    MainWindow.h/.cpp
    LocalCommBridge.h/.cpp
    PythonProcessController.h/.cpp
    JsonProtocol.h
```

核心文件说明：

- `JsonProtocol.h`：JSON 行协议的构造、编码、解码工具，可直接复制到正式项目。
- `LocalCommBridge.h/.cpp`：Qt 侧 TCP 服务端，只负责收发 JSON 消息。
- `PythonProcessController.h/.cpp`：用 `QProcess` 启停 Python 子进程，隔离 Python 崩溃风险。
- `MainWindow.h/.cpp`：当前测试 UI 和协议显示入口。正式项目可不复制 UI，只参考 `onMessageReceived()` 的 `pose` 处理方式。

## 电脑移植依赖

Qt 侧依赖：

- C++17 编译器
- CMake 3.21+
- Qt 6，组件：`Core`、`Widgets`、`Network`

Python 侧依赖：

- Python 3.x
- 当前视觉工程根目录中的 `requirements.txt`
- 视觉工程入口：`main.py`

Qt 工程已移除固定电脑路径。新电脑构建时请使用以下任一方式指定 Qt：

```powershell
cmake -S qt_main_controller -B qt_main_controller/build -DQt6_DIR="C:/Qt/6.x.x/msvc2022_64/lib/cmake/Qt6"
cmake --build qt_main_controller/build --config Debug
```

或：

```powershell
cmake -S qt_main_controller -B qt_main_controller/build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build qt_main_controller/build --config Debug
```

若使用 Qt Creator，在 Kit/CMake 配置中设置 `Qt6_DIR` 或 `CMAKE_PREFIX_PATH` 即可。

运行时如果 Qt 程序无法打开，使用 Qt 的部署工具补齐 DLL：

```powershell
windeployqt qt_main_controller/build/Debug/VisualRoboticArmQtController.exe
```

## Python 启动配置

Qt 界面中有以下启动参数：

- `Python 可执行文件`：可填 `python`，也可填绝对路径，例如 `C:/Python312/python.exe`。
- `Python 工作目录`：当前视觉项目根目录，例如 `E:/PythonProject/TestOnline/VisualRoboticArm`。
- `模块入口`：当前为 `main`，Qt 会执行 `python -u -m main`。
- `监听地址`：默认 `127.0.0.1`。
- `监听端口`：默认 `5001`。
- `客户端名`：默认 `qt-main`。

Qt 实际启动命令等价于：

```powershell
python -u -m main --host 127.0.0.1 --port 5001 --name qt-main
```

`PythonProcessController` 会把 `Python 工作目录` 加入 `PYTHONPATH`，因此 Python 中的 `app.*`、`stage1_comm.*` 可以正常导入。

## 通讯协议

传输方式：

- TCP
- 默认本机 `127.0.0.1:5001`
- 每条消息为一行 JSON，以 `\n` 结尾
- 编码为 UTF-8

通用消息格式：

```json
{
  "type": "pose",
  "seq": 12,
  "ts": 1781770594.85,
  "role": "python",
  "msg": "pose update",
  "payload": {}
}
```

通用字段说明：

- `type`：消息类型，当前主要使用 `hello`、`heartbeat`、`pose`、`cmd`、`ack`、`shutdown`。
- `seq`：发送端自增序号。
- `ts`：Unix 时间戳，单位秒。
- `role`：发送端身份，`python` 或 `qt`。
- `msg`：可选说明文本。
- `payload`：业务数据。

## 6 号电机验证协议

当前验证阶段只允许右手腕控制 `motor_id == 6`。Qt 端不要从左手数据、旧的 `angles` 文本或 UI 文本中推导电机动作，应优先读取 `payload.motor_6` 和 `payload.motor_commands`。

### 顶层字段

```json
{
  "payload": {
    "protocol_version": "vision_motor_v1",
    "control_stage": "right_wrist_motor_6_only",
    "pose_valid": true,
    "motion_allowed": true,
    "filter_state": "move",
    "filter_reason": "stable",
    "motor_6": {},
    "motor_commands": [],
    "summary": "6号电机：发送 | 右旋转 35°"
  }
}
```

- `protocol_version`：当前协议版本，固定为 `vision_motor_v1`。
- `control_stage`：当前控制阶段，固定为 `right_wrist_motor_6_only`。
- `pose_valid`：当前视觉结果是否有效。
- `motion_allowed`：顶层运动许可。当前阶段与 `motor_6.motion_allowed` 保持一致。
- `filter_state` / `filter_reason`：过滤状态及原因。
- `motor_6`：6 号电机状态对象。即使右手丢失，也会保留该对象用于显示“保持”和原因。
- `motor_commands`：正式可执行命令数组。当前阶段只有在右手有效、角度变化超过阈值、允许运动时才包含一条 `motor_id == 6` 的命令；保持状态下必须为空数组。
- `summary`：给 Qt 日志和状态栏使用的简洁字符串，格式固定为 `6号电机：发送/保持 | 方向 角度°`。

### motor_6 / motor_commands[0] 格式

```json
{
  "motor_id": 6,
  "joint": "wrist",
  "axis": "roll",
  "target": "right_wrist_roll",
  "state": "send",
  "valid": true,
  "motion_allowed": true,
  "direction": "left",
  "direction_text": "左旋转",
  "rotation_side": "inward",
  "angle_deg": 18.4,
  "signed_angle_deg": -18.4,
  "confidence": 0.72,
  "reason": "ok",
  "pwm_index": 6
}
```

字段说明：

- `motor_id`：固定为 `6`。
- `joint` / `axis`：固定为 `wrist` / `roll`，表示腕部左右旋转。
- `target`：固定为 `right_wrist_roll`。
- `state`：`send` 表示允许交给 Qt 正式项目；`hold` 表示保持，不下发。
- `valid`：是否识别到有效右手掌面。为 `false` 时禁止控制电机。
- `motion_allowed`：是否允许正式项目下发电机命令。为 `false` 时禁止控制电机。
- `direction`：建议电机方向，取值 `left` / `right` / `unknown`。
- `direction_text`：给 Qt 显示和日志用的中文文本，固定为 `左旋转` / `右旋转` / `保持`。
- `rotation_side`：右手腕相对身体的内外侧，`inward` 表示向内，`outward` 表示向外。
- `angle_deg`：给 Qt 和正式项目使用的最终角度值。
- `signed_angle_deg`：带符号角度，保留给调试或更细电机映射使用。
- `confidence`：0~1 的粗略可信度，仅用于调试观察。
- `reason`：状态原因。常见值：`ok`、`below_threshold`、`right_hand_missing`、`right_hand_missing_left_hand_detected`、`pose_missing`。
- `pwm_index`：固定为 `6`，给 Qt 正式项目中的 `Rs485Controller::setServoAngle()` 使用。

### Qt 端提取建议

```cpp
if (type == "pose") {
    const QJsonObject payload = message.value("payload").toObject();
    const QJsonArray commands = payload.value("motor_commands").toArray();
    if (commands.isEmpty()) {
        return; // 保持，不下发电机
    }

    const QJsonObject cmd = commands.first().toObject();
    if (cmd.value("motor_id").toInt() != 6 ||
        !cmd.value("valid").toBool(false) ||
        !cmd.value("motion_allowed").toBool(false)) {
        return;
    }

    const QString direction = cmd.value("direction").toString();
    const double angleDeg = cmd.value("angle_deg").toDouble();
    // 正式项目在这里做限位、死区、速度限制和板卡/ZMC 指令映射。
    // 例如：Rs485Controller::setServoAngle(6, angleDeg);
}
```

## 正式项目中的电机控制接入建议

当前 Qt 骨架不实现电机控制。正式项目接入时建议在 `MainWindow::onMessageReceived()` 或业务服务层中增加类似流程：

```cpp
if (type == "pose") {
    const QJsonObject payload = message.value("payload").toObject();
    if (!payload.value("motion_allowed").toBool(false)) {
        return; // 必须保持，不允许发电机命令
    }

    const QJsonArray angles = payload.value("angles").toArray();
    // 1. 选择 left/right 手臂
    // 2. 读取 wrist_rotation / arm_rotation / shoulder_rotation
    // 3. 做限幅、死区、速度约束、安全互锁
    // 4. 调用正式项目中的板卡/ZMC/RS485 控制模块
}
```

当前实验阶段可参考：

- `3号电机`：上下方向已在实验 UI 中停用。
- `6号电机`：可用 `wrist_rotation.horizontal_deg` 和 `horizontal` 做左右模拟映射。

正式控制前必须增加：

- 电机限位
- 急停状态判断
- 速度/加速度限制
- 指令频率限制
- 丢帧保持策略
- `motion_allowed == false` 时禁止运动

## 推荐移植文件

如果只想把通讯能力移植到正式 C++ Qt 项目，优先复制：

```text
src/JsonProtocol.h
src/LocalCommBridge.h
src/LocalCommBridge.cpp
src/PythonProcessController.h
src/PythonProcessController.cpp
```

`MainWindow.h/.cpp` 主要是测试 UI，可以只参考以下函数：

- `currentLaunchConfig()`
- `startPython()`
- `stopPython()`
- `onMessageReceived()`
- `formatPoseSummary()` 和相关 JSON 读取方式

## 当前不做的事情

- 不直接访问运动板卡。
- 不发送 RS485 或 Modbus 指令。
- 不实现电机映射闭环。
- 不替代正式项目中的安全控制。
- 不把 Python 视觉逻辑嵌入 Qt 进程内。

这个边界是有意保留的：当前框架只验证 Qt 与 Python 的稳定通讯，后续控制逻辑应在正式 C++ 主项目中实现。
