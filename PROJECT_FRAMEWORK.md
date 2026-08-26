# Robot_R06 项目框架全景文档

> **生成日期**：2026-08-22
> **技术栈**：C++17 / Qt 6（兼容 Qt 5）/ CMake / Zmotion 运动控制器 SDK / 海康 VisionMaster SDK
> **代码总量**：~23,000 行核心源代码（15 个 .cpp + 25 个 .h + 3 个 .ui）

---

## 1. 项目定位

Robot_R06 是一套基于 Qt 的 **六轴机械臂上位机控制软件**，集成：

| 能力域 | 技术手段 |
|--------|----------|
| 运动控制 | Zmotion ZMC 系列控制器（以太网 TCP/IP），关节空间正解 + 笛卡尔空间逆解 |
| 舵机控制 | RS485 Modbus RTU → QD-6PWM-V2.0 模块 → 6 路 KPZ32 舵机（0–180°） |
| 脚本引擎 | 自研类 BASIC DSL，25 种指令，支持循环/条件/CALL/单步/暂停 |
| 视觉定位 | 海康 VisionMaster SDK 静态库集成（进程内） + Python 视觉程序（进程外 TCP） |
| 画板轨迹 | 2D 绘图编辑器 → 轨迹采样 → 3D 坐标变换 → 机械臂笛卡尔运动 |
| 工控 UI | Qt Widgets 多页面界面 + 系统软键盘 + 代码编辑器 + 舵机调试 |

---

## 2. 分层架构

```
┌──────────────────────────────────────────────────────────────────────────┐
│                          UI 层 (Layer 1)                                  │
│  MainWindow · UiController · DrawingWindow · VmWindow · ServoTestWidget │
│  CodeEditor · OsKEventFilter                                            │
├──────────────────────────────────────────────────────────────────────────┤
│                       业务逻辑层 (Layer 2)                                │
│  RobotController · TaskExecutor · HomingController · UiController       │
│  ScriptCommand · TargetDef · VisionRobotLink                             │
├──────────────────────────────────────────────────────────────────────────┤
│                       硬件驱动层 (Layer 3)                                │
│  MotionController · Rs485Controller · ServoController                    │
├──────────────────────────────────────────────────────────────────────────┤
│                       数据模型层 (Layer 4)                                │
│  RobotState · RobotConfig · RobotKinematics · WorkPieceCoord            │
├──────────────────────────────────────────────────────────────────────────┤
│                       基础设施层 (Layer 5)                                 │
│  ZmotionAPI (SDK 封装) · VMbase/VMControl (视觉 SDK 封装) · Logger       │
│  VisionController (Python 视觉进程管理)                                  │
├──────────────────────────────────────────────────────────────────────────┤
│                       外部硬件                                           │
│  ZMC306X 控制器 (192.168.0.123) → 6 轴步进/伺服电机                     │
│                          └→ RS485 → QD-6PWM-V2.0 → 6 路 KPZ32 舵机       │
└──────────────────────────────────────────────────────────────────────────┘
```

**依赖规则**：上层依赖下层，禁止跨层访问（如 UI 不得直接调 ZmotionAPI）。

---

## 3. 目录结构

