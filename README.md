# Robot_R06 六轴机械臂上位机 — 项目文档

> **本文档为项目唯一入口**，合并自原 `README.md` / `ARCHITECTURE.md` / `PROJECT_FRAMEWORK.md` / `ROBOT_FRAMEWORK.md` 四份文档，
> 并按 **2026-08-31 代码状态**重新核对修订（含机械臂型号管理、视觉双链、RobotKinematics 静态构建器等近期改动）。
> 合并前的三份旧文档已废弃（内容均吸收进本文档）。

---

## 1. 项目简介

Robot_R06 是一套基于 Qt 的**六轴机械臂上位机控制软件**，面向工控触摸屏场景：

| 能力域 | 技术手段 |
|--------|----------|
| 运动控制 | Zmotion ZMC306X 控制器（以太网 TCP/IP），关节空间正解 + 笛卡尔空间逆解 |
| 舵机控制 | RS485 Modbus RTU → QD-6PWM-V2.0 模块 → 6 路 KPZ32 舵机（0–180°） |
| 脚本引擎 | 自研类 BASIC DSL，25 种指令，支持循环/条件/CALL/单步/暂停 |
| 视觉定位 | **双链并存**：① Python 视觉进程（QProcess + TCP 5001）；② 海康 VisionMaster SDK 进程内集成（ActiveX 渲染） |
| 画板轨迹 | 2D 绘图编辑器 → 轨迹采样 → 3D 坐标变换 → 机械臂笛卡尔运动 |
| 型号管理 | 机型规格注册表（代码侧单一数据源）+ 控制器 Flash 烧录型号 ID 自动识别 |
| 工控 UI | Qt Widgets 多页面界面 + 系统软键盘 + 代码编辑器 + 舵机调试 |

---

## 2. 技术栈与环境

| 项 | 值 |
|----|----|
| 语言/标准 | C++17 |
| GUI | Qt 6.9（兼容 Qt 5 回退），Widgets / SerialPort / Network / Concurrent / AxContainer |
| 构建 | CMake 3.16+，AUTOUIC/AUTOMOC/AUTORCC |
| 编译器 | MSVC（VS2022/VS2026），全局屏蔽 C4828/C4091（VM SDK GBK 头文件） |
| 运动控制器 | 正运动 ZMC306X，IP 默认 192.168.0.123 |
| 舵机总线 | RS485，QD-6PWM-V2.0 从站 0x21，9600 8N1 |
| 视觉 | 海康 VisionMaster 4.4.0 SDK（进程内）+ Python 视觉程序（进程外） |
| 编码 | 应用源码 UTF-8（含 BOM）；VM SDK 头文件 GBK（不可修改） |

---

## 3. 目录结构

