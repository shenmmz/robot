# Robot_R06 代码架构分析文档

> 分析对象：`C:\Users\31422\Desktop\Robot_R06`
> 分析日期：2026-07-21
> 技术栈：C++17 / Qt 6（兼容 Qt 5）/ CMake / Zmotion 运动控制器 SDK

---

## 1. 项目概览

Robot_R06 是一套基于 Qt 的六轴机械臂上位机控制软件，集成了：

- **运动控制**：通过 Zmotion ZMC 系列控制器（以太网/Modbus）驱动六轴机械臂，支持关节空间（正解）与笛卡尔空间（逆解）两种控制模式。
- **舵机控制**：通过 RS485 Modbus RTU 协议驱动外部 6 路 PWM 模块（QD-6PWM-V2.0），控制 KPZ32 舵机（0–180°）。
- **脚本执行引擎**：自研类 BASIC 脚本语言，支持 MOVEJ/MOVEL/WAIT_IDLE/DELAY/IO/SERVO/CALL/循环/条件等指令，支持单步调试、暂停/恢复/停止。
- **视觉模块**：两套独立视觉链路——① `VisionController` 通过 QProcess 拉起 Python 视觉程序并以 TCP Server 接收姿态指令；② `VisionMasterBridge` 通过 QProcess 启动独立 `CS-VMC-Dome.exe`（海康 VisionMaster 方案）做视觉定位。
- **UI 层**：基于 Qt Widgets 的多页面工控界面（运行/手动编程/IO/报警/设置/RS485/视觉），含系统软键盘、代码编辑器、舵机调试组件。

---

## 2. 目录结构

```
Robot_R06/
├── CMakeLists.txt              # 根构建脚本（Qt 配置、子目录装配、DLL/方案文件拷贝、windeployqt）
├── main.cpp                    # 程序入口（QApplication、文件日志处理器、系统软键盘事件过滤器）
├── mainwindow.h/.cpp/.ui       # 主窗口类（UI 装配、信号槽、视觉指令下发、登录门禁）
├── PointAdjustDialog.ui        # 点位微调对话框 UI
├── README.md                   # 项目说明
├── ROBOT_FRAMEWORK.md          # 框架设计文档（96KB，详细设计说明）
├── rebuild.bat                 # 一键重建脚本
├── library/                    # Zmotion SDK 接口库（INTERFACE）
│   ├── CMakeLists.txt
│   ├── zmotion.h / zauxdll2.h  # 官方 C SDK 头文件
│   └── zmotion.lib / zauxdll.lib / zmotion.dll / zauxdll.dll
├── ZmotionAPI/                 # Zmotion C SDK 的 C++ 封装层（STATIC）
│   ├── CMakeLists.txt
│   ├── ZmotionAPI.h            # ZmotionAPI 类 + ZmotionResult 统一返回结构
│   └── ZmotionAPI.cpp          # 193KB，封装所有运动控制/IO/回零/状态查询 API
├── src/                        # 应用源码（按职责分层）
│   ├── CMakeLists.txt          # 子模块装配（models/hardware/logic/utils/ui/vision）
│   ├── models/                 # 数据模型层（纯数据 + 配置 + 运动学）
│   │   ├── RobotState.{h,cpp}        # 运行时状态（轴位置/速度/笛卡尔坐标/错误码）
│   │   ├── RobotConfig.{h,cpp}       # 配置模型（JSON 读写、轴参数、限位）
│   │   └── RobotKinematics.{h,cpp}   # FRAME6 运动学 TableList 构建/解析
│   ├── hardware/               # 硬件驱动层
│   │   ├── MotionController.{h,cpp}  # 连接管理、状态轮询、正逆解模式切换
│   │   ├── Rs485Controller.{h,cpp}   # Modbus RTU 通信（PWM/舵机）
│   │   └── ServoController.{h,cpp}   # 舵机角度控制（速度插值、拖拽）
│   ├── logic/                  # 业务逻辑层（核心）
│   │   ├── RobotController.{h,cpp}   # 业务核心协调器（连接/运动/模式/回零）
│   │   ├── HomingController.{h,cpp}  # 六轴回零状态机
│   │   ├── TaskExecutor.{h,cpp}      # 脚本执行引擎（队列/循环/暂停/单步）
│   │   ├── UiController.{h,cpp}      # UI 辅助逻辑（IO监控/报警/编程/设置/手动）
│   │   ├── ScriptCommand.h           # 脚本指令数据结构
│   │   └── TargetDef.h               # TARGET 点位扩展数据模型
│   ├── utils/                  # 通用工具
│   │   ├── Logger.{h,cpp}            # 文件日志系统
│   │   ├── CodeEditor.{h,cpp}        # 代码编辑器（高亮/行号）
│   │   ├── OsKEventFilter.{h,cpp}    # 系统软键盘事件过滤器
│   │   └── MathUtils.h               # 数学工具
│   ├── ui/                     # UI 组件库
│   │   └── ServoTestWidget.{h,cpp}   # 舵机调试界面组件
│   └── vision/                 # 视觉模块
│       ├── CMakeLists.txt
│       ├── VisualRoboticArm/
│       │   └── VisionController.{h,cpp}  # Python 视觉程序管理 + TCP Server
│       └── cs_vmcontrol/
│           ├── CMakeLists.txt
│           ├── VisionMasterBridge.{h,cpp}  # CS-VMC-Dome.exe 进程桥接
│           └── convert_sdk_headers.py      # SDK 头文件 GBK→UTF-8 转换工具
├── external/                   # 外部依赖（CS-VMC-Dome 视觉方案、Python venv 等）
└── build/                      # CMake 构建输出目录
```

