# 六轴机械臂项目框架详细文档

---

## 📋 文档说明

本文档详细描述六轴机械臂项目的整体架构、模块划分、核心组件、数据流程和接口定义。

---

## 🏗️ 整体架构

### 分层架构设计

项目采用经典的五层分层架构，各层职责清晰，低耦合高内聚：

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        第1层: UI层 (UI Layer)                          │
│  ┌───────────────────────────────────────────────────────────────────┐  │
│  │  MainWindow                                                     │  │
│  │  - 用户界面展示、按钮、滑块、图表等                              │  │
│  │  - 事件处理、用户交互响应                                       │  │
│  └───────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                   第2层: 业务逻辑层 (Logic Layer)                      │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │   RobotController       │  │         TaskExecutor               │  │
│  │   - 核心业务协调        │  │   - 脚本解析与执行                  │  │
│  │   - 运动控制管理        │  │   - 循环/分支控制                  │  │
│  │   - 状态管理            │  │   - 变量管理                      │  │
│  └─────────────────────────┘  └─────────────────────────────────────┘  │
│                                        │                             │
│                              ┌─────────┴─────────┐                   │
│                              ▼                   ▼                   │
│                   ┌─────────────────┐  ┌─────────────────┐           │
│                   │   UiController  │  │  HomingController│          │
│                   │   - UI逻辑处理  │  │  - 回原点逻辑   │           │
│                   └─────────────────┘  └─────────────────┘           │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    第3层: 数据模型层 (Model Layer)                      │
│  ┌─────────────────────────────────────┐  ┌─────────────────────────┐  │
│  │         RobotState                  │  │       RobotConfig       │  │
│  │  - 实时状态: 位置、速度、状态码     │  │  - 配置参数: 速度、限位 │  │
│  │  - 错误信息、笛卡尔坐标            │  │  - 轴参数、IP地址      │  │
│  └─────────────────────────────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                   第4层: 硬件驱动层 (Hardware Layer)                   │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │    MotionController     │  │      ServoController                │  │
│  │  - 硬件连接管理         │  │  - 6路舵机角度控制（速度控制）      │  │
│  │  - 状态更新循环         │  │  - 平滑转动（定时器50ms）           │  │
│  │  - 正逆解模式切换       │  │  - 批量/单路发送                    │  │
│  └─────────────────────────┘  └──────────────┬──────────────────────┘  │
│                                              │                         │
│  ┌───────────────────────────────────────────▼───────────────────────┐  │
│  │                    Rs485Controller                                │  │
│  │  - Modbus RTU通信（通过ZMC控制器转发）                            │  │
│  │  - 单路/批量PWM协议                                              │  │
│  │  - 角度→占空比自动换算                                            │  │
│  └───────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                   第5层: 硬件API层 (API Layer)                         │
│  ┌───────────────────────────────────────────────────────────────────┐  │
│  │                      ZmotionAPI                                   │  │
│  │  - 底层通信协议处理                                               │  │
│  │  - 运动指令封装                                                   │  │
│  │  - IO控制、状态读取                                               │  │
│  └───────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
                       ┌──────────────────┐
                       │   ZMC306X控制器   │
                       │ (IP:192.168.0.123)│
                       └────────┬─────────┘
                                │
              ┌─────────────────┼─────────────────┐
              ▼                                   ▼
   ┌──────────────────┐                ┌──────────────────┐
   │  6轴步进/伺服电机 │                │ QD-6PWM-V2.0模块 │
   │  (正运动机械臂)   │                │ (RS485 Modbus)   │
   └──────────────────┘                │ 从站地址: 0x21   │
                                       └────────┬─────────┘
                                                ▼
                                       ┌──────────────────┐
                                       │  6路KPZ32舵机    │
                                       │ PWM1~PWM6        │
                                       └──────────────────┘
```

### 层间调用规则

| 调用方 | 被调用方 | 调用方式 |
|--------|----------|----------|
| UI层 | 业务逻辑层 | 直接调用方法 + 信号槽 |
| UI层 (ServoTestWidget) | 硬件驱动层 (ServoController) | 直接调用方法 + 信号槽 |
| 业务逻辑层 | 数据模型层 | 直接访问 (getter/setter) |
| 业务逻辑层 | 硬件驱动层 | 直接调用方法 + 信号槽 |
| 硬件驱动层 (ServoController) | 硬件驱动层 (Rs485Controller) | 直接调用方法 |
| 硬件驱动层 (MotionController) | 硬件API层 (ZmotionAPI) | 直接调用方法 |
| 硬件驱动层 (Rs485Controller) | 硬件API层 (ZmotionAPI) | 直接调用方法 |
| 硬件API层 | 硬件设备 | 网络通信 (TCP/IP) |

**禁止规则**：
- ❌ UI层不得直接访问硬件API层
- ❌ UI层不得直接修改RobotState
- ❌ 业务逻辑层不得直接访问硬件设备

---

## 📁 项目目录结构

```
Robot_R06/                              # 项目根目录
├── src/                                # 源代码目录
│   ├── logic/                          # 🧠 业务逻辑层
│   │   ├── RobotController.h/cpp       # 核心控制器
│   │   ├── TaskExecutor.h/cpp          # 脚本执行器
│   │   ├── UiController.h/cpp          # UI逻辑控制器
│   │   ├── HomingController.h/cpp      # 回原点控制器
│   │   └── ScriptCommand.h             # 脚本指令定义
│   │
│   ├── models/                         # 📊 数据模型层
│   │   ├── RobotState.h/cpp            # 实时状态模型
│   │   ├── RobotConfig.h/cpp           # 配置模型
│   │   └── RobotKinematics.h/cpp       # 运动学计算
│   │
│   ├── hardware/                       # 🔧 硬件驱动层
│   │   ├── MotionController.h/cpp      # 运动控制器（ZMC306X连接管理）
│   │   ├── ServoController.h/cpp       # 舵机控制器（6路PWM舵机，速度控制）
│   │   └── Rs485Controller.h/cpp       # RS485 Modbus控制器（PWM模块通信）
│   │
│   ├── ui/                             # 🖥️ 独立UI组件
│   │   └── ServoTestWidget.h/cpp       # 舵机测试界面（滑块+角度控制）
│   │
│   └── utils/                          # 🛠️ 工具层
│       ├── Logger.h/cpp                # 日志系统
│       └── MathUtils.h                 # 数学工具函数
│
├── ZmotionAPI/                         # 🔌 硬件API封装
│   ├── ZmotionAPI.h/cpp               # Zmotion控制器API
│   └── CMakeLists.txt
│
├── library/                            # 📚 外部依赖库
│   ├── zmotion.h                       # 原厂运动控制库头文件
│   ├── zauxdll2.h                      # 原厂辅助库头文件
│   └── RobotConstants.h                # 常量定义
│
├── config/                             # ⚙️ 配置文件
│   └── robot_config.json               # 机器人配置
│
├── task/                               # 📝 脚本任务文件
│   ├── 1.rbt                           # 示例脚本1
│   ├── test.rbt                        # 测试脚本
│   └── ...
│
├── docs/                               # 📖 文档
│   ├── R06脚本指令参考.md               # 脚本指令文档
│   └── 回原点逻辑说明.md               # 回原点说明
│
├── main.cpp                            # 程序入口
├── mainwindow.h/cpp/ui                 # 主窗口UI
├── CMakeLists.txt                      # CMake配置
└── README.md                           # 项目说明
```

---

## 🧠 业务逻辑层详解

### 1. RobotController（核心控制器）

**职责**：
- 协调各模块工作
- 提供高级API供UI调用
- 状态管理与错误处理
- 信号转发

**核心方法详解**（对应 `RobotController.cpp`）：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `initialize(configFile)` | 从JSON配置文件初始化控制器 | `configFile`: 配置文件路径 | `bool`: 是否成功 |
| `initialize(config)` | 使用已有的RobotConfig对象初始化 | `config`: RobotConfig指针 | `bool`: 是否成功 |
| `connectHardware(ipAddr)` | 与机械臂建立TCP/IP连接 | `ipAddr`: 控制器IP地址 | `bool`: 是否成功 |
| `disconnectHardware()` | 断开硬件连接 | 无 | `bool`: 是否成功 |
| `isConnected()` | 查询连接状态 | 无 | `bool`: 连接状态 |
| `searchControllers()` | 搜索局域网内的控制器 | 无 | `QStringList`: IP列表 |
| `initRobotModel()` | 初始化六轴DH模型到控制器 | 无 | `bool`: 是否成功 |
| `executeHoming()` | 执行全轴回零操作 | 无 | `bool`: 是否成功 |
| `stopHoming()` | 停止回零操作 | 无 | 无 |
| `executeSafeStop()` | 急停：快速停止所有轴 | 无 | `bool`: 是否成功 |
| `moveAxis(axis, pos, speed, accel, decel)` | 单轴绝对位置运动 | 轴索引/目标/速度/加速度/减速度 | `bool`: 是否成功 |
| `moveJ(jointPos, speed, accel, decel, timeout)` | 关节空间多轴直线插补运动 | 6轴目标角度列表 | `bool`: 是否成功 |
| `jogJoint(axis, angle, speed, accel, decel)` | 关节点动（不等待到位） | 轴索引/目标角度 | `bool`: 是否成功 |
| `moveL(cartPos, speed, accel, decel, timeout)` | 笛卡尔直线运动（逆解） | 6维笛卡尔坐标 [X,Y,Z,Rx,Ry,Rz] | `bool`: 是否成功 |
| `jogCart(cartTarget, speed, accel, decel)` | 笛卡尔点动（不等待到位） | 笛卡尔目标坐标 | `bool`: 是否成功 |
| `switchToCartesianMode()` | 切换到逆解模式 | 无 | `bool`: 是否成功 |
| `switchToJointMode()` | 切换到正解模式 | 无 | `bool`: 是否成功 |
| `isCartesianMode()` | 查询是否为笛卡尔模式 | 无 | `bool`: 模式状态 |
| `stopMotion()` | 停止所有轴运动 | 无 | `bool`: 是否成功 |
| `getState()` | 获取RobotState实例 | 无 | `RobotState*` |
| `getConfig()` | 获取RobotConfig实例 | 无 | `RobotConfig*` |
| `getMotionController()` | 获取MotionController实例 | 无 | `MotionController*` |
| `getHomingController()` | 获取HomingController实例 | 无 | `HomingController*` |

**关键实现细节**：

#### `initialize()` - 初始化流程
```cpp
// 1. 创建RobotState（轴数从配置读取）
m_state = new RobotState(m_config->getAxisCount(), this);

// 2. 创建MotionController（关联配置和状态）
m_motionController = new MotionController(m_config, m_state, this);

// 3. 创建HomingController（关联API、配置、状态）
m_homingController = new HomingController(
    m_motionController->getZmotionAPI(),
    m_config, m_state, this);

// 4. 连接所有信号槽
connectSignals();

// 5. 发射initialized()信号
emit initialized();
```

#### `initRobotModel()` - DH模型初始化（核心）
```cpp
// 1. 物理关节轴配置（轴0-5）
for (每轴) {
    api->setAtype(axis, 1);        // 设为旋转轴
    api->setAxisUnits({axis}, {...}); // 设置脉冲当量
    api->setAxisDpos({axis}, {0.0});  // 初始位置清零
    api->setAxisAccel({axis}, {100.0});
    api->setAxisDecel({axis}, {100.0});
}

// 2. 生成25元素DH参数TABLE
RobotKinematics kinematics;
QVector<float> tableData = kinematics.toControllerTable();
api->setTable(0, 25, tableData);  // 写入TABLE[0-24]

// 3. 虚拟笛卡尔轴配置（轴6-11）
for (每虚拟轴) {
    api->setAtype(virtualAxis, 0); // 设为虚拟轴
    api->setAxisUnits({axis}, {10000.0});
    // ...
}