```
Robot_R06/
├── CMakeLists.txt                    # 根构建脚本
├── main.cpp                          # 程序入口（文件日志 + 软键盘过滤器）
├── mainwindow.h/.cpp/.ui             # 主窗口（UI 装配 + 信号槽 + 视觉中转）
├── PointAdjustDialog.ui              # 点位微调对话框
├── rebuild.bat                       # 一键重建脚本
│
├── config/                           # 配置文件
│   ├── robot_config.json             #   机械臂参数（轴/速度/限位/IP）
│   └── developer_account.ini         #   开发者账号配置
│
├── task/                             # 脚本任务文件（.rbt）
│   ├── test.rbt / 1.rbt / servo.rbt ...
│
├── note/                             # 开发笔记
│   ├── R06脚本指令参考.md
│   ├── 回原点逻辑说明.md
│   ├── 编程页功能规划.md
│   └── CS-VMC-Dome分析与集成方案.md ...
│
├── library/                          # Zmotion SDK 接口库（INTERFACE）
│   ├── zmotion.h / zauxdll2.h        #   官方 C SDK 头文件
│   └── zmotion.lib / zauxdll.lib / *.dll
│
├── ZmotionAPI/                        # Zmotion C++ 封装层（STATIC, 4470 行）
│   ├── ZmotionAPI.h/.cpp             #   统一 ZmotionResult 返回结构
│   └── CMakeLists.txt
│
├── src/                               # 应用源码（按职责分层）
│   ├── CMakeLists.txt                #   子模块装配
│   │
│   ├── models/                       # 📊 数据模型层（纯数据 + 配置 + 运动学）
│   │   ├── RobotState.h/.cpp         #   运行时状态（轴位置/速度/笛卡尔/错误码）
│   │   ├── RobotConfig.h/.cpp        #   配置模型（JSON 读写/轴参数/限位）
│   │   └── RobotKinematics.h/.cpp    #   FRAME6 DH 参数 TableList 构建/解析
│   │
│   ├── hardware/                     # 🔧 硬件驱动层
│   │   ├── MotionController.h/.cpp   #   连接管理/状态轮询/正逆解模式切换
│   │   ├── Rs485Controller.h/.cpp     #   Modbus RTU 通信（ZMC 自动组帧 + RAW 透传）
│   │   └── ServoController.h/.cpp    #   舵机角度控制（速度插值/拖拽/批量下发）
│   │
│   ├── logic/                        # 🧠 业务逻辑层（核心）
│   │   ├── RobotController.h/.cpp    #   业务核心协调器（连接/运动/回零/建模）
│   │   ├── HomingController.h/.cpp   #   六轴回零状态机（串行预处理 + 并行回零）
│   │   ├── TaskExecutor.h/.cpp       #   脚本执行引擎（队列/循环/暂停/单步/CALL）
│   │   ├── UiController.h/.cpp       #   UI 辅助逻辑（IO/报警/编程/设置/手动）
│   │   ├── ScriptCommand.h           #   脚本指令数据结构（25 种指令类型）
│   │   └── TargetDef.h               #   TARGET 点位扩展数据模型
│   │
│   ├── draw/                         # 🎨 画板轨迹模块
│   │   ├── drawing_window.h/.cpp     #   绘图窗口（工具栏/画布/3D 变换/发送轨迹）
│   │   └── WorkPieceCoord.h/.cpp     #   工件坐标系（三点定标/2D→3D 变换矩阵）
│   │
│   ├── ui/                           # 🖥️ 独立 UI 组件
│   │   └── ServoTestWidget.h/.cpp    #   舵机调试界面（6 通道滑块/角度/速度）
│   │
│   ├── utils/                        # 🛠️ 通用工具
│   │   ├── Logger.h/.cpp             #   文件日志系统（DEBUG/INFO/WARNING/ERROR）
│   │   ├── CodeEditor.h/.cpp         #   代码编辑器（行号/高亮/当前行标记）
│   │   ├── OsKEventFilter.h/.cpp     #   系统软键盘事件过滤器
│   │   └── MathUtils.h               #   数学工具函数
│   │
│   └── vision/                       # 👁️ 视觉模块
│       ├── VisualRoboticArm/
│       │   └── VisionController.h/.cpp  # Python 视觉进程管理 + TCP Server
│       └── cs_vmcontrol/              # VisionMaster SDK 静态库集成
│           ├── vmbase.h/.cpp         #   SDK 底层封装（方案/流程/模块/回调）
│           ├── vmcontrol.h/.cpp      #   模块控制层（命令分发/回调处理/信号转发）
│           ├── vmtypes.h             #   视觉数据类型定义（枚举/结构体/事件）
│           ├── VisionRobotLink.h/.cpp #   视觉像素→机械臂坐标转换器（滤波+偏移）
│           └── vmwindow/
│               ├── VmWindow.h/.cpp/.ui # 视觉方案操作窗口（ActiveX 渲染 + 操作按钮）
│
├── external/                          # 外部依赖（CS-VMC-Dome 源码/视觉方案/SDK）
│   ├── CS-VMC-Dome/                   #   （不再修改的独立视觉工具项目）
│   └── ...
│
├── build/                             # CMake 构建输出
└── docs/                              # 文档与图片资源
```

---

## 4. 核心模块详解

### 4.1 程序入口 — `main.cpp`

| 职责 | 实现 |
|------|------|
| 文件日志 | `qInstallMessageHandler(fileMessageHandler)` — 绕过 VM SDK 缓冲，实时写入 `log/app/yyyyMMdd_HHmmss.log`（UTF-8 + BOM），同时输出到 stderr（GBK） |
| 软键盘 | `OsKEventFilter` 装到 `qApp`，触摸屏场景 QSpinBox/QLineEdit 获焦自动弹 TabTip/osk.exe |
| 主窗口 | 创建并显示 `MainWindow`，启动 Qt 事件循环 |