```
Robot_R06/
├── CMakeLists.txt                 # 根构建脚本（全局 include、子目录装配、POST_BUILD 拷贝、windeployqt）
├── main.cpp                       # 程序入口（文件日志处理器 + 系统软键盘过滤器 + --setmodel 参数）
├── mainwindow.h/.cpp/.ui          # 主窗口（4500 行：UI 装配、信号槽中枢、视觉中转、登录门禁）
├── PointAdjustDialog.ui           # 点位微调对话框 UI（AUTOUIC 编译，不可删）
├── build_robot.bat                # 一键构建脚本（VS 生成器 + Debug）
├── rebuild.bat                    # 干净重建脚本（Ninja + Release；⚠️ vcvars 路径疑似指向 VS2022，待修）
├── setcom.zpj                     # 正运动 ZDevelop（控制器 IDE）工程文件，关联 Basic1.bas
│
├── library/                       # Zmotion SDK 接口库（INTERFACE）
│   ├── zmotion.h / zauxdll2.h     #   官方 C SDK 头文件（zauxdll2.h 约 4900 行）
│   └── zmotion.lib / zauxdll.lib / zmotion.dll / zauxdll.dll
│
├── ZmotionAPI/                    # Zmotion C SDK 的 C++ 封装（STATIC，ZmotionAPI.cpp 约 4700 行）
│   └── ZmotionAPI.h/.cpp          #   统一返回 ZmotionResult { success, errorCode, errorMessage, values }
│
├── src/                           # 应用源码（按职责分层，每层独立 STATIC 库）
│   ├── models/                    # 📊 数据模型层（依赖图叶子，只依赖 Qt::Core）
│   │   ├── RobotState.h/.cpp      #   运行时状态（轴位置/笛卡尔坐标/状态枚举/错误码）
│   │   ├── RobotConfig.h/.cpp     #   配置模型（按机型规格构建，不再读 JSON）
│   │   ├── RobotKinematics.h/.cpp #   FRAME6 TableList 构建（无状态静态构建器）
│   │   ├── RobotModelId.h         #   型号枚举（Unknown/R01/R02）+ 名称双向映射
│   │   └── RobotModels.h/.cpp     #   机型规格 + 注册表（轴齿轮链/速度/回零的唯一数据源）
│   │
│   ├── hardware/                  # 🔧 硬件驱动层
│   │   ├── MotionController.h/.cpp    #   连接管理、100ms 状态轮询、正逆解模式切换
│   │   ├── Rs485Controller.h/.cpp     #   Modbus RTU 通信（ZMC 自动组帧 + RAW 透传 + CRC16）
│   │   └── ServoController.h/.cpp     #   舵机角度控制（速度插值/拖拽/批量帧/diff 缓存）
│   │
│   ├── logic/                     # 🧠 业务逻辑层（核心）
│   │   ├── RobotController.h/.cpp     #   业务核心协调器（连接/运动/回零/建模/原点 Flash 持久化）
│   │   ├── HomingController.h/.cpp    #   六轴回零状态机（串行预处理 + 并行回零）
│   │   ├── TaskExecutor.h/.cpp        #   脚本执行引擎（队列/循环/暂停/单步/CALL）
│   │   ├── UiController.h/.cpp        #   UI 辅助逻辑（IO/报警/编程/设置/手动点动）
│   │   ├── ScriptCommand.h            #   脚本指令数据结构（25 种指令类型）
│   │   └── TargetDef.h                #   TARGET 点位扩展数据模型（坐标 + 运动参数 + 舵机快照）
│   │
│   ├── draw/                      # 🎨 画板轨迹模块
│   │   ├── drawing_window.h/.cpp      #   绘图窗口（钢笔/线/矩形/圆/弧/文字 + 轨迹采样）
│   │   └── WorkPieceCoord.h/.cpp      #   工件坐标系（三点定标 / 2D→3D 变换矩阵）
│   │
│   ├── ui/                        # 🖥️ 独立 UI 组件
│   │   └── ServoTestWidget.h/.cpp     #   舵机调试界面（6 通道滑块/角度/速度）
│   │
│   ├── utils/                     # 🛠️ 通用工具
│   │   ├── Logger.h/.cpp              #   文件日志系统
│   │   ├── CodeEditor.h/.cpp          #   代码编辑器（行号/高亮/当前行标记）
│   │   ├── OsKEventFilter.h/.cpp      #   系统软键盘事件过滤器
│   │   └── MathUtils.h                #   数学工具
│   │
│   └── vision/                    # 👁️ 视觉模块（双链并存，见 §6.6）
│       ├── VisualRoboticArm/
│       │   └── VisionController.h/.cpp   #   Python 视觉进程管理 + TCP Server（链 A）
│       └── cs_vmcontrol/              #   VM SDK 进程内集成（链 B）
│           ├── vmbase.h/.cpp              #   SDK 底层封装（方案/流程/模块/回调）
│           ├── vmcontrol.h/.cpp           #   模块控制层（命令分发/回调处理/信号转发）
│           ├── vmtypes.h                  #   视觉数据类型（枚举/结构体/事件）
│           ├── VisionRobotLink.h/.cpp     #   视觉像素→机械臂坐标转换（滤波 + 偏移）
│           └── vmwindow/VmWindow.h/.cpp/.ui # 视觉方案操作窗口（ActiveX 渲染）
│
├── external/                      # 外部依赖
│   ├── CS-VMC-Dome/               #   海康视觉独立工程源码（原则上不修改）+ 视觉方案/（.sol）
│   └── VisualRoboticArm/          #   Python 视觉程序 + .venv（2.1GB，链 A 运行时依赖）
│
├── config/developer_account.ini   # 开发者登录门禁凭证（mainwindow 读写）
├── resources/robot.rc + robot.ico # Windows 应用图标（嵌入 exe；换图用 tools/make_icon.py）
├── task/                          # 脚本任务文件（.rbt，test.rbt 为启动自动加载项）
├── tools/make_icon.py             # 图标再生成工具
├── docs/                          # 决策与评审文档（架构分层评审报告 / AxisUnits 设计 / 可删除文件清单）
├── note/ · summarize/ · images/   # 开发笔记与参考资料（不在构建链上）
└── build/                         # CMake 构建输出（VS 生成器，本机用）
```

> ⚠️ 配置文件已删除（2026-08-31）：机械臂参数全部收口到代码侧 `RobotModels.cpp`，不再有任何 JSON 配置。

---

## 4. 分层架构

### 4.1 五层总览