// 4. CONNFRAME关联（通过控制器自动进行正逆解）
// 正解模式：关节轴(0-5) → 虚拟轴(6-11)
// 逆解模式：虚拟轴(6-11) → 关节轴(0-5)
```

#### `moveJ()` - 关节空间运动
```cpp
// 1. 检查连接和轴数匹配
if (!isConnected()) return false;
if (jointPos.size() != m_config->getAxisCount()) return false;

// 2. 确保正解模式
switchToJointMode();

// 3. 获取关节轴列表并发送运动命令
QList<int> axes = m_config->getJointAxisList(); // [0,1,2,3,4,5]
api->directMoveAbsolute(axes, targets, speed, accel, decel);

// 4. 等待运动到位（对比DPOS与目标位置）
api->waitForAxesReachTarget(axes, targets, timeoutMs);

// 5. 更新状态
m_state->setStatus(RobotState::StatusIdle);
```

#### `moveL()` - 笛卡尔直线运动
```cpp
// 1. 检查笛卡尔坐标格式（必须6个值）
if (cartPos.size() != 6) return false;

// 2. 确保逆解模式
switchToCartesianMode();

// 3. 获取当前虚拟轴位置（做角度归一化）
QList<int> virtualAxes = m_config->getVirtualAxisList(); // [6,7,8,9,10,11]
for (i = 3 to 5) { // Rx,Ry,Rz 做 ±180° 归一化
    double delta = cartPos[i] - normalizeAngle180(curRaw[i]);
    targets[i] = curRaw[i] + delta;
}

// 4. 发送笛卡尔运动命令（指令虚拟轴）
api->directMoveAbsolute(virtualAxes, targets, speed, accel, decel);

// 5. 等待到位并更新状态
api->waitForAxesReachTarget(virtualAxes, targets, timeoutMs);
m_state->setStatus(RobotState::StatusIdle);
```

**信号定义**：

```cpp
signals:
    void initialized();                    // 初始化完成
    void initializationFailed(QString);   // 初始化失败
    void hardwareConnected();              // 硬件连接成功
    void hardwareDisconnected();          // 硬件断开
    void hardwareConnectionFailed(QString);// 连接失败
    void modelInitSuccess();              // DH模型初始化成功
    void modelInitFailed(int);            // DH模型初始化失败
    void operationStarted(QString);       // 操作开始
    void operationCompleted(QString);     // 操作完成
    void operationFailed(QString, QString);// 操作失败
    void robotStatusChanged(QString);     // 状态字符串变化
    void errorOccurred(int, QString);     // 错误发生
```

---

### 2. TaskExecutor（脚本执行器）

**职责**：
- 解析脚本指令
- 管理指令队列
- 执行控制流（循环、分支）
- 变量管理

**核心方法详解**（对应 `TaskExecutor.cpp`）：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setCommands(commands)` | 设置指令列表并预处理循环映射 | `commands`: ScriptCommand列表 | 无 |
| `start()` | 开始执行任务 | 无 | 无 |
| `pause()` | 暂停执行（停止运动并等待轴停止） | 无 | 无 |
| `resume()` | 恢复执行（根据下一指令类型设置运动学模式） | 无 | 无 |
| `stop()` | 强制停止（清空队列并停止运动） | 无 | 无 |
| `pauseOrResumeTask()` | 切换暂停/恢复状态 | 无 | 无 |
| `isTaskRunning()` | 查询是否运行中 | 无 | `bool` |
| `executeNext()` | 递归执行下一条指令（核心循环） | 无 | 无 |
| `executeCommand(cmd)` | 执行单条指令（switch分支） | `cmd`: ScriptCommand | `bool` |
| `buildLoopMap()` | 预处理FOR/END_FOR/WHILE/END_WHILE的跳转表 | 无 | 无 |
| `resolveValue(raw, vars)` | 解析变量或常量值 | 原始字符串/变量表 | `double` |
| `evaluateExpr(expr, vars)` | 表达式求值（支持±*/） | 表达式/变量表 | `double` |

**关键实现细节**：

#### `start()` - 任务启动流程
```cpp
// 1. 前置检查
if (m_commands.isEmpty()) {
    emit taskError("指令列表为空"); return;
}
if (!m_robotController->isConnected()) {
    emit taskError("控制器未连接"); return;
}

// 2. 重置状态
m_currentIdx = 0;
m_state = Running;
emit taskStarted(m_commands.size());

// 3. 开始递归执行
executeNext();
```

#### `executeNext()` - 指令执行主循环
```cpp
void TaskExecutor::executeNext() {
    // 1. 状态检查（非Running则停止）
    if (m_state != Running) return;

    // 2. 检查是否执行完毕
    if (m_currentIdx >= m_commands.size()) {
        m_state = Idle;
        emit taskFinished();  // 发出完成信号
        return;
    }

    // 3. 发出进度信号
    const ScriptCommand& cmd = m_commands[m_currentIdx];
    emit taskProgress(m_currentIdx + 1, m_commands.size());

    // 4. 处理控制流指令（不消耗进度）
    switch (cmd.type) {
        case ForLoop:    handleForLoop(cmd); break;
        case EndFor:     handleEndFor(cmd); break;
        case IfDI:       handleIfDI(cmd); break;
        case Else:       handleElse(cmd); break;
        case EndIf:      handleEndIf(cmd); break;
        case WhileDI:    handleWhileDI(cmd); break;
        case WhileTrue:  handleWhileTrue(cmd); break;
        case EndWhile:   handleEndWhile(cmd); break;
        default:
            // 5. 执行普通指令
            if (executeCommand(cmd)) {
                m_currentIdx++;
            } else {
                emit taskError("指令执行失败: " + cmd.sourceLine);
                return;
            }
    }

    // 6. 递归调用执行下一条（异步）
    QTimer::singleShot(0, this, &TaskExecutor::executeNext);
}
```

#### `executeCommand(cmd)` - 指令执行核心
```cpp
bool TaskExecutor::executeCommand(const ScriptCommand& cmd) {
    switch (cmd.type) {
    case ScriptCmdType::MoveJ:
        return m_robotController->moveJ(cmd.targets, speed, accel, decel, timeout);

    case ScriptCmdType::MoveL:
        return m_robotController->moveL(cmd.targets, speed, accel, decel, timeout);

    case ScriptCmdType::MoveOff: {
        // 获取当前关节角度，加上偏移量
        QList<double> cur = m_robotController->getAllAxisPositions();
        for (i) cur[i] += cmd.targets[i];
        return m_robotController->moveJ(cur, speed, accel, decel, timeout);
    }

    case ScriptCmdType::WaitIdle:
        return api->waitForAxesIdle({0,1,2,3,4,5}, cmd.timeoutMs).success;

    case ScriptCmdType::Delay:
        QThread::msleep(cmd.delayMs);
        return true;

    case ScriptCmdType::SetDO:
        return api->setDO(cmd.ioIndex, cmd.ioValue).success;

    case ScriptCmdType::WaitDI:
        return api->waitDI(cmd.ioIndex, cmd.ioValue, cmd.ioTimeout).success;

    case ScriptCmdType::Print:
        emit consoleOutput(cmd.printText);
        return true;

    case ScriptCmdType::ReadPos: {
        // 读取并发送当前位置到控制台
        auto joints = m_robotController->getAllAxisPositions();
        emit consoleOutput(QString("J: %1 %2 %3 %4 %5 %6").arg(...));
        return true;
    }

    case ScriptCmdType::SetVar:
        // 赋值：支持变量表达式如 "x + 10"
        m_variables[cmd.varName] = evaluateExpr(cmd.varExpr, m_variables);
        return true;

    case ScriptCmdType::SpeedSet:
        m_globalSpeed = cmd.targets[0];  // 设置后续默认速度
        return true;

    // ... 其他指令类型
    }
}
```

#### `buildLoopMap()` - 循环跳转表构建
```cpp
// 使用栈结构处理嵌套循环
m_loopEndMap.clear();   // 入口 → 结束位置
m_loopStartMap.clear(); // 结束位置 → 入口

for (int i = 0; i < m_commands.size(); ++i) {
    switch (m_commands[i].type) {
    case ScriptCmdType::ForLoop:
        forStack.push(i);  // 记录FOR入口位置
        break;
    case ScriptCmdType::EndFor:
        int head = forStack.pop();
        m_loopEndMap[head] = i;   // FOR → END_FOR
        m_loopStartMap[i] = head; // END_FOR → FOR
        break;
    // IF_DI/END_IF 类似处理（支持嵌套）
    }
}
```

**支持的指令类型**：

```cpp
enum class ScriptCmdType {
    MoveJ,       // 关节空间运动
    MoveL,       // 笛卡尔直线运动
    WaitIdle,    // 等待所有轴停止
    Delay,       // 延时
    SetDO,       // 设置数字输出
    WaitDI,      // 等待数字输入
    Print,       // 打印输出
    ReadPos,     // 读取位置
    Call,        // 调用子脚本
    SetVar,      // 变量赋值
    ForLoop,     // FOR循环
    EndFor,      // FOR循环结束
    WhileDI,     // WHILE条件循环
    WhileTrue,   // WHILE TRUE循环
    EndWhile,    // WHILE循环结束
    SpeedSet,    // 设置全局速度
    AccelSet,    // 设置全局加速度
    DecelSet,    // 设置全局减速度
    TimeoutSet,  // 设置全局超时
    MoveOff,     // 关节相对偏移
    MoveLOff,    // 笛卡尔相对偏移
    IfDI,        // IF条件判断
    Else,        // ELSE分支
    EndIf,       // END_IF结束
    TargetDef,   // 点位定义
};
```

**TaskExecutor 信号定义**：

```cpp
signals:
    void taskStarted(int total);                // 任务开始
    void taskProgress(int current, int total);  // 进度更新
    void taskPaused(int current, int total);    // 暂停
    void taskResumed(int current, int total);  // 恢复
    void taskFinished();                        // 完成
    void taskError(QString msg);               // 错误
    void consoleOutput(QString text);           // 控制台输出
```

---

### 3. UiController（UI逻辑控制器）

**职责**：
- 处理UI事件回调
- 管理编辑器状态
- 同步点位定义
- 脚本文件管理

**核心方法详解**（对应 `UiController.cpp`）：