### 4.2 主窗口 — `MainWindow`（3882 行）

| 职责 | 关键方法 |
|------|----------|
| UI 装配 | 导航栏多页面切换（运行/手动编程/IO/报警/设置/RS485/视觉/画板） |
| 控制器实例化 | 创建 `RobotController` / `UiController` / `Rs485Controller` / `ServoController` / `VisionController` / `VmWindow` |
| 信号槽中枢 | 硬件连接回调、建模回调、视觉指令中转、画板轨迹接收 |
| 视觉运动 | `onVisionMotorCommands()` — 防抖（10 次确认）+ 节流（500ms）+ 标准差过滤 + 死区过滤 |
| 画板联动 | 接收 `DrawingWindow` 的轨迹点/圆/弧，切换逆解模式，调用 `RobotController::moveL()` 执行 |
| 登录门禁 | 开发者账号验证（`checkLoginGate()`），限制未登录访问高级功能 |
| RS485 调试 | 手动发送/接收、定时发送、Hex/Dec 互转、CRC16 计算 |

### 4.3 业务逻辑层 — `src/logic/`

#### 4.3.1 `RobotController`（986 行）— 业务核心协调器

| 能力 | 方法 | 说明 |
|------|------|------|
| 初始化 | `initialize()` / `initRobotModel()` | 创建子控制器，设置 DH 参数 TABLE[0-24]，配置虚拟笛卡尔轴 |
| 连接 | `connectHardware(ip)` / `disconnectHardware()` | TCP/IP 连接 ZMC306X |
| 关节运动 | `moveJ(joints, spd, acc, dec, tmo)` | 正解模式，6 轴直线插补 + 等待到位 |
| 笛卡尔运动 | `moveL(cart, spd, acc, dec, tmo)` | 逆解模式，虚拟轴运动 + 角度归一化 |
| 关节点动 | `jogJoint(axis, angle, ...)` | 不等待到位的快速点动 |
| 笛卡尔点动 | `jogCart(cart, ...)` | 笛卡尔空间点动 |
| 回零 | `executeHoming()` / `stopHoming()` | 委托 HomingController |
| 安全停止 | `executeSafeStop()` / `stopMotion()` | 急停（模式 2 立即停止） |
| 预设位置 | `moveToHomePosition()` 等 | 回零后移动到安全位姿 |

#### 4.3.2 `TaskExecutor`（1206 行）— 脚本执行引擎

| 能力 | 实现 |
|------|------|
| 指令队列 | `QList<ScriptCommand>` + 递归 `executeNext()`（QTimer::singleShot(0) 异步推进） |
| 控制流 | `buildLoopMap()` 栈式预处理 FOR/ENDFOR、WHILE/ENDWHILE、IF/ELSE/ENDIF 跳转表 |
| 变量系统 | `QHash<QString,double> m_variables` + `evaluateExpr()` 支持 ±*/表达式 |
| 运动参数优先级 | 点位级 motion > 全局 SPEED/ACCEL/DECEL/TIMEOUT > 行级参数 > 默认值 |
| 暂停/恢复 | `pause()` 停止运动并等待轴停 → `resume()` 根据下一指令设置运动学模式 |
| 单步 | `stepOnce()` 执行一条后进入 Paused 状态 |
| CALL 子脚本 | 递归加载 .rbt 文件，保存/恢复当前队列索引 |
| 舵机指令 | `ServoMove` — 解析 `ch=ang:spd` 或 `ch=ang:Tms` 多路格式 |

**支持的 25 种脚本指令**：

```
运动类:   MoveJ / MoveL / MoveOff / MoveLOff / WaitIdle
IO类:     SetDO / WaitDI / IfIO / Else / EndIf
控制流:   ForLoop / EndFor / WhileDI / WhileTrue / EndWhile
参数设定: SpeedSet / AccelSet / DecelSet / TimeoutSet
变量/IO:  SetVar / ReadPos / Print / Call
点位定义: TargetDef
舵机:     ServoMove
延时:     Delay
```

#### 4.3.3 `HomingController`（882 行）— 六轴回零状态机

**两阶段设计**：