```
┌─────────────────────────────────────────────────────────────────────┐
│  UI 层：MainWindow · UiController* · DrawingWindow · VmWindow       │
│          ServoTestWidget · CodeEditor · OsKEventFilter              │
├─────────────────────────────────────────────────────────────────────┤
│  业务逻辑层：RobotController · TaskExecutor · HomingController ·     │
│              UiController* · ScriptCommand · TargetDef               │
├─────────────────────────────────────────────────────────────────────┤
│  硬件驱动层：MotionController · Rs485Controller · ServoController     │
├─────────────────────────────────────────────────────────────────────┤
│  数据模型层：RobotState · RobotConfig · RobotKinematics ·            │
│              RobotModelId · RobotModels（规格注册表）                │
├─────────────────────────────────────────────────────────────────────┤
│  基础设施层：ZmotionAPI → library(zmotion_sdk) · vm_control(VM SDK) │
│              · VisionController(进程外 Python) · Logger             │
├─────────────────────────────────────────────────────────────────────┤
│  外部硬件：ZMC306X(192.168.0.123) → 6 轴电机                        │
│             └→ RS485 → QD-6PWM-V2.0(0x21) → 6 路 KPZ32 舵机         │
└─────────────────────────────────────────────────────────────────────┘
```

*（`UiController` 名义在 logic 层，实际反向依赖 UI——见 §4.3 问题①）*

### 4.2 依赖规则

- **上层依赖下层**，禁止跨层访问（如 UI 不得直接调 ZmotionAPI）。
- 数据模型层 `models` 零业务依赖，是依赖图叶子；`RobotModels` 是唯一参数数据源。
- 每层编译为独立 STATIC 库：`robot_models` / `robot_hardware` / `robot_logic` / `robot_utils` / `robot_ui` / `robot_vision` / `zmotion_api` / `zmotion_sdk` / `vm_control`。
- 视觉通过**进程隔离（QProcess）+ 信号槽**接入，不污染主程序二进制。

### 4.3 已知耦合问题（详见 `docs/架构分层评审报告.md` v1.1）

| # | 问题 | 证据 | 状态 |
|---|------|------|------|
| ① | `UiController` 名义在 logic 层，反向 include `../mainwindow.h`，且 include 40+ QWidget | `UiController.cpp:7-37` 五层全穿透 | 🔴🔴 待排期 |
| ② | `MainWindow` 上帝窗口：直连 zmotion.h / zauxdll2.h / ZmotionAPI / Rs485 / Servo | `mainwindow.cpp:15-25,156,163,460…` | 🔴 大改造 |
| ③ | logic 层绕过 hardware 直连 ZmotionAPI（5 处） | `TaskExecutor.cpp:12` / `RobotController.cpp:17` / `HomingController.cpp:4` / `UiController.cpp:13` / `RobotController.h:48` | 🔴 中等 |
| ④ | 视觉双链并存，职责重叠 | 链 A `mainwindow.cpp:163`、链 B `:1110/:3971` | 🟡 **需产品决策** |

---

## 5. 机械臂型号管理（2026-08-31 落地）

### 5.1 设计思想

- 不同机型只有**参数差异**、没有**行为差异** → 用「数据 + 注册表」而非「每机型一个子类」。
- **控制器 Flash 只存型号枚举值**（小数字），完整参数在代码侧注册表，按 ID 查表（保留类型安全/版本管理）。
- 多台机械臂减速比不同 → 改 `reductionRatio`/`gearRatio` 后脉冲当量**自动重算**（不再出现"改 ratio 不生效"）。

### 5.2 关键文件

| 文件 | 内容 |
|------|------|
| `src/models/RobotModelId.h` | `enum class RobotModelId { Unknown=0, R01=1, R02=2 }` + `robotModelName()` / `robotModelFromName()` |
| `src/models/RobotModels.h/.cpp` | `AxisSpec`（每轴齿轮链+限速+传感器）、`RobotModelSpec`（整机规格）、`RobotModelRegistry::get(id)` |
| `src/models/RobotConfig.cpp` | `initializeFromModel(id)` 从注册表构建全部字段；Unknown 回退 R01 |

### 5.3 脉冲当量数据流（关键链路）

```
RobotModels.cpp buildR01() 的 s.axes（pulsePerRev / reductionRatio / gearRatio）   ← 字面数字唯一落点
   └→ RobotModelSpec::computeAxisUnits()   UNITS = pulsePerRev × reductionRatio × 齿轮比分母 / 360
        └→ RobotConfig::getAxisUnitsList()  实时调注册表推导（不缓存，改 ratio 自动生效）
             ├→ RobotController::initRobotModel()  逐轴 api->setAxisUnits + 建模
             └→ HomingController::initAxisParams()  回零时逐轴下发当量
                  └→ RobotKinematics::buildFrame6Table(axisUnits)  table[6..11] = axisUnits[i] × 360
```