| 方法名 | 功能说明 | 关键实现 |
|--------|----------|----------|
| `init()` | 初始化UI连接 | 启动IO刷新定时器、连接TaskExecutor信号、连接所有按钮 |
| `refreshIO()` | 刷新DI/DO指示灯状态 | 读取20个DI/DO状态并更新LED颜色 |
| `onRobotError(code, msg)` | 机械臂错误回调 | 添加到报警日志表 |
| `onHardwareError(code, msg)` | 硬件错误回调 | 添加到报警日志表 |
| `addAlarmRow(time, level, code, msg)` | 添加报警行 | 更新alarmTableWidget |
| `onClearAlarms()` | 清除所有报警 | 清空报警表并清除RobotState错误 |
| `onProgRun()` | 运行脚本按钮回调 | syncTargets → 合并脚本 → runText |
| `runText(text, sourceLine)` | 解析并执行脚本 | parseScript → TaskExecutor::start |
| `parseScript(text)` | 脚本词法/语法解析 | 逐行解析TARGET/MOVEJ/MOVEL等指令 |
| `syncTargets()` | 同步点位顺序 | 从程序编辑器提取TARGET定义并更新点位编辑器 |
| `onProgStop()` | 停止脚本执行 | TaskExecutor::stop → 更新按钮文本 |
| `onProgSave()` | 保存脚本到文件 | 合并点位区+程序区 → 写入.rbt文件 |
| `onProgLoad()` | 加载脚本文件 | 读取.rbt文件 → 分割TARGET区和程序区 → 显示 |
| `onProgNew()` | 新建脚本 | 清空两个编辑器、点位缓存、当前路径 |
| `insertAtCursor(editor, text)` | 在编辑器光标处插入文本 | 获取QTextCursor并insertText |
| `appendConsole(text)` | 追加文本到控制台 | 在控制台末尾追加带时间戳的文本 |
| `readDefaultMotionParams(...)` | 读取默认运动参数 | 从debugSpeed/debugAccel等控件读取 |
| `onProgInsertMoveJ()` | 插入MOVEJ按钮 | 读取当前关节角度 → 生成TARGET → 插入MOVEJ |
| `onProgInsertMoveL()` | 插入MOVEL按钮 | 读取当前笛卡尔坐标 → 生成TARGET → 插入MOVEL |
| `onProgInsertWaitIdle()` | 插入WAIT_IDLE | 直接插入"WAIT_IDLE()" |
| `onProgInsertDelay()` | 插入DELAY | 弹窗输入毫秒 → 插入"DELAY(ms)" |
| `onProgInsertSetDO()` | 插入SET_DO | 弹窗输入IO编号和值 → 插入"SET_DO(idx,val)" |
| `onProgInsertWaitDI()` | 插入WAIT_DI | 弹窗输入IO编号/期望值/超时 → 插入 |
| `onProgInsertPrint()` | 插入PRINT | 弹窗输入文本 → 插入'PRINT("text")' |
| `onProgInsertComment()` | 插入注释 | 插入"# "并跳到行尾 |
| `onProgInsertSet()` | 插入变量赋值 | 弹窗输入变量名和值 → 插入"var = val" |
| `onProgInsertCall()` | 插入CALL | 文件选择对话框 → 插入'CALL("file.rbt")' |
| `onProgReadPosition()` | 读取当前位置 | 插入READ_POS指令并立即打印到控制台 |
| `onProgInsertFor()` | 插入FOR循环 | 弹窗输入变量/起始/结束 → 插入FOR/END_FOR |
| `onProgInsertMoveOff()` | 插入MOVEJOFF | 弹窗输入6轴偏移 → 插入"MOVEJOFF(dJ0,...)" |
| `onProgInsertMoveLOff()` | 插入MOVELOFF | 弹窗输入6轴偏移 → 插入"MOVELOFF(dX,...)" |
| `onProgInsertSpeed/Accel/Decel/Timeout()` | 插入参数设定 | 弹窗输入值 → 插入SPEED/ACCEL/DECEL/TIMEOUT |
| `onProgInsertIfDI/Else/EndIf()` | 插入条件分支 | 弹窗输入条件 → 插入IF_DI/ELSE/END_IF |
| `onJogJoint(axis, positive)` | 关节点动 | 读取步长 → 计算目标角度 → jogJoint移动 |
| `onJogCart(index, positive)` | 笛卡尔点动 | 读取步长 → 计算笛卡尔偏移 → jogCart移动 |
| `onDebugModeToggled(checked)` | 调试模式开关 | 启用/禁用所有手动控件 |
| `onLoadSettings()` | 加载设置 | 从RobotConfig读取参数显示到UI |
| `onSaveSettings()` | 保存设置 | 从UI读取参数写入RobotConfig并保存JSON |
| `onTaskProgress(current, total)` | 任务进度回调 | 更新状态栏 → updateState → emit jogCompleted |
| `onTaskPaused(current, total)` | 任务暂停回调 | 弹窗选择继续/结束任务 |
| `onTaskResumed(current, total)` | 任务恢复回调 | 更新状态栏显示 |
| `onTaskFinished()` | 任务完成回调 | 恢复按钮文本 → emit jogCompleted → 打印完成 |
| `onTaskError(msg)` | 任务错误回调 | 恢复按钮文本 → 打印错误到控制台/状态栏 |

**关键实现细节**：

#### `init()` - UI连接初始化
```cpp
void UiController::init() {
    // 1. 保存centralwidget用于后续子控件查找
    m_centralWidget = m_mainWindow->findChild<QWidget*>("centralwidget");

    // 2. 启动IO刷新定时器（200ms）
    m_ioTimer = new QTimer(this);
    connect(m_ioTimer, &QTimer::timeout, this, &UiController::refreshIO);
    m_ioTimer->start(200);

    // 3. 创建TaskExecutor并连接信号
    m_taskExecutor = new TaskExecutor(m_robotController, this);
    connect(m_taskExecutor, &TaskExecutor::taskProgress, this, &UiController::onTaskProgress);
    connect(m_taskExecutor, &TaskExecutor::taskPaused, this, &UiController::onTaskPaused);
    // ... 其他TaskExecutor信号连接

    // 4. 连接报警信号
    connect(m_robotController, &RobotController::errorOccurred, this, &UiController::onRobotError);

    // 5. 连接编程页面按钮（使用lambda批量连接）
    auto connProgBtn = [this](const QString& name, auto slot) {
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(name))
            connect(btn, &QPushButton::clicked, this, slot);
    };
    connProgBtn("btnProgRun", &UiController::onProgRun);
    connProgBtn("btnProgMoveJ", &UiController::onProgInsertMoveJ);
    // ... 其他按钮连接
}
```

#### `parseScript(text)` - 脚本解析核心
```cpp
QList<ScriptCommand> UiController::parseScript(const QString& text) {
    QList<ScriptCommand> result;

    // 逐行解析
    for (每行) {
        if (行以"TARGET "开头) {
            // 解析: TARGET P1 = {J0: 0, J1: 90, ...}
            QString name = 提取点位名称;
            QList<double> vals = 解析6个值;
            m_targets[name] = vals;  // 存入点位缓存
            continue; // 不产生执行指令
        }

        if (行以"MOVEJ("开头) {
            cmd.type = ScriptCmdType::MoveJ;
            if (含花括号) {
                // 旧格式: MOVEJ({J0:0,...}, speed=25)
                cmd.targets = 解析6个值;
            } else {
                // 新格式: MOVEJ(P1, speed=25)
                cmd.targetName = 提取点位名称;
                cmd.targets = m_targets[点位名称];
            }
            cmd.speed/accel/decel = 解析speed=/acc=/dec=参数;
            result.append(cmd);
        }

        // 类似处理 MOVEL, FOR, IF_DI, DELAY, SET_DO, WAIT_DI 等
    }
    return result;
}
```

#### `onProgInsertMoveJ()` - 插入MOVEJ指令
```cpp
void UiController::onProgInsertMoveJ() {
    // 1. 读取当前关节角度
    QList<double> joints = m_robotController->getState()->getAllAxisPositions();

    // 2. 读取默认运动参数
    double spd, acc, dec; int timeout;
    readDefaultMotionParams(spd, acc, dec, timeout);

    // 3. 生成唯一点位名称
    QString name = nextTargetName(m_targetCounter, m_targets);

    // 4. 写入点位编辑器
    QString target = QString("TARGET %1 = {J0: %2, J1: %3, J2: %4, J3: %5, J4: %6, J5: %7}")
        .arg(name).arg(joints[0]).arg(joints[1])...;
    insertAtCursor(getTargetEditor(), target);

    // 5. 插入MOVEJ指令到程序编辑器
    QString cmd = QString("MOVEJ(%1, speed=%2, acc=%3, dec=%4)")
        .arg(name).arg(spd).arg(acc).arg(dec);
    insertAtCursor(cmd);

    // 6. 更新点位缓存并同步
    m_targets.insert(name, joints);
    syncTargets();
}
```

---

### 4. HomingController（回原点控制器）

**职责**：
- 负责六轴机械臂的回零/回原点操作
- 串行预处理（J3→J2→J5 依次移开原点/反向限位）
- 并行回零（6轴同时执行回零流程）
- 回零进度和结果通知

**回零流程**（参考 HomeManager 设计）：
```
1. 串行预处理：J3→J2→J5 依次检查传感器、移开原点
2. 并行回零：6轴同时执行回零状态机
3. 每轴状态机：
   INIT → CHECK_SENSOR → MOVE_AWAY_ORIGIN → WAIT →
   MOVE_AWAY_REVERSE → WAIT → START_DATUM → WAIT_DATUM → SUCCESS/FAIL
```

**架构位置**：
```
MainWindow → RobotController → HomingController → ZmotionAPI → 硬件
```

---

#### 数据结构定义（`HomingController.h`）

**HomeAxisHWConfig 结构体** - 轴硬件配置：
```cpp
struct HomeAxisHWConfig {
    int axis;        // 轴号（0-5对应J0-J5）
    int atype;       // 轴类型（1=旋转轴）
    int dir;         // 轴方向（1=正向，-1=反向）
    int homeIn;      // 原点传感器输入IO编号
    int negIn;       // 反向限位传感器输入IO编号（-1表示无）
};
```

**HomeStrategy 结构体** - 回零策略：
```cpp
struct HomeStrategy {
    float moveAwayOrigin;   // 移开原点的相对距离
    float moveAwayReverse;  // 移开反向限位的相对距离
    float homeSpeed;        // 回零高速（度/秒）
    float homeAccel;        // 回零加速度（度/秒²）
    float homeDecel;        // 回零减速度（度/秒²）
    float homeLspeed;       // 回零慢速阈值（度/秒）
    float homeSramp;        // S曲线过渡时间（毫秒）
    float homeCreep;        // 回零爬行速度（度/秒）
    int   datumMode;        // 回零模式（3=正向限位回零，4=反向限位回零）
};
```

**HomeAxisState 枚举** - 回零状态机：
```cpp
enum HomeAxisState {
    HOME_INIT = 0,                 // 初始状态：设置运动参数
    HOME_CHECK_SENSOR,             // 检查传感器状态
    HOME_MOVE_AWAY_ORIGIN,         // 发送移开原点命令
    HOME_WAIT_MOVE_AWAY_ORIGIN,    // 等待移开原点完成
    HOME_MOVE_AWAY_REVERSE,        // 发送移开反向限位命令
    HOME_WAIT_MOVE_AWAY_REVERSE,   // 等待移开反向限位完成
    HOME_START_DATUM,              // 发送回零命令
    HOME_WAIT_DATUM_START,         // 等待回零启动
    HOME_WAIT_DATUM,               // 等待回零完成
    HOME_SUCCESS,                  // 回零成功
    HOME_FAIL                      // 回零失败
};
```

---

#### 全局配置数组（`HomingController.cpp` 匿名命名空间）

**g_axis_hw 数组** - 6轴硬件配置表：
| 轴号 | axis | atype | dir | homeIn | negIn | 说明 |
|------|------|-------|-----|--------|-------|------|
| J0 | 0 | 1 | +1 | 0 | 1 | 机械臂第1轴，原点IN0，反向限位IN1 |
| J1 | 1 | 1 | +1 | 2 | -1 | 机械臂第2轴，原点IN2，无反向限位 |
| J2 | 2 | 1 | -1 | 3 | -1 | 机械臂第3轴，原点IN3，反向运动 |
| J3 | 3 | 1 | +1 | 4 | 5 | 机械臂第4轴，原点IN4，反向限位IN5 |
| J4 | 4 | 1 | +1 | 6 | -1 | 机械臂第5轴，原点IN6，无反向限位 |
| J5 | 5 | 1 | +1 | 7 | 8 | 机械臂第6轴，原点IN7，反向限位IN8 |

**g_home_strategy 数组** - 6轴回零策略表：
| 轴号 | moveAwayOrigin | moveAwayReverse | homeSpeed | homeAccel | homeDecel | homeLspeed | homeSramp | homeCreep | datumMode |
|------|----------------|------------------|-----------|-----------|-----------|------------|-----------|------------|-----------|
| J0 | -10.0 | -155.0 | 40.0 | 80.0 | 100.0 | 0.0 | 200.0 | 1.0 | 3 |
| J1 | +38.0 | 0.0 | 40.0 | 80.0 | 200.0 | 0.0 | 200.0 | 1.0 | 4 |
| J2 | -50.0 | 0.0 | 20.0 | 50.0 | 100.0 | 0.0 | 200.0 | 1.0 | 3 |
| J3 | -10.0 | -200.0 | 40.0 | 80.0 | 100.0 | 0.0 | 200.0 | 1.0 | 3 |
| J4 | -20.0 | 0.0 | 40.0 | 80.0 | 100.0 | 0.0 | 200.0 | 1.0 | 3 |
| J5 | -10.0 | -220.0 | 60.0 | 100.0 | 200.0 | 0.0 | 200.0 | 2.0 | 3 |