---

## 3. 分层架构与模块职责

项目采用**经典的分层架构**，自上而下分为四层，依赖方向严格单向（上层依赖下层，下层不感知上层）：

```
┌─────────────────────────────────────────────────────────────┐
│  UI 层（MainWindow + UiController + ServoTestWidget）          │
├─────────────────────────────────────────────────────────────┤
│  业务逻辑层（RobotController / HomingController /              │
│             TaskExecutor / UiController）                      │
├─────────────────────────────────────────────────────────────┤
│  硬件驱动层（MotionController / Rs485Controller /              │
│             ServoController）                                  │
├─────────────────────────────────────────────────────────────┤
│  数据模型层（RobotState / RobotConfig / RobotKinematics）      │
├─────────────────────────────────────────────────────────────┤
│  基础设施层（ZmotionAPI → Zmotion SDK / utils / vision）       │
└─────────────────────────────────────────────────────────────┘
```

### 3.1 数据模型层（models/）

| 模块 | 职责 | 关键接口 |
|------|------|----------|
| `RobotState` | 纯数据类，存储六轴实时位置/速度/加速度、笛卡尔坐标 [X,Y,Z,A,B,C]、运行状态枚举、错误码。仅控制器可写，各模块可读。 | `setAxisPosition()` / `setCartesianPosition()` / `statusChanged()` 信号 |
| `RobotConfig` | 机械臂配置模型，从 `config/robot_config.json` 加载/保存；含轴数、各轴速度/加速度/限位、减速比、脉冲当量、回零参数。 | `loadFromJson()` / `saveToJson()` / `getAxisConfig()` |
| `RobotKinematics` | 构建/解析 Zmotion `FRAME6` 类型 `CONNFRAME` 所需的 25 项 TableList（DH 参数映射）。正逆解由控制器内部完成，PC 端只负责建模参数下发。 | `toControllerTable()` / `loadFromControllerTable()` |

**特点**：`models` 仅依赖 `Qt::Core`，不依赖任何硬件或业务模块，是依赖图的叶子节点。

### 3.2 硬件驱动层（hardware/）

| 模块 | 职责 | 关键依赖 |
|------|------|----------|
| `MotionController` | 连接管理（connect/disconnect）、状态轮询定时器（默认 100ms）、正逆解模式切换、信号转发。运动指令已下沉到 `ZmotionAPI`，本类只做连接/状态/模式。 | `RobotConfig` `RobotState` `ZmotionAPI` |
| `Rs485Controller` | 通过 Zmotion 控制器的 Modbus 转 RS485 功能，向外部 PWM 模块发送 Modbus RTU 帧（含 CRC 自动计算、RAW 透传模式、发送缓存 diff 优化）。 | `ZmotionAPI` |
| `ServoController` | 舵机角度→占空比转换（500–2500µs 脉宽），支持速度控制（100ms tick 软插值）、拖拽模式、脚本批量下发。实际通信委托 `Rs485Controller`。 | `Rs485Controller` |