```
阶段 1: 串行预处理（避免机械干涉）
  J2 → J1 → J4 依次移开原点传感器
  每轴状态机: INIT → CHECK_SENSOR → MOVE_AWAY → WAIT → START_DATUM → WAIT → SUCCESS

阶段 2: 并行回零（6 轴同时，含反向限位保护）
  全轴状态机: INIT → CHECK_SENSOR → MOVE_AWAY_ORIGIN/REVERSE → START_DATUM → WAIT_DATUM
              ↕ 反向限位触发 → 取消回零 → 回退 MOVE_AWAY_REVERSE → 重试
```

| 硬件配置 | 原点 IO | 反向限位 IO | 回零模式 |
|----------|---------|-------------|----------|
| J0 | IN0 | IN1 | datumMode=3（正向限位回零） |
| J1 | IN2 | — | datumMode=4（反向限位回零） |
| J2 | IN3 | — | datumMode=3 |
| J3 | IN4 | IN5 | datumMode=3 |
| J4 | IN6 | — | datumMode=3 |
| J5 | IN7 | IN8 | datumMode=3 |

轮询定时器 200ms，回零超时 180s，移开操作超时 30s。

#### 4.3.4 `UiController`（3987 行）— UI 辅助逻辑

| 功能区 | 职责 |
|--------|------|
| IO 监控 | 200ms 定时器刷新 20 路 DI/DO 状态，自定义命名 LED 显示 |
| 报警管理 | 接收 RobotController/hardware 错误信号，写入报警表，支持清除 |
| 编程页面 | 脚本解析(`parseScript`)、运行/停止/单步、插入各种指令、点位管理、文件保存/加载 |
| 设置页面 | 从 RobotConfig 读取/保存参数到 UI |
| 手动点动 | `onJogJoint(axis, positive)` / `onJogCart(index, positive)` — 读取步长计算目标 |
| TaskExecutor 回调 | 进度更新、暂停弹窗、完成/错误处理 |
| 舵机交互 | 脚本中 SERVO 指令的插入与执行 |

### 4.4 硬件驱动层 — `src/hardware/`

#### 4.4.1 `MotionController`（289 行）

| 职责 | 实现 |
|------|------|
| 连接管理 | `connect(ip)` → `ZmotionAPI::openEth()`，`disconnect()` → `close()` |
| 状态轮询 | 100ms 定时器 → 读取 6 轴 DPOS/MPEED/笛卡尔坐标 → 更新 RobotState |
| 模式切换 | `setInverseKinematicsMode()` / `setForwardKinematicsMode()` — Zmotion CONNFRAME 配置 |
| 信号转发 | connected/disconnected/motionCompleted/hardwareError → RobotController |

#### 4.4.2 `Rs485Controller`（518 行）

| 模式 | 方法 | 组帧方 | CRC |
|------|------|--------|-----|
| ZMC 自动组帧 | `sendModbusRequest()` → `MODBUSM_REGSET` | ZMC 硬件 | ZMC 自动 |
| RAW 透传 | `sendRawModbusFrame()` → `PUTCHAR` 逐字节 | 手动拼帧 | `calcModbusCRC16()` |

**Modbus 寄存器映射**（QD-6PWM-V2.0，从站 0x21，9600/8/N/1）：

| 协议 | 起始地址 | 数据格式 |
|------|----------|----------|
| 单路 PWM | 0x2251–0x2256 | 频率(3B) + 占空比(2B) + 填充(1B) |
| 批量 6 路 | 0x0246 | PWM1-4 各 5B(频率+占空比) + PWM5-6 各 2B(仅占空比) |

#### 4.4.3 `ServoController`（910 行）

| 机制 | 说明 |
|------|------|
| 角度→占空比 | `duty = 25 + (angle / 180) × 100`（0°=500µs→25‰, 90°=1500µs→75‰, 180°=2500µs→125‰） |
| 速度插值 | 50ms 定时器 tick，`step = speed × 0.05`，误差 < 0.5° 到达目标 |
| 批量发送 | 每个 tick 收集 6 路角度 → 一次 `setAllPwm()` 批量帧（避免逐路发送延迟） |
| 拖拽模式 | `setAngleDrag()` 直接更新状态，由定时器统一批量发送 |
| diff 缓存 | 记录上次各通道占空比，跳过未变化通道（减少总线负载） |
| 初始位置 | PWM1=122°, PWM2=113°, PWM3=49°, PWM4=109°, PWM5=75°, PWM6=80° |

### 4.5 数据模型层 — `src/models/`