**常量**：
- `kMoveTimeoutMs = 30000` - 移开原点/反向限位操作超时（30秒）
- `axisName(i)` - 生成轴名称辅助函数（如"机械臂第1轴(J0)"）

---

#### 核心方法详解（`HomingController.cpp`）

| 方法名 | 功能说明 | 关键实现细节 |
|--------|----------|--------------|
| `HomingController(api, config, state, parent)` | 构造函数 | 初始化6轴状态数组、创建轮询定时器（200ms）并连接超时信号 |
| `~HomingController()` | 析构函数 | 调用stopHoming()释放资源 |
| `bindZmotionAPI(api)` | 绑定API | 用于运行时切换API实例 |
| `reset()` | 重置回零状态 | 6轴回零状态→HOME_INIT、时间戳清零、J2/J3/J5移开标志清零 |
| `startHoming()` | 启动回零流程 | 检查是否已连接→调用reset()→关闭日志→rapidStop()→启动200ms轮询定时器→emit homingStarted() |
| `stopHoming()` | 停止回零操作 | 停止定时器→rapidStop()→emit homingFailed() |
| `step()` | 主循环入口 | 先执行stepSerial()，再执行stepParallel() |
| `isAllHomed()` | 检查所有轴是否完成回零 | 遍历6轴检查m_homeState[i] == HOME_SUCCESS |
| `onPollTimeout()` | 定时器超时回调 | 调用step()→检查全部完成/有失败→发射信号 |
| `stepSerial()` | 串行预处理 | 顺序处理J2(索引2)→J1(索引1)→J4(索引4)，每次只处理一个轴 |
| `processPrehomeAxis(i)` | 处理单个轴的串行预处理 | 串行阶段的状态机（不检查反向限位） |
| `stepParallel()` | 并行回零 | 遍历6轴同时执行回零状态机（含反向限位检测） |
| `initAxisParams(i)` | 初始化轴参数 | 设置ATYPE/Units/Speed/Accel/Decel/Lspeed/Creep/Sramp/原点IO/IO反转/使能 |
| `checkAxisSensors(i, checkNegIn)` | 检查传感器 | 读取原点IO和反向限位IO，返回0/1/2（-1=IO错误） |
| `isAxisIdle(i)` | 检查轴是否空闲 | 使用getIfIdle2()（更可靠，会同时检查MSPEED） |
| `moveAxisRelative(i, distance)` | 相对运动 | 调用setAxisSingleMove() |
| `startAxisDatum(i)` | 启动回零 | 使能轴→调用setAxisSingleDatum(axis, datumMode, speed, creep, 0.0) |
| `moveAllAxesToPositions(positions, speed, accel, decel)` | 移动到指定位置 | 遍历6轴调用startAxisSingleMoveAbs() |

---

#### 回零方式详解

##### 1. **串行预处理**（`stepSerial` + `processPrehomeAxis`）

**目的**：J2→J1→J4 三个轴依次处理，先移开原点传感器，避免后续回零时发生机械干涉

**处理顺序**：
```cpp
// 机械臂第3轴(J2) → 机械臂第2轴(J1) → 机械臂第5轴(J4)
if (!m_j3MovedAway && !m_isHomed[2]) {
    processPrehomeAxis(2);  // J2: 索引2，机械臂第3轴
    return;
}
if (!m_j2MovedAway && !m_isHomed[1]) {
    processPrehomeAxis(1);  // J1: 索引1，机械臂第2轴
    return;
}
if (!m_j5MovedAway && !m_isHomed[4]) {
    processPrehomeAxis(4);  // J4: 索引4，机械臂第5轴
    return;
}
```

**串行阶段状态机（简化版，不检测反向限位）**：
```
HOME_INIT 
   → 设置轴参数(atype/units/speed/accel/decel/lspeed/creep/sramp/originIO/enable)
   ↓
HOME_CHECK_SENSOR
   → 读取原点IO → 若已触发则直接到START_DATUM
   ↓
HOME_MOVE_AWAY_ORIGIN
   → 发送相对运动moveAwayOrigin距离移开原点
   ↓
HOME_WAIT_MOVE_AWAY_ORIGIN
   → 等待isAxisIdle() + 超时检测(30s)
   ↓
HOME_START_DATUM
   → 发送回零命令，记录起始时间
   ↓
HOME_WAIT_DATUM
   → 检查getAxisSingleHomeStatus()，超时则失败
   ↓
HOME_SUCCESS / HOME_FAIL
```

**关键实现**：
```cpp
void HomingController::processPrehomeAxis(int i) {
    HomeAxisState& state = m_homeState[i];
    bool& movedAway = (i == 2 ? m_j3MovedAway : (i == 1 ? m_j2MovedAway : m_j5MovedAway));

    switch (state) {
    case HOME_INIT:
        if (!initAxisParams(i)) { state = HOME_FAIL; ... return; }
        state = HOME_CHECK_SENSOR;
        break;

    case HOME_CHECK_SENSOR: {
        int diVal = m_zmotionAPI->getDI(hw.homeIn);
        bool homeTriggered = (diVal != 0);  // 低电平触发
        if (!homeTriggered) {
            state = HOME_MOVE_AWAY_ORIGIN;  // 需要移开
        } else {
            movedAway = true;
            state = HOME_START_DATUM;       // 已离开，直接回零
        }
        break;
    }

    case HOME_MOVE_AWAY_ORIGIN:
        moveAxisRelative(i, st.moveAwayOrigin);  // 移开原点
        m_moveStartTimeMs[i] = currentTime;
        state = HOME_WAIT_MOVE_AWAY_ORIGIN;
        break;

    case HOME_WAIT_MOVE_AWAY_ORIGIN:
        if (isAxisIdle(i)) {  // 已停止
            movedAway = true;
            state = HOME_START_DATUM;
        } else if (timeout > 30s) {  // 超时
            state = HOME_FAIL;
        }
        break;

    case HOME_START_DATUM:
        startAxisDatum(i);  // 启动回零
        state = HOME_WAIT_DATUM;
        break;

    case HOME_WAIT_DATUM:
        // 使用getAxisSingleHomeStatus验证DATUM真正成功
        ZmotionResult r = m_zmotionAPI->getAxisSingleHomeStatus(hw.axis);
        if (r.success && r.axisHomeStatus != 0) {
            m_isHomed[i] = true;
            state = HOME_SUCCESS;
        } else if (elapsed > 180s) {
            state = HOME_FAIL;
        }
        break;
    }
}
```

##### 2. **并行回零**（`stepParallel`）

**目的**：6轴同时回零，比串行处理快5-6倍

**与串行的区别**：
1. 同时处理所有轴（而非一次一个）
2. 增加反向限位传感器检测和移开逻辑
3. 回零等待阶段增加反向限位触发回退保护

**并行阶段状态机（完整版）**：
```
HOME_INIT
   ↓
HOME_CHECK_SENSOR（同时检查原点和反向限位）
   ├─ 原点被触发 → MOVE_AWAY_ORIGIN
   ├─ 反向限位被触发 → MOVE_AWAY_REVERSE
   └─ 都未触发 → START_DATUM（直接回零）
   ↓
HOME_MOVE_AWAY_ORIGIN → HOME_WAIT_MOVE_AWAY_ORIGIN
   ↓ (完成后回到 CHECK_SENSOR 重新判断)
HOME_MOVE_AWAY_REVERSE → HOME_WAIT_MOVE_AWAY_REVERSE
   ↓
HOME_START_DATUM → HOME_WAIT_DATUM
   │ (等待期间持续检查反向限位)
   │ (反向限位触发 → 取消回零 → MOVE_AWAY_REVERSE)
   ↓
HOME_SUCCESS / HOME_FAIL
```

**关键实现 - 反向限位保护**：
```cpp
case HOME_WAIT_DATUM: {
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // 1. 反向限位触发检测与回退
    if (hw.negIn >= 0) {
        int negTriggered = m_zmotionAPI->getDI(hw.negIn);
        if (!negTriggered) {  // 0=触发
            m_zmotionAPI->singleAxisCancel({hw.axis}, 2);  // 取消运动
            startTime = 0;
            state = HOME_MOVE_AWAY_REVERSE;  // 回退
            emit homingProgress(i, state);
            break;
        }
    }

    // 2. 检查回零状态
    ZmotionResult r = m_zmotionAPI->getAxisSingleHomeStatus(hw.axis);
    if (r.success && r.axisHomeStatus != 0) {
        m_isHomed[i] = true;
        state = HOME_SUCCESS;
    } else if (startTime != 0 && (now - startTime) > m_homingTimeoutMs) {
        state = HOME_FAIL;  // 超时
    }
    break;
}
```

---

#### `initAxisParams` - 轴参数初始化详解

**目的**：为每轴设置完整的回零所需参数

**实现流程**：
```cpp
bool HomingController::initAxisParams(int i) {
    // 1. 设置轴类型
    setAtype(hw.axis, hw.atype);  // 1=旋转轴

    // 2. 设置脉冲当量
    double axisUnit = m_config->getAxisUnitsList().at(i);
    setAxisUnits({hw.axis}, {axisUnit});

    // 3. 设置运动参数
    setAxisSpeed({hw.axis}, {st.homeSpeed});    // 回零高速
    setAxisAccel({hw.axis}, {st.homeAccel});    // 加速度
    setAxisDecel({hw.axis}, {st.homeDecel});    // 减速度
    setAxisLspeed({hw.axis}, {st.homeLspeed});  // 起始速度
    setAxisCreepSpeed({hw.axis}, {st.homeCreep});// 爬行速度
    setAxisSramp({hw.axis}, {st.homeSramp});    // S曲线时间

    // 4. 映射原点IO
    setAxisDatumOrigin({hw.axis}, {hw.homeIn});

    // 5. 反转IO电平（低电平触发）
    setInvertIn(hw.homeIn, 1);
    if (hw.negIn >= 0) {
        setInvertIn(hw.negIn, 1);
    }

    // 6. 使能轴
    setAxisEnable({hw.axis}, 1);

    return true;  // 失败返回false
}
```

---

#### `startAxisDatum` - 启动回零详解

**调用链**：
```cpp
bool HomingController::startAxisDatum(int i) {
    // 1. 使能轴
    setAxisEnable({hw.axis}, 1);

    // 2. 发送回零命令
    // 参数：轴号, 回零模式, 回零速度, 爬行速度, 偏移量
    setAxisSingleDatum(hw.axis, st.datumMode, st.homeSpeed, st.homeCreep, 0.0f);
    // datumMode=3: 正向限位回零
    // datumMode=4: 反向限位回零
}
```

**回零模式说明**：
- `datumMode=3` (DATUM_MODE_LIMIT_FWD)：高速向正向运动，碰到限位后减速反向找原点
- `datumMode=4` (DATUM_MODE_LIMIT_REV)：高速向反向运动，碰到限位后减速反向找原点
- 控制器内部状态机：高速→撞限位→反向低速爬行→找到原点IO上升沿→停止

---

#### 整体回零流程图