R01 六轴当量（已验证与原始规格一致）：J0/J1 = 555.56、J2/J4 = 83.33、J3 = 138.89、J5 = 27.78（脉冲/度）。

### 5.4 Flash 布局与烧录

| 数据 | Flash 块 | 布局 |
|------|----------|------|
| 原点（断电保持） | 块 0 | 9 float（出厂默认 home 种子 [84.999, 68.62, -51.944, 4.229, 71, 90]） |
| **型号记录** | **块 100（独立块）** | 4 float：`[modelMagic=2026, version=1, modelId, axisCount]` |

- **烧录**：启动参数 `--setmodel R01 [--ip 192.168.0.123]` → 连上控制器一次性写 Flash 并退出（代码 DLL 路径，非 SSCOM 裸 socket）。
- **识别**：正常启动连上控制器后 `readModelFromFlash` 自动读取 → 存入 `m_modelId`；空/损坏/未知 → 默认 R01 兜底。
- Flash 有擦写寿命（约 10 万次），型号只在"写入"时烧录，不每次启动写。
- 已知边界：`m_modelId` 目前仅用于日志识别，尚未接到全量 spec 切换（R02 为空占位 spec）。

---

## 6. 核心模块详解

### 6.1 程序入口与主窗口

**`main.cpp`**
- `qInstallMessageHandler(fileMessageHandler)`：所有 qDebug 实时写入 `log/app/yyyyMMdd_HHmmss.log`（UTF-8 + BOM），stderr 输出用 `toLocal8Bit()`（GBK，匹配 Windows 控制台）。
- `OsKEventFilter` 装到 `qApp`：触摸屏场景 QSpinBox/QLineEdit 获焦自动弹出系统软键盘。
- 解析 `--setmodel` 启动参数（型号烧录模式）。

**`MainWindow`（4504 行）**
- 导航栏多页面切换（运行 / 手动编程 / IO / 报警 / 设置 / RS485 / 视觉 / 画板）。
- 控制器实例化：`RobotController` / `UiController` / `Rs485Controller` / `ServoController` / `VisionController` / `VisionRobotLink`（VmWindow 延迟创建）。
- 视觉指令中转：`onVisionMotorCommands()` 防抖（连续 10 次相同角度确认）+ 节流（500ms）。
- 画板联动：接收 `DrawingWindow` 轨迹点/圆/弧 → 切逆解模式 → `RobotController::moveL()`。
- 开发者登录门禁（`config/developer_account.ini`）、RS485 调试面板（Hex/Dec/CRC16）。

### 6.2 数据模型层 `src/models/`

| 类 | 职责 | 关键接口 |
|----|------|----------|
| `RobotState` | 运行时状态（内存），仅控制器可写 | 状态枚举 `Idle/Moving/Homing/Error/EmergencyStop/Teaching`；`setStatus()`/`statusChanged()` |
| `RobotConfig` | 配置模型，按机型构建，**不再读 JSON** | `initializeFromModel(RobotModelId)` / `getAxisUnitsList()`（实时推导）/ `getJointAxisList()` / `getVirtualAxisList()` |
| `RobotKinematics` | FRAME6 CONNFRAME 的 25 项 TableList 构建（**无状态静态构建器**） | `static buildFrame6Table(const QList<double>& axisUnits)` |
| `RobotModelId` / `RobotModels` | 型号枚举 + 规格注册表（§5） | `RobotModelRegistry::get(id)` / `computeAxisUnits()` |

**FRAME6 Table 映射**（`RobotKinematics.cpp` 硬编码几何，未进 spec）：

| 索引 | 含义 | 值 |
|------|------|-----|
| 0 | LargeZ（基座垂直高度） | 307.5 mm |
| 1 | L1（1轴到2轴 X 偏移） | -80.87 mm |
| 2 | L2（大臂长度） | 312.0 mm |
| 3 | L3（3轴到4轴中心距离） | 85.0 mm |
| 4 | L4（小臂总长） | `80.92f + 306.36f`（运行时 = 387.28；源码注释标 387.18，真实值需向机械设计确认） |
| 5 | D5（5轴转一圈时6轴转动圈数） | 0.0 |
| 6–11 | J0–J5 一圈脉冲数 | `axisUnits[i] × 360` |
| 12 | PulesVR（虚拟轴一圈脉冲） | 3600000（10000×360，固定） |
| 13–15 | SmallLX / SmallLY / SmallLZ（轴5到TCP距离） | 0 / 0 / 53.18 |
| 16–18 | InitRx / InitRy / InitRz | 0 / 0 / 0 |
| 19–24 | 预留 | 0 |