| 类 | 职责 | 关键数据 |
|----|------|----------|
| `RobotState` | 运行时状态（内存） | 6 轴位置/速度、笛卡尔 [X,Y,Z,Rx,Ry,Rz]、状态枚举、错误码 |
| `RobotConfig` | 持久化配置（JSON） | 控制器 IP、轴数、各轴速度/加速度/限位、脉冲当量、关节轴/虚拟轴列表 |
| `RobotKinematics` | DH 参数建模 | 25 项 TABLE 数组（LargeZ/L1-L6/脉冲数），`toControllerTable()` / `fromControllerTable()` |

**RobotStatus 状态机**：Idle → Moving → Homing → Error → EmergencyStop → Teaching

### 4.6 画板轨迹模块 — `src/draw/`

#### 4.6.1 `DrawingWindow`（2918 行）— 2D 绘图编辑器

| 工具 | 功能 |
|------|------|
| FreePen | 自由钢笔（鼠标轨迹采样为 QPainterPath） |
| Line / Rectangle | 直线 / 矩形 |
| Circle | 圆形（圆心+半径，执行时用圆弧插补指令） |
| Arc | 三点弧（起点+经过点+终点） |
| Text | 文字轨迹（支持骨架化→单线轨迹，用于写字） |
| Select | 选择/移动/复制/粘贴 |
| Eraser | 橡皮擦（区域擦除） |

| 核心功能 | 方法 |
|----------|------|
| 轨迹采样 | `extractPoints(resolution)` — 按 mm 分辨率从 QPainterPath 采样点列 |
| 分组采样 | `extractPointsGrouped()` — 多路径分别采样（支持抬笔/落笔） |
| 3D 变换 | `getDrawingPoints3D()` — 2D 坐标 × WorkPieceCoord 变换矩阵 → 3D 空间坐标 |
| 圆弧提取 | `getDrawingCircles3D()` / `getDrawingArcs3D()` — 原始几何参数传递 |
| 运动参数 | `DrawMotionParams` — 速度/加速度/S 曲线/拐角过渡/工具姿态 Rx,Ry,Rz |
| 信号发送 | `sendDrawingPoints(pointGroups, rx, ry, rz)` → MainWindow → RobotController::moveL() |
| 视图操作 | 鼠标滚轮缩放、平移、撤销/重做、网格、工件平面显示 |

#### 4.6.2 `WorkPieceCoord` — 工件坐标系

| 能力 | 实现 |
|------|------|
| 三点定标 | `calculateFromThreePoints(p1, p2, p3)` — p1=原点, p2=X 轴方向, p3=XY 平面 |
| 坐标变换 | `transform2DTo3D(x, y, z=0)` — 2D 工件坐标 → 3D 机械臂坐标 |
| 变换矩阵 | `getTransformationMatrix()` — 4×4 齐次变换矩阵 |
| 姿态 | 保存 Rx/Ry/Rz（角度归一化到 ±180°），传递给 DrawMotionParams |

### 4.7 视觉模块 — `src/vision/`

项目有**两套独立视觉链路**，可按需启用：

#### 4.7.1 VisionMaster SDK 静态库集成（进程内，当前主用）

```
MainWindow → VmWindow → VMControl → VMbase → VisionMaster SDK
                                       ↓ 信号
                            VisionRobotLink → targetReady(x, y) → MainWindow
```

| 层级 | 类 | 职责 |
|------|----|------|
| 底层 | `VMbase`（993 行） | 直接对接 SDK 接口：方案加载/保存/关闭/运行，流程执行/停止/结果读取，模块参数页句柄，SDK 回调提取（`OnWorkBeginStatusCallBack` / `OnWorkEndStatusCallBack`） |
| 控制 | `VMControl`（871 行） | 命令分发层：方案级命令(Load/Save/Run/Stop)、流程级命令(Run/Runing/Stop)、模块命令(Set/GroupSet)，回调处理，超时定时器，连续采集模式管理 |
| 窗口 | `VmWindow`（613 行） | UI 封装：方案加载/执行/停止按钮，流程下拉框，ActiveX 渲染控件显示，数据窗口，参数页按钮 |
| 转换 | `VisionRobotLink`（163 行） | 视觉像素→机械臂坐标：预热丢弃(3 帧) + 移动平均(3 帧) + 固定偏移映射，输出 `targetReady(x, y)` 信号 |
| 类型 | `vmtypes.h` | 枚举(SolMotionType/ProcMotionType/ModuMotionType/VisionStatus/VisionDataType) + 结构体(PointData/TargetData/Vision*Event) |