```
用户调用 executeHoming()
        ↓
RobotController::executeHoming()
        ↓
HomingController::startHoming()
   ├─ reset() 重置状态
   ├─ 关闭日志、rapidStop()
   ├─ 启动 200ms 轮询定时器
   └─ emit homingStarted()
        ↓
轮询定时器每 200ms 触发
        ↓
HomingController::step()
   ├─ stepSerial()  // 串行预处理
   │   ├─ processPrehomeAxis(2) → J2 移开原点
   │   ├─ processPrehomeAxis(1) → J1 移开原点
   │   └─ processPrehomeAxis(4) → J4 移开原点
   │       （每个轴：INIT → SENSOR → MOVE_AWAY → START_DATUM → WAIT → SUCCESS）
   │
   └─ stepParallel()  // 并行回零（6轴同时）
       └─ 遍历6轴执行：
           INIT → CHECK_SENSOR → MOVE_AWAY_* → START_DATUM
                                  → WAIT_DATUM
                                      ├─ 反向限位触发 → 回退到 MOVE_AWAY_REVERSE
                                      └─ 回零完成 → SUCCESS
        ↓
onPollTimeout() 检查完成状态
   ├─ 全部SUCCESS → 停止定时器 → emit homingCompleted()
   └─ 任意FAIL → 停止定时器 → emit homingFailed()
        ↓
RobotController 收到 homingCompleted
        ↓
moveAllAxesToPositions(homePosition)  // 移动到 homePosition
        ↓
emit operationCompleted("回零")
```

---

#### 信号定义

```cpp
signals:
    void homingStarted();                                    // 回零开始
    void homingProgress(int axisIndex, HomeAxisState state); // 单轴进度更新
    void homingAxisCompleted(int axisIndex);                 // 单轴回零完成
    void homingAxisFailed(int axisIndex, QString errorMsg);  // 单轴回零失败
    void homingCompleted();                                  // 全部回零完成
    void homingFailed(QString errorMsg);                     // 回零失败
```

#### 配置接口

```cpp
// 设置/获取回零超时时间（默认180秒）
void setHomingTimeoutMs(int ms);
int getHomingTimeoutMs() const;

// 设置/获取轮询间隔（默认200ms）
void setPollInterval(int ms);
int getPollInterval() const;

// 查询回零状态
bool isHoming() const;        // 正在回零
bool isAllHomed() const;      // 全部回零完成
QList<bool> homedFlags() const; // 各轴回零状态
```

---

## 🔧 硬件驱动层详解

### 1. MotionController（运动控制器）

**职责**：
- 管理与 ZMC306X 控制器的 TCP/IP 连接
- 定时轮询更新机械臂实时状态（轴位置、速度、笛卡尔坐标）
- 管理正解/逆解运动学模式切换
- 错误处理与信号转发

**架构位置**：
```
RobotController → MotionController → ZmotionAPI → ZMC306X控制器 → 6轴电机
```

**核心方法详解**（对应 `MotionController.h`）：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `MotionController(config, state, parent)` | 构造函数 | `config`: 配置对象, `state`: 状态对象 | - |
| `connect(ipAddr)` | 建立TCP/IP连接 | `ipAddr`: 控制器IP地址 | `bool`: 是否成功 |
| `disconnect()` | 断开连接 | 无 | `bool`: 是否成功 |
| `isConnected()` | 查询连接状态 | 无 | `bool` |
| `getZmotionAPI()` | 获取底层API实例 | 无 | `ZmotionAPI*` |
| `updateState()` | 手动触发状态更新（读取轴位置/速度） | 无 | `void` |
| `setStateUpdateInterval(ms)` | 设置自动轮询间隔 | `ms`: 毫秒数 | `void` |
| `getStateUpdateInterval()` | 获取当前轮询间隔 | 无 | `int` (默认100ms) |
| `setInverseKinematicsMode()` | 切换到逆解模式（笛卡尔控制） | 无 | `bool` |
| `setForwardKinematicsMode()` | 切换到正解模式（关节控制） | 无 | `bool` |
| `isInverseKinematicsMode()` | 查询当前是否为逆解模式 | 无 | `bool` |

**信号定义**：
```cpp
signals:
    void connected();                          // 连接成功
    void disconnected();                       // 断开连接
    void connectionError(const QString&);      // 连接错误
    void motionStarted(int axisIndex);         // 轴开始运动
    void motionStopped(int axisIndex);         // 轴停止运动
    void motionCompleted(int axisIndex);       // 轴运动完成
    void hardwareError(int code, const QString&); // 硬件错误
    void statusUpdated();                      // 状态已更新
    void kinematicsModeChanged(bool isInverse); // 运动学模式切换
```

**私有成员**：
| 成员 | 类型 | 说明 |
|------|------|------|
| `m_config` | `RobotConfig*` | 机器人配置（轴数、脉冲当量等） |
| `m_state` | `RobotState*` | 实时状态（轴位置、速度、笛卡尔坐标） |
| `m_zmotionAPI` | `ZmotionAPI*` | 底层API实例 |
| `m_isConnected` | `bool` | 连接状态标志 |
| `m_updateTimer` | `QTimer*` | 状态轮询定时器 |
| `m_updateIntervalMs` | `int` | 轮询间隔（默认100ms） |

---

### 2. ServoController（舵机控制器）

**职责**：
- 控制6路 KPZ32 舵机（P01~P06），角度范围 0°~180°
- 支持速度控制：通过50ms定时器逐步逼近目标角度，实现平滑转动
- 支持拖拽模式：直接更新内部状态，由定时器批量帧统一发送
- 角度→PWM占空比自动换算（公式：`duty = 25 + angle × 100/180`）
- 实际通信委托给 `Rs485Controller`，自身不直接操作硬件

**架构位置**：
```
MainWindow → ServoController → Rs485Controller → ZMC306X(Modbus主端) → QD-6PWM-V2.0 → KPZ32舵机
```

---

#### 核心常量（`ServoController.cpp`）

| 常量 | 值 | 说明 |
|------|-----|------|
| `MIN_ANGLE` | 0.0f | 舵机最小角度（对应500us脉宽） |
| `MAX_ANGLE` | 180.0f | 舵机最大角度（对应2500us脉宽） |
| `DEFAULT_FREQ_HZ` | 50 | PWM频率（标准舵机要求50Hz） |
| `CENTER_DUTY` | 75 | 中位占空比（75/1000 = 7.5% = 90°） |
| `TIMER_INTERVAL_MS` | 50 | 定时器间隔（50ms = 每秒20次更新） |

---

#### ServoState 结构体（每通道状态）

```cpp
struct ServoState {
    float currentAngle = 90.0f;   // 当前角度（定时器逐步更新）
    float targetAngle  = 90.0f;   // 目标角度（用户设置）
    float speed        = 10.0f;   // 转动速度（度/秒，默认10°/s）
    bool  isMoving     = false;   // 是否正在移动
};
```

**currentAngle 与 targetAngle 的关系**：
```
定时器每50ms触发一次 onTimerTick()：
  step = speed × (50/1000)  ← 每周期推进的角度
  currentAngle += step      ← 逐步逼近 targetAngle
  当 |targetAngle - currentAngle| < 0.5° 时，到达目标，停止移动
```

---

#### 核心方法详解

**查询方法**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `getCurrentAngle(channel)` | 获取当前角度 | `channel`: 0-5 | `float` (度) |
| `getTargetAngle(channel)` | 获取目标角度 | `channel`: 0-5 | `float` (度) |
| `getSpeed(channel)` | 获取转动速度 | `channel`: 0-5 | `float` (度/秒) |
| `isMoving(channel)` | 查询是否正在移动 | `channel`: 0-5 | `bool` |
| `checkChannel(channel)` | 检查通道号合法性 | `channel`: 0-5 | `bool` |

**控制槽函数**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setAngleWithSpeed(channel, angle, speed)` | 设置舵机角度（带速度控制，平滑转动） | `channel`:0-5, `angle`:0-180°, `speed`:度/秒(默认10) | `bool` |
| `setAngleDrag(channel, angle)` | 拖拽模式（仅更新状态，由定时器批量发送） | `channel`:0-5, `angle`:0-180° | `bool` |
| `setDefaultSpeed(channel, speed)` | 设置通道默认速度 | `channel`:0-5, `speed`:度/秒 | `void` |
| `stop(channel)` | 停止指定通道并保持当前位置 | `channel`: 0-5 | `bool` |
| `stopAll()` | 停止所有通道 | 无 | `bool` |
| `initAll()` | 初始化所有舵机到指定角度（PWM1=122°,PWM2=113°,PWM3=49°,PWM4=109°,PWM5=75°,PWM6=80°） | 无 | `bool` |

**信号定义**：
```cpp
signals:
    void angleChanged(int channel, float angle);       // 角度变化
    void moveStarted(int channel);                     // 开始移动
    void moveCompleted(int channel, float angle);      // 移动完成
    void errorOccurred(const QString& message);        // 错误发生