### 6.3 硬件驱动层 `src/hardware/`

| 模块 | 职责 | 关键点 |
|------|------|--------|
| `MotionController`（289 行） | 连接管理、状态轮询（100ms）、正逆解模式切换 | `connect(ip)` → `ZmotionAPI::openEth()`；`setInverseKinematicsMode()` / `setForwardKinematicsMode()` |
| `Rs485Controller`（518 行） | Modbus RTU 通信 | 两种组帧：`sendModbusRequest()`（ZMC 自动组帧，MODBUSM_REGSET）与 `sendRawModbusFrame()`（RAW 透传，PUTCHAR 逐字节 + 自算 CRC16）；`setCom(9600,8,1,0,1,14,2,800)` |
| `ServoController`（910 行） | 舵机角度控制 | `duty = 25 + angle×100/180`；50ms tick 软插值（`step = speed × 0.05`，误差 <0.5° 到位）；批量帧 `setAllPwm()` + diff 缓存；初始角度 122/113/49/109/75/80 |

**Modbus 寄存器映射**（QD-6PWM-V2.0，从站 0x21）：

| 协议 | 起始地址 | 数据格式 |
|------|----------|----------|
| 单路 PWM | 0x2251–0x2256 | 频率(3B) + 占空比(2B) + 填充(1B) |
| 批量 6 路 | 0x0246 | PWM1-4 各 5B（频率+占空比）+ PWM5-6 各 2B（仅占空比） |

### 6.4 业务逻辑层 `src/logic/`

**`RobotController`（1122 行）** — 业务核心协调器
- 初始化：`initialize(RobotModelId modelId = R01)` / `initialize(RobotConfig*)` —— 不再有 JSON 版本。
- 连接：`connectHardware(ip)` / `disconnectHardware()` / `searchControllers()`。
- 建模：`initRobotModel()` —— 物理关节轴（0-5，atype=1）→ 写 25 项 TABLE → 虚拟笛卡尔轴（6-11，atype=0）→ CONNFRAME 正逆解关联。
- 运动：`moveJ()`（正解，等待到位）/ `moveL()`（逆解，Rx/Ry/Rz ±180° 归一化）/ `jogJoint()` / `jogJointBatch()` / `jogCart()` / `moveAxis()` / `stopMotion()`。
- 回零：`executeHoming()` / `stopHoming()`（委托 HomingController）。
- **原点 Flash 持久化**：`setOriginFromCurrentPos()` 把当前 6 轴 DPOS 写 Flash（块 0）；`getOriginPosition()` Flash 优先、config 兜底。
- 安全停止：`executeSafeStop()`（减速停止 + 禁用轴）。

**`HomingController`（882 行）** — 六轴回零状态机
- **阶段 1 串行预处理**（避免机械干涉）：索引 2 → 1 → 4（机械臂第 3/2/5 轴）依次移开原点传感器。
- **阶段 2 并行回零**：6 轴同时执行状态机 `INIT → CHECK_SENSOR → MOVE_AWAY_ORIGIN/REVERSE → START_DATUM → WAIT_DATUM → SUCCESS/FAIL`，含反向限位触发回退保护。
- 硬件配置表 `g_axis_hw`（原点 IO：J0=IN0/J1=IN2/J2=IN3/J3=IN4/J4=IN6/J5=IN7，反向限位：J0=IN1/J3=IN5/J5=IN8）、回零策略 `g_home_strategy`（datumMode 3 正向 / 4 反向）。
- 轮询 200ms，回零超时 180s，移开操作超时 30s；`initAxisParams()` 逐轴下发脉冲当量（来自 `getAxisUnitsList()`）。

**`TaskExecutor`（1231 行）** — 脚本执行引擎（详见 §7）
**`UiController`（4143 行）** — IO 监控（200ms 刷新 20 路 DI/DO）、报警管理、编程页面（解析/运行/单步/点位管理）、设置页、手动点动。

### 6.5 画板 / UI 组件 / 工具