**链路**：`ServoController → Rs485Controller → ZmotionAPI → 硬件 PWM 模块`

### 3.3 业务逻辑层（logic/）

| 模块 | 职责 | 关键依赖 |
|------|------|----------|
| `RobotController` | **业务核心协调器**。协调 config/state/hardware，提供高级操作：连接、建模（`initRobotModel`）、回零、安全停止、预设位置、关节/笛卡尔运动（`moveJ`/`moveL`/`jogJoint`/`jogCart`）、模式切换。 | `RobotConfig` `RobotState` `MotionController` `HomingController` `Rs485Controller` `ServoController` `ZmotionAPI` |
| `HomingController` | 六轴回零状态机：串行预处理（J3→J2→J5 移开原点）→ 并行回零（6 轴状态机 INIT→CHECK_SENSOR→…→SUCCESS/FAIL）。 | `ZmotionAPI` `RobotConfig` `RobotState` |
| `TaskExecutor` | **脚本执行引擎**。管理 `QList<ScriptCommand>` 队列，支持逐步执行、暂停（保留队列）、恢复、停止、单步（`stepOnce`）、CALL 子脚本、FOR/WHILE 循环、变量表、点位级运动参数。 | `RobotController` `ServoController` |
| `UiController` | **UI 辅助逻辑**。IO 监控（DI/DO 自定义命名）、报警日志、编程页面（脚本解析/运行/单步/点位管理）、设置页、手动点动。持有 `TaskExecutor` 实例。 | `RobotController` `TaskExecutor` |

**脚本指令体系**（`ScriptCommand.h`）：`MoveJ` / `MoveL` / `MoveOff` / `MoveLOff` / `WaitIdle` / `Delay` / `SetDO` / `WaitDI` / `ReadPos` / `Print` / `Call` / `SetVar` / `ForLoop`/`EndFor` / `WhileDI`/`WhileTrue`/`EndWhile` / `SpeedSet`/`AccelSet`/`DecelSet`/`TimeoutSet` / `IfIO`/`Else`/`EndIf` / `TargetDef` / `ServoMove`。

**点位模型**（`TargetDef.h`）：`TargetDef` 取代旧 `QList<double>`，每个 TARGET 点位可携带 6 轴坐标 + 运动参数（speed/acc/dec/tmo）+ 6 路舵机快照（`ServoSlot`）。`TargetDef.h` 独立于 UiController/TaskExecutor，避免循环 include。

### 3.4 UI 层

| 模块 | 职责 |
|------|------|
| `MainWindow` | 主窗口，负责 UI 装配（导航栏、各页面）、控制器实例化与信号槽连接、视觉指令下发（防抖+节流）、开发者登录门禁。 |
| `UiController` | 见 3.3，承担大部分 UI 交互逻辑，降低 MainWindow 复杂度。 |
| `ServoTestWidget` | 独立舵机调试弹窗组件（6 通道勾选/角度/速度/滑块），可"插入 SERVO 到编辑器"。 |
| `CodeEditor` | 代码编辑器（行号、语法高亮、当前行绿色高亮+圆点）。 |
| `OsKEventFilter` | 全局事件过滤器，触摸屏环境下 QSpinBox/QLineEdit 获焦时弹出系统软键盘。 |

### 3.5 基础设施层

| 模块 | 职责 |
|------|------|
| `ZmotionAPI` | Zmotion C SDK 的 C++ 封装（STATIC 库 `zmotion_api`）。统一返回 `ZmotionResult`，封装连接、轴参数、运动指令、IO、回零、状态查询、Modbus 透传等。 |
| `library` (zmotion_sdk) | INTERFACE 库，仅暴露官方 `zmotion.h`/`zauxdll2.h` 头文件与 `.lib` 链接。 |
| `VisionController` | 用 `QProcess` 拉起 Python 视觉程序，自身作为 `QTcpServer` 监听 5001 等待 Python 端 connect，解析 JSON `motor_commands` 下发。 |
| `VisionMasterBridge` | 用 `QProcess` 启动独立 `CS-VMC-Dome.exe`（海康 VisionMaster 方案），监控进程状态、捕获 stdout 视觉数据行。 |