```

---

#### 核心实现机制

##### 1. 速度控制（`setAngleWithSpeed`）

```cpp
bool ServoController::setAngleWithSpeed(int channel, float angle, float speed)
{
    // 1. 角度限幅 [0, 180]
    // 2. 速度限幅 [1, 180] 度/秒
    // 3. 记录目标状态
    m_states[channel].targetAngle = angle;
    m_states[channel].speed = speed;
    m_states[channel].isMoving = true;  // 触发定时器开始推进
    emit moveStarted(channel);
}
```

##### 2. 定时器主循环（`onTimerTick`，每50ms触发）

```cpp
void ServoController::onTimerTick()
{
    // 1. 检查是否有舵机在移动
    bool anyMoving = false;
    for (int i = 0; i < m_channelCount; ++i) {
        if (m_states[i].isMoving) { anyMoving = true; break; }
    }
    if (!anyMoving) return;

    // 2. 收集各通道最新角度
    float angles[6];
    for (int i = 0; i < m_channelCount; ++i)
        angles[i] = m_states[i].currentAngle;

    // 3. 遍历所有通道，计算新角度
    for (int i = 0; i < m_channelCount; ++i) {
        if (!m_states[i].isMoving) continue;

        float diff = m_states[i].targetAngle - m_states[i].currentAngle;

        // 到达目标（误差<0.5°）
        if (qAbs(diff) < 0.5f) {
            m_states[i].currentAngle = m_states[i].targetAngle;
            m_states[i].isMoving = false;
            emit moveCompleted(i, m_states[i].targetAngle);
            continue;
        }

        // 每周期步进量 = 速度 × 时间间隔
        float step = m_states[i].speed * (TIMER_INTERVAL_MS / 1000.0f);
        if (qAbs(step) > qAbs(diff)) step = diff;  // 不超过剩余距离

        m_states[i].currentAngle += step;
        angles[i] = m_states[i].currentAngle;
    }

    // 4. 批量发送所有6路（一次通信完成）
    quint16 duties[6];
    for (int i = 0; i < m_channelCount; ++i) {
        float angle = angles[i];
        if (angle < MIN_ANGLE) angle = MIN_ANGLE;
        if (angle > MAX_ANGLE) angle = MAX_ANGLE;
        duties[i] = static_cast<quint16>(25.0f + (angle / 180.0f) * 100.0f);
    }
    m_rs485->setAllPwm(0x21, 50,duties[0], 50,duties[1], 50,duties[2], 50,duties[3], duties[4], duties[5]);
}
```

##### 3. 拖拽模式（`setAngleDrag`）

```cpp
// 直接更新内部状态，不单独发送
// 由 onTimerTick 的批量帧统一发送，避免P05/P06滞后
m_states[channel].currentAngle = angle;
m_states[channel].targetAngle = angle;
m_states[channel].isMoving = true;  // 标记为移动中，触发批量发送
```

##### 4. 角度→占空比换算（`angleToDutyPermil`）

```cpp
// KPZ32舵机 PWM 映射（50Hz, 周期20ms）：
//   0°  → 500us  → 2.5%  = 25/1000
//   90° → 1500us → 7.5%  = 75/1000
//   180°→ 2500us → 12.5% = 125/1000
//
// 公式: dutyPermil = 25 + (angle / 180) × 100
//
// 示例:
//   122° → 25 + 122×100/180 = 92.78 → 92 (截断)
//   90°  → 25 + 90×100/180  = 75.00 → 75
//   49°  → 25 + 49×100/180  = 52.22 → 52
```

##### 5. 初始化角度（`initAll`）

```cpp
bool ServoController::initAll()
{
    const float initAngles[] = {122.0f, 113.0f, 49.0f, 109.0f, 75.0f, 80.0f};

    // 通过 angleToDutyPermil() 自动计算占空比
    quint16 d1 = angleToDutyPermil(initAngles[0]);  // 122° → 92
    quint16 d2 = angleToDutyPermil(initAngles[1]);  // 113° → 87
    quint16 d3 = angleToDutyPermil(initAngles[2]);  // 49°  → 52
    quint16 d4 = angleToDutyPermil(initAngles[3]);  // 109° → 85
    quint16 d5 = angleToDutyPermil(initAngles[4]);  // 75°  → 66
    quint16 d6 = angleToDutyPermil(initAngles[5]);  // 80°  → 69

    // 批量发送
    m_rs485->setAllPwm(0x21, 50,d1, 50,d2, 50,d3, 50,d4, d5, d6);
}
```

---

#### 发送方式对比

| 场景 | 调用函数 | 底层发送方式 | 通信次数 |
|------|---------|------------|---------|
| 平滑转动（定时器） | `onTimerTick()` | `setAllPwm()` → `sendModbusRequest()` | 每50ms 1次 |
| 拖拽模式 | `setAngleDrag()` | 同上（由定时器统一发送） | 同上 |
| 初始化 | `initAll()` | `setAllPwm()` → `sendModbusRequest()` | 1次 |
| 停止保持位置 | `stop()` | `setServoAngle()` → `sendRawModbusFrame()` | 1次 |

---

### 3. Rs485Controller（RS485 Modbus通信控制器）

**职责**：
- 通过 ZMC306X 控制器的 Modbus 转 RS485 功能，控制外部 QD-6PWM-V2.0 六路 PWM 模块
- 支持两种发送模式：ZMC 自动组帧（标准）和用户自行组帧（RAW）
- 实现角度→占空比转换（`setServoAngle`）
- 单路设置和批量设置两种 PWM 协议

**架构位置**：
```
ServoController → Rs485Controller → ZmotionAPI → ZMC306X(Modbus主端) → RS485总线 → QD-6PWM-V2.0
```

**硬件信息**：
| 项目 | 值 |
|------|-----|
| PWM模块型号 | QD-6PWM-V2.0（武汉奇点美信） |
| 从站地址 | 0x21（出厂默认） |
| 通信协议 | Modbus RTU |
| 波特率 | 9600, 8, N, 1 |
| PWM通道数 | 6路（P01~P06） |
| PWM1-4频率范围 | 1Hz ~ 100KHz（各路独立） |
| PWM5-6频率 | 与PWM1共享（不可独立设置） |
| 占空比范围 | 0 ~ 1000（千分比，0%~100%） |

---

#### Modbus 寄存器地址表

**单路PWM协议（规格书第5节第1小节）**：

| 通道 | 寄存器地址 | 数据格式 |
|------|-----------|---------|
| PWM1 | 0x2251 | 频率(3B) + 占空比(2B) + 填充(1B) |
| PWM2 | 0x2252 | 频率(3B) + 占空比(2B) + 填充(1B) |
| PWM3 | 0x2253 | 频率(3B) + 占空比(2B) + 填充(1B) |
| PWM4 | 0x2254 | 频率(3B) + 占空比(2B) + 填充(1B) |
| PWM5 | 0x2255 | 频率(3B) + 占空比(2B) + 填充(1B) |
| PWM6 | 0x2256 | 频率(3B) + 占空比(2B) + 填充(1B) |

**批量PWM协议（规格书第5节第2小节）**：

| 项目 | 值 |
|------|-----|
| 起始地址 | 0x0246 |
| 寄存器数量 | 12（0x000C） |
| 数据字节数 | 24（0x18） |
| 数据格式 | PWM1-4各5字节(频率3B+占空比2B) + PWM5-6各2字节(仅占空比) |

---

#### 核心方法详解

**查询方法**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `isConnected()` | 查询ZMotion API是否已设置 | 无 | `bool` |
| `getZmotionAPI()` | 获取ZMotion API指针 | 无 | `ZmotionAPI*` |

**设置方法**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setZmotionAPI(api)` | 设置ZMotion API指针 | `api`: ZmotionAPI指针 | `void` |
| `setServoAngle(pwmIndex, angle)` | 设置舵机角度（内部自动转占空比） | `pwmIndex`:1-6, `angle`:0-180° | `bool` |
| `setPwmDuty(pwmIndex, dutyCycle)` | 设置单路占空比（使用默认频率50Hz） | `pwmIndex`:1-6, `dutyCycle`:0-1000 | `bool` |
| `setSinglePwmFrequencyAndDuty(addr, idx, freq, duty)` | 设置单路频率+占空比 | `addr`:从站地址, `idx`:1-6, `freq`:Hz, `duty`:0-1000 | `bool` |
| `setAllPwm(slave, freq1,d1, freq2,d2, freq3,d3, freq4,d4, d5, d6)` | 批量设置6路PWM | `slave`:从站地址, `freq1-4`:Hz, `d1-6`:0-1000 | `bool` |

**底层发送方法**：

| 方法名 | 功能说明 | 帧处理方 | 参数 | 返回值 |
|--------|----------|---------|------|--------|
| `sendModbusRequest(slave, funcCode, startAddr, data)` | 通过ZMC发送Modbus请求 | **ZMC内部自动组帧**（含CRC/帧头） | `slave`:从站地址, `funcCode`:功能码, `startAddr`:寄存器地址, `data`:数据 | `bool` |
| `sendRawModbusFrame(slave, funcCode, startAddr, data)` | 通过RAW模式发送Modbus帧 | **用户自行组帧**（CRC自动追加） | 同上 | `bool` |

**信号定义**：
```cpp
signals:
    void communicationSuccess();                        // 通信成功
    void communicationFailed(const QString& errorMsg);  // 通信失败
    void dataSent(qint64 bytesWritten);                 // 数据已发送
    void errorOccurred(const QString& errorMsg);        // 错误发生
```

**私有成员**：
| 成员 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `m_zmotionAPI` | `ZmotionAPI*` | nullptr | ZMotion API指针 |
| `m_slaveAddress` | `quint8` | 0x21 | 从站地址 |
| `m_defaultFrequency` | `quint32` | 50 | 默认PWM频率(Hz) |
| `m_pwm1Frequency` | `quint32` | 50 | PWM1当前频率（PWM5/6共享） |
| `m_lastDuty1` | `quint16` | 75 | PWM1上次占空比（初始化检测） |
| `m_localRegBase` | `int` | 0 | 本地MODBUS_REG起始索引 |
| `m_serialConfigured` | `bool` | false | RS485串口是否已配置 |

---

#### 两种发送模式对比

```
┌──────────────────────────────────────────────────────────────────────┐
│  sendModbusRequest() — ZMC内部自动组帧                               │
│                                                                      │
│  1. SETCOM(9600,8,1,0,1,14,2,800)  → 配置Modbus主端模式              │
│  2. MODBUSM_DES(从站地址, 端口)    → 指定目标从站                     │
│  3. 写数据到 MODBUS_REG            → 把数据放进ZMC本地寄存器          │
│  4. MODBUSM_REGSET(start,num,local) → ZMC自动加帧头/CRC，自动发送     │
│                                                                      │
│  ✓ 用户只管喂数据，帧格式/CRC由ZMC硬件负责                           │
│  ✓ 用于 setAllPwm() 批量发送                                         │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│  sendRawModbusFrame() — 用户自行组帧                                 │
│                                                                      │
│  1. 手动拼帧：地址+功能码+起始地址+寄存器数+字节数+数据               │
│  2. 手动算 CRC16，追加到帧尾                                         │
│  3. SETCOM(9600,8,1,0,1,0,2,800) → 切换到RAW透传模式                 │
│  4. PUTCHAR#1,byte              → 逐字节往串口里灌                    │
│  5. SETCOM(9600,8,1,0,1,14,2,800) → 恢复回Modbus主端模式             │
│                                                                      │
│  ✓ 完全掌控帧内容，但要自己算CRC、自己切模式                          │
│  ✓ 用于 setSinglePwmFrequencyAndDuty() 单路设置                      │
└──────────────────────────────────────────────────────────────────────┘
```

| 对比项 | `sendModbusRequest` | `sendRawModbusFrame` |
|--------|--------------------|--------------------|
| 组帧方 | ZMC 控制器自动 | 我们手动拼 |
| CRC | ZMC 自动算 | 我们自己 `calcModbusCRC16()` |
| 发送方式 | `MODBUSM_REGSET` 一条指令 | 逐字节 `PUTCHAR` 循环 |
| 串口模式 | 保持 `mode=14`（Modbus主端） | 先切 `mode=0`（RAW），发完恢复 |
| 使用场景 | `setAllPwm()` 批量6路 | `setSinglePwmFrequencyAndDuty()` 单路 |

---

#### Modbus CRC16 计算

```cpp
static quint16 calcModbusCRC16(const QByteArray& data)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<quint8>(data[i]);
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    return crc;  // 低字节在前，高字节在后
}
```

---

### 4. ServoTestWidget（舵机测试界面）

**职责**：
- 提供6路舵机的独立测试界面
- 支持滑块拖拽、角度输入、速度设置
- 支持勾选通道批量操作

**架构位置**：
```
MainWindow → ServoTestWidget → ServoController → Rs485Controller → 硬件
```

**UI 组件**：

| 组件 | 数量 | 说明 |
|------|------|------|
| `QCheckBox` | 6 | 勾选要操作的通道 |
| `QSlider` | 6 | 角度滑块（0~180°） |
| `QDoubleSpinBox` | 6×2 | 角度输入框 + 速度输入框 |
| `QLabel` | 6 | 当前角度显示 |
| `QPushButton` | 3 | 应用 / 初始化 / 停止 |

**核心槽函数**：

| 方法名 | 功能说明 |
|--------|----------|
| `onApplyClicked()` | 应用按钮：将勾选通道的角度/速度发送到舵机 |
| `onInitClicked()` | 初始化按钮：调用 `ServoController::initAll()` |
| `onStopClicked()` | 停止按钮：停止所有勾选通道 |
| `onSliderChanged(channel, value)` | 滑块变化：实时更新角度输入框并发送（拖拽模式） |
| `onAngleSpinChanged(value)` | 角度输入变化：同步更新对应滑块位置 |
| `onCheckChanged()` | 勾选变化：更新 `m_checkedChannels[]` 数组 |
| `onAngleChanged(channel, angle)` | 角度变化回调：更新当前角度显示标签 |

---

## 🔗 舵机通信架构详解

### 整体通信链路

```
┌─────────────┐    ┌─────────────────┐    ┌──────────────┐    ┌─────────────┐    ┌──────────┐
│ MainWindow  │───▶│ ServoController │───▶│Rs485Controller│───▶│ ZMC306X     │───▶│ QD-6PWM  │
│ (UI/逻辑)   │    │ (角度/速度控制)  │    │(Modbus协议)   │    │(Modbus主端) │    │ V2.0模块  │
└─────────────┘    └─────────────────┘    └──────────────┘    └─────────────┘    └────┬─────┘
                                                                                      │ RS485
                                                                                      ▼
                                                                               ┌──────────┐
                                                                               │ KPZ32舵机 │
                                                                               │ PWM1~PWM6 │
                                                                               └──────────┘
```

### 数据流示例：用户拖拽滑块设置PWM1到120°