- **`DrawingWindow`（2918 行）**：FreePen / Line / Rectangle / Circle / Arc / Text（骨架化写字）/ Select / Eraser；`extractPoints()` 按 mm 分辨率采样、`extractPointsGrouped()` 多路径分组采样、`getDrawingPoints3D()` 2D→3D 变换、`DrawMotionParams` 运动参数。
- **`WorkPieceCoord`**：三点定标 `calculateFromThreePoints(p1,p2,p3)` + 4×4 齐次变换矩阵 + 姿态 Rx/Ry/Rz。
- **`ServoTestWidget`**：6 通道勾选/滑块/角度/速度，可"插入 SERVO 到编辑器"。
- **`Logger`**：分级日志（DEBUG/INFO/WARNING/ERROR），控制台 + 文件双输出。
- **`CodeEditor`**：行号、语法高亮、当前行标记。
- **`OsKEventFilter`**：触摸屏弹软键盘（TabTip/osk.exe）。

### 6.6 视觉模块 `src/vision/`（⚠️ 双链并存，需产品决策）

> 历史演进：早期 QProcess 启动 CS-VMC-Dome.exe 的方案（VisionMasterBridge）**已移除**（CMake 中 CS-VMC-Dome.exe 复制已注释禁用，注释明确"视觉定位已改用 VMControl SDK 静态库"）。当前两条链：

**链 A — Python 视觉（进程外 TCP）**
```
Python 视觉程序 (QTcpSocket client) → TCP 5001 → VisionController (QTcpServer) → MainWindow
```
- `mainwindow.cpp:163-169`：`new VisionController` + 6 个信号连接（started/stopped/errorOccurred/outputMessage/motorCommands）。
- 数据：JSON `motor_commands` 数组 → `QList<QPair<int,double>>` (axis, angle) → 防抖 + 节流后 `jogJoint()/moveJ()` 下发。
- 运行时依赖：`external/VisualRoboticArm/.venv`（2.1GB，`VisionController.cpp:57` 优先用 `.venv/Scripts/python.exe` 启动）——**不可删**。

**链 B — VM SDK 进程内集成（ActiveX 渲染）**
```
VM SDK 回调 → VMbase → VMControl →(dataReady)→ VisionRobotLink →(targetReady)→ MainWindow → RobotController
```
- `mainwindow.cpp:1110-1141` `onBtnVerifyVision()`（视觉定位按钮）：`new VmWindow()` + `show()`（ActiveX 渲染），VMControl 的 dataReady/messageReady/statusChanged 分别接 VisionRobotLink 滤波 / 日志面板 / 状态标签。
- `mainwindow.cpp:3971-3980` `initVisionAutoLoad()`：程序启动时也创建 VmWindow。
- `mainwindow.cpp:1104`：拿水流程直接 `vmControl()->executeProcedureCommand(ProcMotionType::Run, ...)` 逐帧拍摄。
- `VisionRobotLink`：预热丢弃（3 帧）+ 移动平均（3 帧）+ 固定偏移映射，输出 `targetReady(x,y)`。
- 视觉方案文件：`external/CS-VMC-Dome/视觉方案/水瓶识别.sol`（POST_BUILD 复制到输出目录）。

**处置建议**：删除任一链前必须先做产品决策；架构上可抽 `IVisionProvider` 接口统一两链（见 `docs/架构分层评审报告.md` §5.4）。

---

## 7. 脚本引擎

### 7.1 支持的 25 种指令（`ScriptCommand.h`）

```
运动类:   MoveJ / MoveL / MoveOff / MoveLOff / WaitIdle
IO 类:    SetDO / WaitDI / IfIO / Else / EndIf
控制流:   ForLoop / EndFor / WhileDI / WhileTrue / EndWhile
参数:     SpeedSet / AccelSet / DecelSet / TimeoutSet
变量/IO:  SetVar / ReadPos / Print / Call
点位:     TargetDef
舵机:     ServoMove（ch=ang:spd 或 ch=ang:Tms 多路格式）
延时:     Delay
```

### 7.2 脚本格式示例

```
TARGET P1 = {J0: 0, J1: 90, J2: -45, J3: 0, J4: 45, J5: 0}
SPEED(30)
ACCEL(80)
MOVEJ(P1, speed=25, acc=50, dec=60)
MOVEL({100, 200, 150, 0, 0, 0}, speed=50)
WAIT_IDLE()
DELAY(500)
SET_DO(0, 1)
WAIT_DI(1, 1, timeout=5000)
FOR(i, 1, 10, 1)
    MOVEJ(P1)
    MOVEJ(HOME)
END_FOR
IF(DI, 2, 1)
    PRINT("Sensor triggered")
ELSE
    PRINT("Sensor not triggered")
END_IF
SERVO(0=90:10, 1=45:5, 2=120:30)
CALL("sub_task.rbt")
x = 100
MOVEOFF(0, x+50, 0, 0, 0, 0)
```

### 7.3 运动参数优先级