---

## 4. 模块依赖关系

### 4.1 CMake 链接依赖（编译期）

```
Robot_R06 (exe)
 ├── Qt::Widgets, Qt::Network
 ├── robot_models      (STATIC)  → Qt::Core
 ├── robot_hardware    (STATIC)  → Qt::Core, Qt::SerialPort, robot_models, zmotion_sdk, zmotion_api
 ├── robot_logic       (STATIC)  → Qt::Core, Qt::Widgets, robot_models, robot_hardware, zmotion_api
 ├── robot_utils       (STATIC)  → Qt::Core, Qt::Widgets
 ├── robot_ui          (STATIC)  → Qt::Widgets, robot_hardware
 ├── robot_vision      (STATIC)  → Qt::Core, Qt::Network
 ├── vm_control        (STATIC)  → Qt::Core
 └── zmotion_sdk       (INTERFACE) → zauxdll.lib, zmotion.lib
```

**关键观察**：
- `robot_logic` 依赖 `robot_hardware` 和 `robot_models`，但不依赖 `robot_ui`/`robot_vision`——业务逻辑与 UI/视觉解耦。
- `robot_hardware` 依赖 `zmotion_api`（C++ 封装）和 `zmotion_sdk`（SDK 接口），但不依赖 `robot_logic`——硬件层不感知业务层。
- `robot_ui` 仅依赖 `robot_hardware`（ServoController），不依赖业务逻辑层，保持组件独立性。
- `robot_vision` 与 `vm_control` 相互独立，均只依赖 Qt，不依赖业务/硬件层——视觉模块通过 MainWindow 的信号槽接入。

### 4.2 运行时对象装配（实例化关系）

```
MainWindow
 ├── RobotController (业务逻辑核心)
 │    ├── RobotConfig*        (由 MainWindow 从 JSON 加载后传入)
 │    ├── RobotState*         (RobotController 内部 new)
 │    ├── MotionController*   (new, 持有 config+state)
 │    │    └── ZmotionAPI*    (MotionController 内部 new，连接硬件)
 │    ├── HomingController*   (new, 持有 ZmotionAPI+config+state)
 │    ├── Rs485Controller*    (new)
 │    │    └── setZmotionAPI(MotionController::getZmotionAPI())  ← 共享同一 ZmotionAPI 实例
 │    └── ServoController*    (new Rs485Controller)
 ├── UiController (UI 辅助逻辑)
 │    └── TaskExecutor*       (new RobotController)
 ├── VisionController*        (独立 QProcess + TCP Server)
 └── VisionMasterBridge*      (独立 QProcess 启动 CS-VMC-Dome.exe)
```

**关键设计点**：
1. **单一 ZmotionAPI 实例共享**：`Rs485Controller` 通过 `MotionController::getZmotionAPI()` 获取同一个 `ZmotionAPI` 指针，确保运动控制与 RS485 通信走同一控制器连接。
2. **RobotController 是业务中枢**：UI 层（MainWindow/UiController）只与 `RobotController` 和 `TaskExecutor` 交互，不直接触碰硬件层。
3. **视觉模块旁路接入**：`VisionController` 的 `motorCommands` 信号经 MainWindow 的 `onVisionMotorCommands` 做防抖（连续 10 次相同角度确认）+ 节流（500ms）后，调用 `RobotController::jogJoint()`/`moveJ()` 下发到运动链路——视觉不直接依赖业务逻辑类，通过 MainWindow 中转。

### 4.3 信号槽数据流

```
[硬件] ZmotionAPI → MotionController → RobotController → (信号) → MainWindow/UiController → UI 刷新
[脚本] UiController → TaskExecutor → RobotController → MotionController → ZmotionAPI → 硬件
[视觉] Python/VM → VisionController/VisionMasterBridge → MainWindow → RobotController → 硬件
[舵机] TaskExecutor/UiController → ServoController → Rs485Controller → ZmotionAPI → RS485 PWM 模块
```

---

## 5. 代码组织方式总结