```
1. UI层: onSliderChanged(0, 120)
   ↓
2. ServoController::setAngleDrag(0, 120.0f)
   → currentAngle = 120, targetAngle = 120, isMoving = true
   ↓
3. onTimerTick() 触发（50ms后）
   → 计算 duty = 25 + (120/180)×100 = 91
   → 调用 m_rs485->setAllPwm(0x21, 50,91, 50,88, 50,52, 50,86, 66, 69)
   ↓
4. Rs485Controller::setAllPwm()
   → 组装24字节数据：PWM1-4各5字节(频率+占空比) + PWM5-6各2字节(占空比)
   → 调用 sendModbusRequest(0x21, 0x10, 0x0246, data)
   ↓
5. Rs485Controller::sendModbusRequest()
   → SETCOM(9600,8,1,0,1,14,2,800)     配置Modbus主端
   → MODBUSM_DES(0x21, 1)              指定从站
   → MODBUS_REG(0)=0x0032              写入频率高字节
   → MODBUS_REG(1)=0x0A00              写入频率中低字节
   → ...                                写入其他寄存器
   → MODBUSM_REGSET(0x0246, 12, 0)     ZMC自动组帧发送
   ↓
6. ZMC306X控制器
   → 自动组装完整Modbus RTU帧（含CRC）
   → 通过RS485端口发送到总线
   ↓
7. QD-6PWM-V2.0模块（从站地址0x21）
   → 解析Modbus帧
   → PWM1输出：50Hz, 占空比91/1000 (9.1%, 约1820us脉宽)
   ↓
8. KPZ32舵机
   → 收到PWM信号，转动到约120°位置
```

---

## 📊 数据模型层详解

### 1. RobotState（实时状态模型）

**职责**：
- 存储机械臂的实时状态
- 提供状态访问与修改接口
- 发射状态变化信号

**核心数据成员**：
```cpp
class RobotState {
    int m_axisCount;                      // 轴数
    RobotStatus m_status;                // 总体状态（空闲/运动中/错误/急停）
    QList<double> m_axisPositions;       // 各轴位置 (°)
    QList<double> m_axisVelocities;     // 各轴速度
    QList<double> m_cartesianPositions; // 笛卡尔坐标 [X,Y,Z,Rx,Ry,Rz]
    int m_errorCode;                    // 错误码
    QString m_errorMessage;              // 错误信息
};
```

**核心方法详解**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `RobotState(axisCount, parent)` | 构造函数 | `axisCount`: 轴数量 | - |
| `getAxisCount()` | 获取轴数 | 无 | `int` |
| `getStatus()` | 获取当前状态 | 无 | `RobotStatus` |
| `setStatus(status)` | 设置状态（发射信号） | `status`: 新状态 | 无 |
| `getAxisPosition(index)` | 获取单轴位置 | `index`: 轴索引 | `double` |
| `setAxisPosition(index, value)` | 设置单轴位置 | `index`: 轴索引, `value`: 位置值 | 无 |
| `getAllAxisPositions()` | 获取所有轴位置 | 无 | `QList<double>` |
| `setAllAxisPositions(positions)` | 设置所有轴位置 | `positions`: 位置列表 | 无 |
| `getAxisVelocity(index)` | 获取单轴速度 | `index`: 轴索引 | `double` |
| `setAxisVelocity(index, value)` | 设置单轴速度 | `index`: 轴索引, `value`: 速度值 | 无 |
| `getCartesianPosition(index)` | 获取笛卡尔坐标 | `index`: 坐标索引(0-5) | `double` |
| `setCartesianPosition(index, value)` | 设置笛卡尔坐标 | `index`: 坐标索引, `value`: 坐标值 | 无 |
| `getAllCartesianPositions()` | 获取全部笛卡尔坐标 | 无 | `QList<double>` |
| `setAllCartesianPositions(positions)` | 设置全部笛卡尔坐标 | `positions`: 坐标列表 | 无 |
| `getErrorCode()` | 获取错误码 | 无 | `int` |
| `getErrorMessage()` | 获取错误信息 | 无 | `QString` |
| `setError(code, message)` | 设置错误 | `code`: 错误码, `message`: 错误信息 | 无 |
| `clearError()` | 清除错误 | 无 | 无 |
| `isMoving()` | 判断是否正在运动 | 无 | `bool` |
| `isError()` | 判断是否有错误 | 无 | `bool` |

**RobotStatus 枚举**：
```cpp
enum RobotStatus {
    StatusIdle = 0,           // 空闲
    StatusMoving = 1,         // 运动中
    StatusHoming = 2,         // 回零中
    StatusError = 3,          // 错误
    StatusEmergencyStop = 4,  // 急停
    StatusTeaching = 5,       // 示教模式
};
```

**信号定义**：
```cpp
signals:
    void statusChanged(RobotStatus);           // 状态变化
    void axisPositionChanged(int, double);     // 单轴位置变化
    void allAxisPositionsChanged();            // 所有轴位置变化
    void cartesianPositionChanged(int, double);// 笛卡尔坐标变化
    void allCartesianPositionsChanged();       // 所有笛卡尔坐标变化
    void errorOccurred(int, QString);          // 错误发生
    void errorCleared();                       // 错误清除
```

---

### 2. RobotConfig（配置模型）

**职责**：
- 存储机械臂配置参数
- 从JSON文件加载/保存配置

**AxisConfig 结构体**：
```cpp
struct AxisConfig {
    int axisIndex;              // 轴编号
    double maxSpeed;           // 最大速度 (°/s)
    double maxAccel;           // 最大加速度 (°/s²)
    double maxDecel;           // 最大减速度 (°/s²)
    double softLimitMin;       // 软限位最小值
    double softLimitMax;       // 软限位最大值
    double homeSpeed;          // 回零速度
    double homeAccel;          // 回零加速度
    double homeDecel;          // 回零减速度
};
```

**核心数据成员**：
```cpp
class RobotConfig {
    QString m_controllerIP;              // 控制器IP地址
    int m_axisCount;                     // 轴数
    QList<AxisConfig> m_axisConfigs;     // 各轴配置
    double m_defaultSpeed;               // 默认速度
    double m_defaultAccel;               // 默认加速度
    double m_defaultDecel;               // 默认减速度
    int m_defaultTimeout;                // 默认超时时间(ms)
    QList<int> m_jointAxisList;          // 关节轴列表 [0,1,2,3,4,5]
    QList<int> m_virtualAxisList;        // 虚拟轴列表 [6,7,8,9,10,11]
    QList<double> m_axisUnitsList;       // 各轴脉冲当量
};
```

**核心方法详解**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `RobotConfig(parent)` | 构造函数 | - | - |
| `getControllerIP()` | 获取控制器IP | 无 | `QString` |
| `setControllerIP(ip)` | 设置控制器IP | `ip`: IP地址字符串 | 无 |
| `getAxisCount()` | 获取轴数 | 无 | `int` |
| `setAxisCount(count)` | 设置轴数 | `count`: 轴数量 | 无 |
| `getAxisConfig(index)` | 获取单轴配置 | `index`: 轴索引 | `AxisConfig*` |
| `setAxisConfig(index, config)` | 设置单轴配置 | `index`: 轴索引, `config`: AxisConfig | 无 |
| `getDefaultSpeed()` | 获取默认速度 | 无 | `double` |
| `setDefaultSpeed(speed)` | 设置默认速度 | `speed`: 速度值 | 无 |
| `getDefaultAccel()` | 获取默认加速度 | 无 | `double` |
| `setDefaultAccel(accel)` | 设置默认加速度 | `accel`: 加速度值 | 无 |
| `getDefaultDecel()` | 获取默认减速度 | 无 | `double` |
| `setDefaultDecel(decel)` | 设置默认减速度 | `decel`: 减速度值 | 无 |
| `getDefaultTimeout()` | 获取默认超时时间 | 无 | `int` |
| `setDefaultTimeout(timeout)` | 设置默认超时时间 | `timeout`: 超时时间(ms) | 无 |
| `getJointAxisList()` | 获取关节轴列表 | 无 | `QList<int>` |
| `setJointAxisList(list)` | 设置关节轴列表 | `list`: 轴索引列表 | 无 |
| `getVirtualAxisList()` | 获取虚拟轴列表 | 无 | `QList<int>` |
| `setVirtualAxisList(list)` | 设置虚拟轴列表 | `list`: 轴索引列表 | 无 |
| `getAxisUnitsList()` | 获取脉冲当量列表 | 无 | `QList<double>` |
| `setAxisUnitsList(list)` | 设置脉冲当量列表 | `list`: 脉冲当量列表 | 无 |
| `loadFromJson(path)` | 从JSON文件加载配置 | `path`: 文件路径 | `bool` |
| `saveToJson(path)` | 保存配置到JSON文件 | `path`: 文件路径 | `bool` |
| `loadFromJsonString(json)` | 从JSON字符串加载配置 | `json`: JSON字符串 | `bool` |
| `toJsonString()` | 转换为JSON字符串 | 无 | `QString` |

---

### 3. RobotKinematics（运动学计算）

**职责**：
- 生成控制器FRAME6 CONNFRAME所需的25项TableList
- 脉冲当量的设置与解析
- DH参数的管理

**核心数据成员**：
```cpp
class RobotKinematics {
    double m_largeZ;          // 基座垂直高度
    double m_L1;              // 1轴到2轴X偏移
    double m_L2;              // 大臂长度
    double m_L3;              // 3轴到4轴中心距离
    double m_L4;              // 4轴到5轴距离
    double m_L5;              // 5轴到6轴距离（工具安装面）
    double m_L6;              // 工具长度
};
```

**FRAME6 TableList 映射**：

| 索引 | 含义 | 默认值 |
|------|------|--------|
| table[0] | LargeZ（基座垂直高度） | 307.5mm |
| table[1] | L1（1轴到2轴X偏移） | -80.87mm |
| table[2] | L2（大臂长度） | 312.0mm |
| table[3] | L3（3轴到4轴中心距离） | 85.0mm |
| table[4] | L4（4轴到5轴距离） | 387.18mm |
| table[5] | L5（5轴到6轴距离） | 0.0mm |
| table[6] | J0脉冲数 | Units × 360 |
| table[7] | J1脉冲数 | Units × 360 |
| table[8] | J2脉冲数 | Units × 360 |
| table[9] | J3脉冲数 | Units × 360 |
| table[10] | J4脉冲数 | Units × 360 |
| table[11] | J5脉冲数 | Units × 360 |
| table[12-24] | 预留参数 | 0.0 |

**核心方法详解**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `RobotKinematics()` | 构造函数，使用默认参数 | - | - |
| `setParameters(params)` | 设置DH参数 | `params`: DH参数结构体 | 无 |
| `getParameters()` | 获取DH参数 | 无 | `DHParameters` |
| `toControllerTable()` | 生成25元素TABLE数组 | 无 | `QVector<float>` |
| `fromControllerTable(table)` | 从TABLE数组恢复参数 | `table`: 25元素数组 | 无 |
| `validate()` | 验证参数有效性 | 无 | `bool` |
| `getDefaultUnits()` | 获取默认脉冲当量 | 无 | `QList<double>` |

---

## 🔌 硬件API层详解

### ZmotionAPI（底层API封装）

**职责**：
- 封装Zmotion控制器SDK
- 提供简洁的C++接口
- 处理网络通信
- 错误处理与重试机制

**架构位置**：
```
MotionController → ZmotionAPI → ZMC306X控制器
```

**ZmotionResult 结构体**：
```cpp
struct ZmotionResult {
    bool success;          // 操作是否成功
    int errorCode;         // 错误码（0=成功）
    QString errorMessage;  // 错误信息
    QList<double> values;  // 返回值列表
};
```

**核心方法分类**：

#### 1. 连接管理
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `openEth(ipAddr)` | 建立TCP/IP连接 | `ipAddr`: 控制器IP | `bool` |
| `close()` | 断开连接 | 无 | 无 |
| `isConnected()` | 查询连接状态 | 无 | `bool` |
| `getHandle()` | 获取控制器句柄 | 无 | `HANDLE` |