```
点位级 motion (TargetDef.motion)  ← 最高
  ↓ 未设时
全局 SPEED/ACCEL/DECEL/TIMEOUT
  ↓ 未设时
行级 speed=/acc=/dec= 参数
  ↓ 未设时
默认值 (25/50/60/10000)
```

执行引擎：`QList<ScriptCommand>` + 递归 `executeNext()`（QTimer::singleShot(0) 异步推进）；`buildLoopMap()` 栈式预处理跳转表；变量表 `QHash<QString,double>` + `evaluateExpr()`；单步 `stepOnce()`；CALL 递归加载 .rbt 并保存/恢复队列索引。

---

## 8. 对象装配与信号槽数据流

```
MainWindow
 ├── RobotController (业务核心)
 │    ├── RobotConfig*          ← initializeFromModel(RobotModelId) 构建
 │    ├── RobotState*           ← RobotController 内部 new
 │    ├── MotionController*     ← new, 持有 config + state
 │    │    └── ZmotionAPI*      ← MotionController 内部 new（唯一实例）
 │    ├── HomingController*     ← new, 持有 ZmotionAPI + config + state
 │    ├── Rs485Controller*      ← new, setZmotionAPI(共享同一实例)
 │    └── ServoController*      ← new(Rs485Controller*)
 ├── UiController → TaskExecutor*（new(RobotController*)）
 ├── VisionController*          ← 链 A：QProcess + TCP 5001
 ├── VisionRobotLink*           ← 链 B：视觉坐标转换（VmWindow 延迟创建）
 └── DrawingWindow* / ServoTestWidget*（非模态）
```

| 数据流 | 路径 |
|--------|------|
| 硬件状态 | ZmotionAPI → MotionController → RobotController →(信号)→ MainWindow/UiController → UI |
| 脚本执行 | UiController → TaskExecutor → RobotController → MotionController → ZmotionAPI → 硬件 |
| 视觉定位 A | Python → TCP 5001 → VisionController →(motorCommands)→ MainWindow(防抖+节流) → RobotController |
| 视觉定位 B | VM SDK → VMbase → VMControl →(dataReady)→ VisionRobotLink →(targetReady)→ MainWindow → RobotController |
| 画板轨迹 | DrawingWindow →(sendDrawingPoints)→ MainWindow(切逆解) → RobotController::moveL() |
| 舵机控制 | TaskExecutor/UiController → ServoController → Rs485Controller → ZmotionAPI → RS485 → PWM 模块 |

---

## 9. 硬件通信链路

```
┌──────────┐   TCP/IP    ┌───────────────┐
│ 上位机    │────────────→│ ZMC306X       │
│ Robot_R06 │ 192.168.0.x │ 控制器         │
└──────────┘             └───┬───────┬───┘
                    脉冲/方向 │       │ RS485
                             ▼       ▼
                    ┌────────────┐  ┌──────────────┐
                    │ 6 轴步进    │  │ QD-6PWM-V2.0 │
                    │ /伺服电机   │  │ 从站 0x21    │
                    │ J0~J5      │  │ 9600 8N1     │
                    └────────────┘  └──────┬───────┘
                                           │ PWM
                                           ▼
                                    ┌──────────────┐
                                    │ 6 路 KPZ32   │
                                    │ 舵机 0°~180°  │
                                    └──────────────┘
```

- **运动控制**：进程内直接调用 Zmotion SDK（低延迟、确定性）。
- **RS485 通信**：通过 ZMC 控制器 Modbus 透传（ZMC 做 Modbus 主端）。
- **视觉**：进程外 Python（链 A）+ VM SDK 进程内（链 B），见 §6.6。

---

## 10. 构建与运行

### 10.1 本机一键构建

```bat
build_robot.bat   :: 加载 VS 环境 → cmake -B build → cmake --build --config Debug（VS/MSBuild 生成器）
rebuild.bat       :: 删 build/ → Ninja + Release 全量重建（⚠️ 内部 vcvars64.bat 指向 VS2022，若机器只有 VS2026 需先修路径）
```

### 10.2 CMake 结构要点

- 根 `CMakeLists.txt`：`include_directories(${CMAKE_SOURCE_DIR})`（项目普遍用根相对 include，如 `#include "src/models/RobotModelId.h"`）→ `add_subdirectory(library / ZmotionAPI / src)` → `qt_add_executable(Robot_R06)`。
- 链接：`Qt::Widgets/Network/Concurrent/AxContainer` + `robot_models/hardware/logic/utils/ui/vision` + `vm_control` + `zmotion_sdk`。
- **POST_BUILD 拷贝**：`zauxdll.dll`/`zmotion.dll`/`zauxdll.lib`/`zmotion.lib`（Zmotion SDK）、`my_cross_platform.dll`（VM SDK bin）、`myLibs/`（VM ActiveX 依赖，来自 `C:/Program Files/VisionMaster4.4.0/Applications`）、`视觉方案/`（.sol）。CS-VMC-Dome.exe 复制已注释禁用。
- Windows + Qt6 自动 `windeployqt`（`--no-translations`）。
- 图标：`resources/robot.rc` 嵌入 `robot.ico`（换图用 `tools/make_icon.py`，源图 `config/robot.jpeg`）。