**数据流**：VM SDK 回调 → VMbase::processEndCallBack → VMControl::proEndReceive → 按 DataType 分发：
- TargetData → `hanleTargetData()` → `dataReady(TargetData)` 
- PointData → `hanlePointData()` → `dataReady(PointData)`
- → VmWindow 显示 / VisionRobotLink 转换坐标

#### 4.7.2 Python 视觉程序（进程外 TCP，备用）

```
Python (QTcpSocket client) → TCP 5001 → VisionController (QTcpServer) → MainWindow
```

| 能力 | 实现 |
|------|------|
| 进程管理 | `QProcess` 启动 `python -m main`，监控 stdout/stderr/退出 |
| TCP 通信 | `QTcpServer` 监听 5001，等待 Python 主动 connect |
| 数据解析 | JSON `motor_commands` 数组 → `QList<QPair<int,double>>` (axis, angle) |
| 信号 | `motorCommands(commands)` → MainWindow 防抖+节流后下发 |

### 4.8 基础设施层

#### `ZmotionAPI`（4470 行）— Zmotion C SDK 的 C++ 封装

| 类别 | 方法示例 |
|------|----------|
| 连接 | `openEth(ip)` / `close()` / `isConnected()` |
| 轴配置 | `setAtype()` / `setAxisUnits()` / `setAxisSpeed/Accel/Decel/Lspeed/Creep/Sramp()` |
| 运动控制 | `directMoveAbsolute()` / `directMoveRelative()` / `rapidStop()` / `singleAxisCancel()` |
| IO | `getDI()` / `setDO()` / `getDO()` / `setInvertIn()` / `setInvertOut()` |
| 状态 | `getAxisPosition()` / `getAxisSpeed()` / `getIfIdle2()` / `waitForAxesIdle()` / `waitForAxesReachTarget()` |
| 回零 | `setAxisSingleDatum()` / `setAxisDatumOrigin()` / `getAxisSingleHomeStatus()` |
| 运动学 | `startInverseKinematics()` / `startForwardKinematics()` / `setTable()` / `getTable()` |
| Modbus | `modbusMRegSet()` / `setCom()` / `putChar()` — RS485 透传 |

统一返回 `ZmotionResult { bool success; int errorCode; QString errorMessage; QList<double> values; }`。

---

## 5. 对象装配与运行时关系

```
MainWindow
 ├── RobotController (业务逻辑核心)
 │    ├── RobotConfig*              ← MainWindow 从 JSON 加载后传入
 │    ├── RobotState*               ← RobotController 内部 new
 │    ├── MotionController*         ← new, 持有 config + state
 │    │    └── ZmotionAPI*          ← MotionController 内部 new（唯一实例）
 │    ├── HomingController*          ← new, 持有 ZmotionAPI + config + state
 │    ├── Rs485Controller*           ← new
 │    │    └── setZmotionAPI(...)    ← 共享 MotionController 的同一 ZmotionAPI
 │    └── ServoController*           ← new(Rs485Controller*)
 │
 ├── UiController (UI 辅助逻辑)
 │    └── TaskExecutor*             ← new(RobotController*)
 │
 ├── VmWindow* (视觉方案操作窗口，延迟创建)
 │    └── VMControl*                ← VmWindow 内部 new
 │         └── VMbase*              ← VMControl 内部 new
 │
 ├── VisionRobotLink* (视觉→机械臂坐标转换)
 │
 ├── VisionController* (Python 视觉进程管理，可选)
 │
 ├── DrawingWindow* (画板窗口，非模态)
 │    └── WorkPieceCoord m_workPieceCoord (工件坐标系)
 │
 └── ServoTestWidget* (舵机调试弹窗)
```

**关键设计点**：
1. **单一 ZmotionAPI 实例**：运动控制与 RS485 通信复用同一控制器连接，避免双连接冲突。
2. **RobotController 是业务中枢**：UI 层只与 RobotController 和 TaskExecutor 交互。
3. **视觉旁路接入**：VMControl 的数据信号经 VisionRobotLink 转换坐标后，通过 MainWindow 中转到 RobotController。
4. **画板旁路接入**：DrawingWindow 的轨迹信号经 MainWindow 切换逆解模式后调用 moveL()。

---

## 6. 信号槽数据流