#### 2. 轴配置
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setAtype(axis, type)` | 设置轴类型 | `axis`: 轴号, `type`: 类型(0=虚拟,1=旋转) | `ZmotionResult` |
| `setAxisUnits(axes, units)` | 设置脉冲当量 | `axes`: 轴列表, `units`: 当量列表 | `ZmotionResult` |
| `setAxisSpeed(axes, speeds)` | 设置速度 | `axes`: 轴列表, `speeds`: 速度列表 | `ZmotionResult` |
| `setAxisAccel(axes, accels)` | 设置加速度 | `axes`: 轴列表, `accels`: 加速度列表 | `ZmotionResult` |
| `setAxisDecel(axes, decels)` | 设置减速度 | `axes`: 轴列表, `decels`: 减速度列表 | `ZmotionResult` |
| `setAxisEnable(axes, enable)` | 使能/禁用轴 | `axes`: 轴列表, `enable`: 0/1 | `ZmotionResult` |

#### 3. 运动控制
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `directMoveAbsolute(axes, targets, speed, accel, decel)` | 绝对位置运动 | `axes`:轴列表, `targets`:目标位置 | `ZmotionResult` |
| `directMoveRelative(axes, offsets, speed, accel, decel)` | 相对位置运动 | `axes`:轴列表, `offsets`:偏移量 | `ZmotionResult` |
| `rapidStop(mode)` | 急停 | `mode`: 停止模式(1=减速,2=立即) | `ZmotionResult` |
| `singleAxisCancel(axes, mode)` | 取消单轴运动 | `axes`:轴列表, `mode`: 取消模式 | `ZmotionResult` |

#### 4. IO控制
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `getDI(index)` | 读取数字输入 | `index`: IO编号 | `int` |
| `setDO(index, value)` | 设置数字输出 | `index`: IO编号, `value`: 0/1 | `ZmotionResult` |
| `getDO(index)` | 读取数字输出 | `index`: IO编号 | `int` |
| `setInvertIn(index, invert)` | 反转输入电平 | `index`: IO编号, `invert`: 0/1 | `ZmotionResult` |
| `setInvertOut(index, invert)` | 反转输出电平 | `index`: IO编号, `invert`: 0/1 | `ZmotionResult` |

#### 5. 状态读取
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `getAxisPosition(axes)` | 获取轴位置 | `axes`: 轴列表 | `QList<double>` |
| `getAxisSpeed(axes)` | 获取轴速度 | `axes`: 轴列表 | `QList<double>` |
| `getIfIdle2(axes)` | 检查轴是否空闲 | `axes`: 轴列表 | `QList<bool>` |
| `waitForAxesIdle(axes, timeoutMs)` | 等待轴停止 | `axes`:轴列表, `timeoutMs`:超时 | `ZmotionResult` |
| `waitForAxesReachTarget(axes, targets, timeoutMs)` | 等待到位 | `axes`:轴列表, `targets`:目标 | `ZmotionResult` |

#### 6. 回原点
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setAxisSingleDatum(axis, mode, speed, creep, offset)` | 单轴回零 | `axis`:轴号, `mode`:回零模式 | `ZmotionResult` |
| `setAxisDatumOrigin(axes, origins)` | 设置原点传感器 | `axes`:轴列表, `origins`:IO列表 | `ZmotionResult` |
| `getAxisSingleHomeStatus(axis)` | 获取回零状态 | `axis`:轴号 | `ZmotionResult` |

#### 7. 运动学模式
| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `startInverseKinematics()` | 启动逆解模式 | 无 | `ZmotionResult` |
| `startForwardKinematics()` | 启动正解模式 | 无 | `ZmotionResult` |
| `setTable(start, count, values)` | 设置TABLE寄存器 | `start`:起始索引, `values`:值数组 | `ZmotionResult` |
| `getTable(start, count)` | 读取TABLE寄存器 | `start`:起始索引, `count`:数量 | `QList<double>` |

---

## 🛠️ 工具层详解

### Logger（日志系统）

**职责**：
- 提供分级日志记录（DEBUG/INFO/WARNING/ERROR）
- 支持控制台和文件双重输出
- 线程安全的日志写入

**日志级别**：
```cpp
enum LogLevel {
    LogDebug = 0,    // 调试信息（开发调试用）
    LogInfo = 1,     // 一般信息（正常运行日志）
    LogWarning = 2,  // 警告信息（需要注意）
    LogError = 3,    // 错误信息（需要处理）
};
```

**核心方法详解**：

| 方法名 | 功能说明 | 参数 | 返回值 |
|--------|----------|------|--------|
| `setLogLevel(level)` | 设置日志级别 | `level`: 日志级别 | 无 |
| `setLogFile(path)` | 设置日志文件路径 | `path`: 文件路径 | 无 |
| `setConsoleOutput(enable)` | 启用/禁用控制台输出 | `enable`: true/false | 无 |
| `debug(message)` | 输出调试日志 | `message`: 日志内容 | 无 |
| `info(message)` | 输出一般日志 | `message`: 日志内容 | 无 |
| `warning(message)` | 输出警告日志 | `message`: 日志内容 | 无 |
| `error(message)` | 输出错误日志 | `message`: 日志内容 | 无 |
| `debugf(format, ...)` | 格式化输出调试日志 | `format`: 格式化字符串 | 无 |
| `infof(format, ...)` | 格式化输出一般日志 | `format`: 格式化字符串 | 无 |
| `warningf(format, ...)` | 格式化输出警告日志 | `format`: 格式化字符串 | 无 |
| `errorf(format, ...)` | 格式化输出错误日志 | `format`: 格式化字符串 | 无 |

**日志格式**：
```
[2024-01-15 10:30:45.123] [INFO] [RobotController] 控制器已连接
[2024-01-15 10:30:46.456] [WARNING] [HomingController] J2原点传感器已触发
[2024-01-15 10:31:00.789] [ERROR] [ZmotionAPI] 连接超时
```

---

## 🚀 快速开发指南

### 添加新功能的标准步骤

假设你要添加"示教模式"功能：

#### 1️⃣ 在数据模型层添加新状态

```cpp
// RobotState.h 中添加
enum RobotStatus {
    StatusIdle = 0,
    StatusMoving = 1,
    StatusHoming = 2,
    StatusError = 3,
    StatusEmergencyStop = 4,
    StatusTeaching = 5,  // 新增示教模式状态
};
```

#### 2️⃣ 在业务逻辑层实现功能

```cpp
// RobotController.h 中添加
class RobotController : public QObject {
public:
    bool startTeachingMode();
    bool stopTeachingMode();
    bool isTeachingMode() const;
    
signals:
    void teachingModeStarted();
    void teachingModeStopped();
};
```

```cpp
// RobotController.cpp 中实现
bool RobotController::startTeachingMode() {
    if (!isConnected()) return false;
    if (m_state->getStatus() != RobotState::StatusIdle) return false;
    
    m_state->setStatus(RobotState::StatusTeaching);
    emit teachingModeStarted();
    return true;
}

bool RobotController::stopTeachingMode() {
    if (m_state->getStatus() != RobotState::StatusTeaching) return false;
    
    m_state->setStatus(RobotState::StatusIdle);
    emit teachingModeStopped();
    return true;
}
```

#### 3️⃣ 在UI层添加控制

```cpp
// MainWindowNew.cpp 中实现
void MainWindowNew::onTeachButtonClicked()
{
    if (m_robotController->isTeachingMode()) {
        m_robotController->stopTeachingMode();
        ui->btnTeach->setText("进入示教模式");
    } else {
        m_robotController->startTeachingMode();
        ui->btnTeach->setText("退出示教模式");
    }
}
```

#### 4️⃣ 在UI层连接信号

```cpp
void MainWindowNew::connectSignalsAndSlots()
{
    connect(m_robotController, &RobotController::teachingModeStarted,
            this, &MainWindowNew::onTeachingModeStarted);
    connect(m_robotController, &RobotController::teachingModeStopped,
            this, &MainWindowNew::onTeachingModeStopped);
    
    connect(ui->btnTeach, &QPushButton::clicked,
            this, &MainWindowNew::onTeachButtonClicked);
}
```

---

## 📊 信号流详解

### 场景：用户点击"回零"按钮

```
┌─────────────────────────────────────────────────────────────┐
│ 步骤1: 用户交互                                              │
│ 用户点击"回零"按钮                                           │
│ ↓                                                            │
│ MainWindowNew::onHomeButtonClicked() 被调用                  │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 步骤2: 业务逻辑                                              │
│ m_robotController->executeHoming()                          │
│ RobotController::executeHoming()                            │
│   ├─ emit operationStarted("回零")  ──┐                    │
│   ├─ m_homingController->startHoming()├─→ 串行预处理      │
│   │   └─ 并行回零...                │   J3→J2→J5          │
│   └─ emit operationCompleted()      │   6轴同时回零       │
└─────────────────────────────────────┼──────────────────────┘
          ↑                            │
          └────────────────────────────┘
          │
┌─────────────────────────────────────────────────────────────┐
│ 步骤3: 状态反馈                                              │
│ UI接收信号并更新显示                                          │
│ "正在回零..." → "回零完成"                                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 📋 常见问题解答

### Q1: 为什么要分这么多层？
**A**: 
- **提高可维护性**：修改一个层不会影响其他层
- **提高可测试性**：业务逻辑独立于UI和硬件
- **提高可复用性**：业务逻辑可以被不同的UI调用
- **支持多人开发**：不同的人可以同时开发不同的层

### Q2: 什么时候使用信号槽？
**A**: 
- UI -> 业务逻辑：UI调用方法，业务逻辑通过信号返回结果
- 硬件 -> UI：状态改变时用信号通知UI
- 避免直接的层级跨越（例如UI不应该直接调用ZmotionAPI）

### Q3: RobotState和RobotConfig有什么区别？
**A**:
| | RobotState | RobotConfig |
|---|---|---|
| 用途 | 存储实时状态 | 存储静态配置 |
| 变化频率 | 频繁变化 | 很少变化 |
| 来源 | 硬件读取 | 配置文件 |
| 谁可以修改 | 只有MotionController | RobotController和文件 |

### Q4: 如何添加日志？
**A**:
```cpp
#include "src/utils/Logger.h"

Logger::info("应用启动成功");
Logger::warning("硬件连接超时");
Logger::error("轴0运动失败");
Logger::debugf("当前位置: J0=%f, J1=%f", pos[0], pos[1]);
```

### Q5: 回零流程是怎样的？
**A**:
1. **串行预处理**：J3→J2→J5依次移开原点传感器（避免机械干涉）
2. **并行回零**：6轴同时执行回零状态机
3. 每轴状态机：检查传感器→移开原点→移开限位→启动回零→等待完成

### Q6: 如何调试脚本执行问题？
**A**:
1. 打开日志级别为DEBUG
2. 在TaskExecutor中添加详细日志
3. 检查脚本语法是否正确
4. 使用PRINT指令输出中间变量

---

## 🎯 开发流程总结

### 新手开发流程

1. **理解框架** ← 你现在在这里
2. **修改CMakeLists.txt**（将新文件添加到编译列表）
3. **编写新功能**（按照分层架构）
4. **编译和测试**
5. **调试**（查看日志）
6. **提交**

### 编译命令
```bash
cd Robot_R06
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

---

## ✅ 检查清单

在开始编码前，检查：

- [ ] 项目能正常编译吗？
- [ ] 所有的#include路径正确吗？
- [ ] CMakeLists.txt已更新吗？
- [ ] 理解了分层架构吗？
- [ ] 知道数据流向吗？
- [ ] 了解信号槽的使用吗？

---

## 🆘 遇到问题？

1. **编译错误** → 检查include路径和CMakeLists.txt
2. **链接错误** → 检查CMakeLists.txt中的target_link_libraries
3. **运行时错误** → 查看Logger输出和调试窗口
4. **通信失败** → 检查硬件连接和ZmotionAPI配置
5. **运动异常** → 检查DH参数和脉冲当量配置

---

**文档版本**: v2.0  
**更新日期**: 2026-06-13  
**适用项目**: 六轴机械臂控制软件 (Robot_R06)

---