### 10.3 沙箱/命令行验证路径（MSBuild 崩溃时的替代）

```powershell
Import-Module "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath "C:\Program Files\Microsoft Visual Studio\18\Community" -DevCmdArguments "-arch=x64 -host_arch=x64"
cmake -B build_ninja -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:\Qt\6.9.0\msvc2022_64"
cmake --build build_ninja --target Robot_R06
```

---

## 11. 代码量统计（2026-08-31）

| 文件 | 行数 | 职责 |
|------|------|------|
| ZmotionAPI.cpp | 4739 | SDK 封装（全量 API） |
| mainwindow.cpp | 4504 | 主窗口（UI 装配/信号槽/视觉中转） |
| UiController.cpp | 4143 | UI 辅助逻辑（IO/报警/编程/设置/手动） |
| drawing_window.cpp | 2918 | 画板轨迹编辑器 |
| TaskExecutor.cpp | 1231 | 脚本执行引擎 |
| RobotController.cpp | 1122 | 业务核心协调器 |
| vmbase.cpp | 993 | VM SDK 底层封装 |
| ServoController.cpp | 910 | 舵机控制 |
| HomingController.cpp | 882 | 回零状态机 |
| vmcontrol.cpp | 871 | VM 模块控制层 |
| VmWindow.cpp | 613 | 视觉方案操作窗口 |
| Rs485Controller.cpp | 518 | RS485 Modbus 通信 |
| VisionController.cpp | 422 | Python 视觉进程管理 |
| MotionController.cpp | 289 | 连接/轮询/模式切换 |
| 应用源码合计 | **≈ 3.1 万行** | （不含 SDK 头文件与 external/） |

---

## 12. 开发指南

### 添加新功能的流程

1. 数据模型层加数据 → 2. 业务逻辑层实现 → 3. 硬件驱动层接 API → 4. UI 层加控件与信号槽 → 5. 新文件加入 CMake → 6. 编译验证。

### 常用检查清单

- [ ] include 路径正确（根相对 include 全局可用）
- [ ] 遵循分层（不要 UI → ZmotionAPI 直连）
- [ ] 新源文件加入 `PROJECT_SOURCES` / 对应 STATIC 库
- [ ] 添加 qDebug 日志（会实时落盘）
- [ ] 改机械臂参数 → 改 `RobotModels.cpp`（**不要**搜常量到处改）

### 常见问题

| 问题 | 答案 |
|------|------|
| 编译失败 undefined reference？ | CMake 里少了源文件，检查目标链接 |
| 硬件连接失败？ | 检查 ZMC306X IP（默认 192.168.0.123） |
| 想改某轴减速比/齿轮比？ | 改 `RobotModels.cpp` `buildR01()` 对应 `s.axes` 行，当量自动重算 |
| 想改 FRAME6 几何？ | `RobotKinematics.cpp` `buildFrame6Table()` 内硬编码常量（真实 L4 需向机械设计确认） |
| 视觉定位用哪条链？ | 链 B（VM SDK，视觉定位按钮）；链 A（Python，启动/停止视觉程序）——见 §6.6 |
| 自定义原点怎么保存？ | 点动到目标位姿 → `setOriginFromCurrentPos()` 写 Flash，断电不丢 |

---

## 13. 附：文档修订记录

| 日期 | 说明 |
|------|------|
| 2026-08-31 | **四合一**：README/ARCHITECTURE/PROJECT_FRAMEWORK/ROBOT_FRAMEWORK 合并为本文档，按当前代码修订（型号管理、视觉双链、RobotKinematics 静态构建器、去 JSON、原点 Flash 持久化等）。 |
| 2026-08-22 | PROJECT_FRAMEWORK.md 生成（本版已废弃，内容并入本文档 §6/§7/§9）。 |
| 2026-07-21 | ARCHITECTURE.md 生成（已废弃）。 |
| 2026-06-13 | ROBOT_FRAMEWORK.md v2.0（已废弃，接口大量过时）。 |

> 专项决策文档保留在 `docs/`：`架构分层评审报告.md`（耦合问题 v1.1）、`AxisUnits数据链路重构设计.md`、`可删除文件清单.md`。