```
[硬件状态]  ZmotionAPI → MotionController → RobotController →(信号)→ MainWindow/UiController → UI 刷新

[脚本执行]  UiController → TaskExecutor → RobotController → MotionController → ZmotionAPI → 硬件

[视觉定位]  VM SDK → VMbase → VMControl →(dataReady)→ VisionRobotLink →(targetReady)→ MainWindow → RobotController → 硬件
           Python → TCP → VisionController →(motorCommands)→ MainWindow →(防抖+节流)→ RobotController → 硬件

[画板轨迹]  DrawingWindow →(sendDrawingPoints)→ MainWindow →(切换逆解)→ RobotController::moveL() → 硬件

[舵机控制]  TaskExecutor/UiController → ServoController → Rs485Controller → ZmotionAPI → RS485 → PWM 模块 → 舵机
```

---

## 7. CMake 构建系统

### 7.1 编译目标

| 库名 | 类型 | 依赖 |
|------|------|------|
| `zmotion_sdk` | INTERFACE | zauxdll.lib, zmotion.lib |
| `zmotion_api` | STATIC | Qt::Core, zmotion_sdk |
| `robot_models` | STATIC | Qt::Core |
| `robot_hardware` | STATIC | Qt::Core, Qt::SerialPort, robot_models, zmotion_sdk, zmotion_api |
| `robot_logic` | STATIC | Qt::Core, Qt::Widgets, robot_models, robot_hardware, zmotion_api |
| `robot_utils` | STATIC | Qt::Core, Qt::Widgets |
| `robot_ui` | STATIC | Qt::Widgets, robot_hardware |
| `robot_vision` | STATIC | Qt::Core, Qt::Network |
| `vm_control` | STATIC | Qt::Core, VisionMaster SDK |
| `Robot_R06` | EXECUTABLE | 以上全部 + Qt::Widgets/Network/Concurrent/AxContainer |

### 7.2 POST_BUILD 拷贝

| 文件/目录 | 目的 |
|-----------|------|
| zauxdll.dll, zmotion.dll | Zmotion SDK 运行时 |
| MultiCard.dll, my_cross_platform.dll | VisionMaster SDK 运行时 |
| myLibs/ 目录 | VM ActiveX 控件运行时依赖 |
| 视觉方案/ 目录 | .sol 方案文件 |

### 7.3 构建后部署

- Qt6：自动执行 `windeployqt` + 生成 `qt.conf`
- MSVC：全局屏蔽 C4828（编码）/C4091（typedef 冗余）警告（VM SDK GBK 头文件）

---

## 8. 脚本引擎

### 8.1 脚本格式示例

```
# 点位定义
TARGET P1 = {J0: 0, J1: 90, J2: -45, J3: 0, J4: 45, J5: 0}
TARGET HOME = {J0: 0, J1: 0, J2: 0, J3: 0, J4: 0, J5: 0}

# 运动指令
SPEED(30)
ACCEL(80)
MOVEJ(P1, speed=25, acc=50, dec=60)
MOVEL({100, 200, 150, 0, 0, 0}, speed=50)
WAIT_IDLE()
DELAY(500)

# IO 控制
SET_DO(0, 1)
WAIT_DI(1, 1, timeout=5000)

# 循环与条件
FOR(i, 1, 10, 1)
    MOVEJ(P1)
    MOVEJ(HOME)
END_FOR

IF(DI, 2, 1)
    PRINT("Sensor triggered")
ELSE
    PRINT("Sensor not triggered")
END_IF

# 舵机控制
SERVO(0=90:10, 1=45:5, 2=120:30)

# 子脚本调用
CALL("sub_task.rbt")

# 变量与表达式
x = 100
y = x + 50
MOVEOFF(0, y, 0, 0, 0, 0)
```

### 8.2 参数优先级体系

```
点位级 motion (TargetDef.motion)  ← 最高
  ↓ 未设时
全局 SPEED/ACCEL/DECEL/TIMEOUT
  ↓ 未设时
行级 speed=/acc=/dec= 参数
  ↓ 未设时
默认值 (25/50/60/10000)
```

---

## 9. 硬件通信链路总览

```
┌──────────┐   TCP/IP    ┌───────────────┐
| 上位机    |────────────→| ZMC306X       |
| Robot_R06 | 192.168.0.x | 控制器         |
└──────────┘             └───┬───────┬───┘
                             │       │
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
                                    │ 舵机 P01~P06 │
                                    │ 0°~180°      │
                                    └──────────────┘
```

- **运动控制**：进程内直接调用 Zmotion SDK（低延迟、确定性）
- **RS485 通信**：通过 ZMC 控制器 Modbus 透传（ZMC 做 Modbus 主端）
- **视觉**：VM SDK 进程内集成（稳定）+ Python 进程外 TCP（灵活）

---

## 10. 关键设计亮点