### 5.1 分层 + 模块化
- 按职责严格分层（models / hardware / logic / utils / ui / vision），每层编译为独立 STATIC 库。
- 根 `CMakeLists.txt` 保持简洁，子目录通过 `add_subdirectory` 装配，符合大型项目规范。

### 5.2 依赖倒置与解耦
- 数据模型层（`models`）零业务依赖，是依赖图叶子。
- 业务逻辑层（`logic`）不依赖 UI 层，UI 逻辑集中在 `UiController` 而非散落 MainWindow。
- 视觉模块（`vision`）通过进程隔离（QProcess）+ 信号槽接入，不污染主程序二进制，规避了 VisionMaster SDK 的 ActiveX/DLL/编码耦合问题（见 `convert_sdk_headers.py` 与 `vm_control` 的注释）。

### 5.3 进程隔离策略
- **运动控制**：进程内直接调用 Zmotion SDK（低延迟、确定性）。
- **视觉**：进程外独立 exe（Python 视觉程序 / CS-VMC-Dome.exe），通过 QProcess + TCP/stdio 通信，崩溃不影响主程序。

### 5.4 脚本引擎设计
- 自研领域特定语言（DSL），`ScriptCommand` 统一指令结构，`TaskExecutor` 负责解析后执行。
- 支持运行时参数优先级：点位级 motion > 全局 SPEED/ACCEL/DECEL/TIMEOUT > 行级参数 > 自动估算。
- 单步调试、暂停/恢复、CALL 子脚本、循环/条件，具备完整调试能力。

### 5.5 配置与状态分离
- `RobotConfig`（持久化配置，JSON）与 `RobotState`（运行时状态，内存）分离，前者可保存，后者仅控制器可写。

---

## 6. 关键设计亮点与潜在改进点

### 亮点
1. **单一硬件连接共享**：Rs485 与运动控制复用同一 ZmotionAPI 实例，避免双连接冲突。
2. **视觉进程隔离**：彻底规避 SDK 耦合，提升稳定性。
3. **舵机软插值优化**：diff 缓存 + 100ms tick，解决旧版 50ms tick 阶梯抖动问题（见 ServoController.h 注释）。
4. **脚本参数优先级体系**：灵活且可预测的运动参数解析。
5. **系统软键盘适配**：工控触摸屏场景的 QSpinBox 焦点弹键盘。

### 潜在改进点
1. **MainWindow 过于庞大**：`mainwindow.cpp` 约 2966 行，承担了视觉指令下发、登录门禁、RS485 UI、位置显示等多重职责，可考虑进一步拆分（如视觉指令处理、登录模块独立）。
2. **UiController 体量偏大**：`UiController.cpp` 约 176KB，聚合了 IO 监控、报警、编程、设置、手动点动等大量逻辑，长期维护可考虑按页面拆分子控制器。
3. **RobotController 与 MainWindow 双向耦合**：MainWindow 直接持有并操作 RobotController 的各子控制器（如 `getServoController()`），部分逻辑可下沉到 RobotController 统一封装。
4. **硬编码路径**：CMake 中 `C:/Program Files/VisionMaster4.4.0/...` 为绝对路径，跨机器构建需调整。

---

## 7. 构建与运行

- **构建系统**：CMake 3.16+，C++17，Qt 6（回退 Qt 5）。
- **关键构建步骤**（根 CMakeLists.txt）：
  1. 编译 `zmotion_api` / `robot_*` 静态库。
  2. 链接 Zmotion SDK `.lib`，POST_BUILD 拷贝 `zauxdll.dll`/`zmotion.dll` 到输出目录。
  3. 拷贝 `CS-VMC-Dome.exe` 及其依赖 DLL、VisionMaster `myLibs`、视觉方案 `.sol` 文件。
  4. Qt6 下自动 `windeployqt` + 生成 `qt.conf`。
- **入口**：`main.cpp` 安装文件日志处理器（绕过 VisionMaster SDK 日志缓冲）+ 系统软键盘过滤器，创建并显示 MainWindow。

---

*本文档基于源码静态分析生成，覆盖 src/ 全部子模块、主窗口、项目管理、脚本执行引擎、运动控制链路、视觉模块及 UI/数据层。*