| # | 亮点 | 说明 |
|---|------|------|
| 1 | 单一硬件连接共享 | Rs485 与运动控制复用同一 ZmotionAPI 实例，避免双连接冲突 |
| 2 | 视觉 SDK 进程内集成 | VMControl/VMbase 直接调用 SDK，通过信号槽解耦，崩溃可隔离 |
| 3 | 视觉坐标转换滤波 | VisionRobotLink 预热丢弃 + 移动平均，抑制视觉抖动 |
| 4 | 舵机软插值优化 | 50ms tick + 批量帧 + diff 缓存，解决旧版阶梯抖动 |
| 5 | 画板轨迹 3D 变换 | 2D 绘图 → WorkPieceCoord 变换矩阵 → 机械臂笛卡尔运动 |
| 6 | 脚本参数优先级 | 点位级 > 全局 > 行级 > 默认，灵活且可预测 |
| 7 | 回零状态机 | 串行预处理避免干涉 + 并行回零含反向限位保护 |
| 8 | 文件日志实时性 | 绕过 VM SDK 缓冲，立即 flush，不受第三方库影响 |
| 9 | 系统软键盘适配 | 工控触摸屏场景自动弹键盘 |
| 10 | 开发者登录门禁 | 限制未授权用户访问高级功能 |

---

## 11. 代码量统计

| 文件 | 行数 | 职责 |
|------|------|------|
| ZmotionAPI.cpp | 4470 | SDK 封装（全量 API） |
| UiController.cpp | 3987 | UI 辅助逻辑（IO/报警/编程/设置/手动） |
| mainwindow.cpp | 3882 | 主窗口（UI 装配/信号槽/视觉中转） |
| drawing_window.cpp | 2918 | 画板轨迹编辑器 |
| TaskExecutor.cpp | 1206 | 脚本执行引擎 |
| RobotController.cpp | 986 | 业务核心协调器 |
| vmbase.cpp | 993 | VM SDK 底层封装 |
| ServoController.cpp | 910 | 舵机控制（速度插值/批量） |
| HomingController.cpp | 882 | 回零状态机 |
| vmcontrol.cpp | 871 | VM 模块控制层 |
| VmWindow.cpp | 613 | 视觉方案操作窗口 |
| Rs485Controller.cpp | 518 | RS485 Modbus 通信 |
| VisionController.cpp | 422 | Python 视觉进程管理 |
| MotionController.cpp | 289 | 连接/轮询/模式切换 |
| VisionRobotLink.cpp | 163 | 视觉坐标转换 |
| **合计** | **~23,110** | |

---

## 12. 配置文件

### `config/robot_config.json`

| 字段 | 说明 | 默认值 |
|------|------|--------|
| controllerIP | ZMC306X 控制器 IP | 192.168.0.123 |
| axisCount | 轴数 | 6 |
| jointAxisList | 关节轴列表 | [0,1,2,3,4,5] |
| virtualAxisList | 虚拟笛卡尔轴列表 | [6,7,8,9,10,11] |
| axisUnits | 各轴脉冲当量 | 由 DH 参数计算 |
| defaultSpeed/Accel/Decel/Timeout | 默认运动参数 | 25/50/60/10000 |
| axisConfigs[] | 各轴限位/回零参数 | 见配置文件 |

### `config/developer_account.ini`

开发者账号密码配置，用于登录门禁。

---

## 13. 潜在改进方向

| # | 改进点 | 现状 | 建议 |
|---|--------|------|------|
| 1 | MainWindow 体量大 | 3882 行，职责多 | 拆分视觉指令处理、RS485 UI、登录模块为独立类 |
| 2 | UiController 体量大 | 3987 行，聚合多页面逻辑 | 按页面拆分子控制器（IOController/AlarmController/ProgramController） |
| 3 | 硬编码 VM SDK 路径 | `C:/Program Files/VisionMaster4.4.0/...` | CMake 变量化或环境变量 |
| 4 | RobotController 与 MainWindow 耦合 | MainWindow 直接获取子控制器 | 统一封装接口，减少直接访问内部组件 |
| 5 | 脚本引擎无错误行号 | parseScript 报错时定位不够精确 | 增强 parser 错误定位能力 |
| 6 | draw 模块未纳入 CMake 子目录 | 根 CMakeLists 直接列源文件 | 独立为 `robot_draw` STATIC 库 |

---

*本文档基于 2026-08-22 源码静态分析生成，覆盖全部子模块的当前实现状态。*
