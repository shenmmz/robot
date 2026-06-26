# VisualRoboticArm — Python 视觉识别与 QT 通讯模块

## 项目简介

本项目负责**人体手臂姿态视觉识别**与**实时数据回传**，是机械臂模仿跟随系统的"感知层"。

**核心职责边界**：
- Python 端：通过 USB 相机采集图像，使用 MediaPipe Holistic 进行人体手臂关键点检测与关节角度计算，实时回传结构化姿态数据。
- QT 通讯模块：管理 Python 子进程生命周期（自启动/停止/重启），通过 TCP 接收 Python 推送的姿态数据，解析后在 GUI 日志区显示，并对外提供标准化数据接口。
- **不负责**：运动学逆解、电机控制、多电机联调等后续处理——这些由下游模块（机械臂驱动层）独立完成。

## 最终需求

| 需求项 | 说明 |
|--------|------|
| QT 模块自启动 Python 程序 | QT 启动后通过 QProcess 自动拉起 Python 视觉子进程，退出时自动终止 |
| Python 进行人体手臂识别 | 基于 MediaPipe Holistic（33 全身关键点 + 手部 21 点），实时检测手臂姿态 |
| 数据格式统一 | 基于 JSON Line Protocol（`stage1_comm/protocol.py`），定义 pose/hello/heartbeat/cmd/ack 消息类型 |
| 实时回传通讯 | Python 每帧计算完成后通过 TCP 推送姿态数据到 QT 侧 |
| QT 模块接收数据 | QT 侧 LocalCommBridge 接收、解析、显示角度数据 |

## 项目结构

```
VisualRoboticArm/
├── main.py                         # Python 入口，启动 PySide6 Qt 应用
├── README.txt                      # 本文件
├── 设计方案.md                      # 详细设计方案（与本文件配套）
├── requirements.txt                # Python 依赖（PySide6/MediaPipe/OpenCV/Numpy）
├── app/
│   ├── main_window.py             # PySide6 主窗口（相机预览/角度显示/串口面板/姿态通讯）
│   ├── models.py                   # 数据模型（ArmAngles/JointRotation/Motor6Command/Point3D）
│   ├── services.py                 # MediaPipe Holistic 服务（姿态检测+3D角度计算+绘图）
│   ├── worker.py                   # 帧处理线程（QImage→OpenCV→MediaPipe→结果回传）
│   ├── pose_comm.py               # TCP 姿态通讯客户端（PoseCommClient，自动重连）
│   ├── pose_filter.py             # 姿态运动滤波器（PoseMotionFilter，阈值去抖）
│   └── rs485.py                    # RS485 串口管理器（调试用）
├── solver/
│   ├── __init__.py
│   └── angle_calc.py              # 3D 关节角度解算器（肩/肘/腕/掌面旋转，528行完整实现）
├── stage1_comm/                    # 最小通讯测试框架（独立可运行）
│   ├── __init__.py
│   ├── main.py                     # Qt 侧测试入口
│   ├── bridge.py                   # LocalCommBridge（TCP Server，管理连接）
│   ├── process_controller.py       # PythonProcessController（QProcess 生命周期管理）
│   ├── protocol.py                 # JSON Line 消息协议（build/encode/decode）
│   ├── README.md
│   └── python_sub/                 # Python 子进程模拟客户端
├── qt_main_controller/             # QT C++ 主控项目（接收端，可独立构建运行）
│   ├── CMakeLists.txt              # CMake 构建配置（Qt6 Core/Widgets/Network）
│   ├── build/                      # 构建产物目录
│   └── src/
│       ├── main.cpp                # QT 入口
│       ├── MainWindow.h / .cpp     # QT 主窗口（日志区/姿态显示/Python进程控制/通讯管理）
│       ├── LocalCommBridge.h / .cpp # TCP Server 通信层（管理客户端连接和数据收发）
│       ├── PythonProcessController.h / .cpp  # Python 子进程管理（启动/停止/重启/stdout捕获）
│       └── JsonProtocol.h          # JSON 协议定义与辅助函数
└── Output/                         # 文档产出目录（旧版文档存档）
```

## 架构概览

```
┌──────────────────────────────────────────────────────────┐
│                  QT 主控程序 (C++ Qt6)                    │
│  ┌────────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │ MainWindow     │  │ 运动学逆解    │  │ 机械臂驱动层  │ │
│  │ (日志/姿态显示) │  │ (下游模块)    │  │ (下游模块)    │ │
│  └───────┬────────┘  └──────▲───────┘  └──────▲───────┘ │
│          │                  │                   │        │
│  ┌───────┴──────────────────┴───────────────────┴──────┐ │
│  │         消息总线 / 状态管理（下游负责）              │ │
│  └──────────────────────┬──────────────────────────────┘ │
│                         │                                │
│             TCP Server (JSON Line Protocol)               │
│                 127.0.0.1:5001                            │
│                         │                                │
│  ┌──────────────────────┴──────────────────────────────┐ │
│  │  LocalCommBridge (TCP Server, 管理连接)              │ │
│  │  PythonProcessController (QProcess, 管理生命周期)    │ │
│  └──────────────────────┬──────────────────────────────┘ │
│                         │                                │
│          TCP Client ← → TCP Server (JSON Line)           │
│                         │                                │
│  ┌──────────────────────┴──────────────────────────────┐ │
│  │        Python 视觉识别进程 (PySide6，独立进程)        │ │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────┐  │ │
│  │  │摄像头采集│ │MediaPipe │ │3D角度解算│ │滤波去抖│  │ │
│  │  │(PySide6) │ │Holistic  │ │(solver/) │ │(filter)│  │ │
│  │  └──────────┘ └──────────┘ └─────┬────┘ └───┬────┘  │ │
│  │                                  │           │       │ │
│  │                    PoseCommClient (TCP 客户端)        │ │
│  └──────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────┘
```

## 通信协议

**通讯方式**：TCP + JSON Line Protocol（每行一个完整 JSON 对象，以 `\n` 分隔）

**协议定义文件**：
- Python 侧：`stage1_comm/protocol.py`
- C++ 侧：`qt_main_controller/src/JsonProtocol.h`

**消息类型**：

| 类型 | 方向 | 用途 |
|------|------|------|
| hello | Python → QT | 建连握手，告知角色和版本 |
| heartbeat | Python → QT | 心跳保活 |
| pose | Python → QT | 实时姿态数据（核心消息） |
| cmd | QT → Python | 控制指令 |
| ack | Python → QT | 指令确认 |
| shutdown | QT → Python | 关闭信号 |

**pose 消息结构**：

```json
{
  "type": "pose",
  "seq": 123,
  "ts": 1718700000.123,
  "role": "python",
  "payload": {
    "protocol_version": "vision_motor_v1",
    "source": "vision",
    "client": "visual-arm",
    "frame_id": 1024,
    "control_stage": "right_wrist_motor_6_only",
    "pose_valid": true,
    "motion_allowed": true,
    "filter_state": "move",
    "filter_reason": "ok",
    "motor_6": {
      "motor_id": 6,
      "pwm_index": 6,
      "target": "right_wrist_roll",
      "joint": "wrist",
      "axis": "roll",
      "state": "send",
      "valid": true,
      "motion_allowed": true,
      "direction": "left",
      "direction_text": "左旋转",
      "rotation_side": "inward",
      "angle_deg": 35.2,
      "signed_angle_deg": 35.2,
      "confidence": 0.92,
      "reason": "ok"
    },
    "motor_commands": [...],
    "summary": "6号电机：发送 | 左旋转 35.2°"
  }
}
```

## 计划阶段

| 阶段 | 目标 | 状态 |
|------|------|------|
| 阶段一 | **通讯打通**：两端通讯闭环，QT 日志区显示实时角度数据。验证：启动 QT 模块后自动启动 Python 程序 | ✅ 已完成 |
| 阶段二 | **初步测试**：单一的手腕角度识别（6号电机），实现先控制一个电机的目标。验证：能够实时识别手腕翻转角度，机械臂单一电机跟随旋转 | 🔄 进行中 |
| 阶段三 | **3D 角度还原**：从 MediaPipe 3D 关键点计算物理空间关节角（肩关节 3DOF/肘关节/腕关节） | 📋 已部分完成（solver/angle_calc.py 已实现，待联调验证） |
| 阶段四 | **相机标定**：建立图像坐标系到机械臂基坐标系的映射（棋盘格标定 + 手眼标定） | ⏳ 待开始 |
| 阶段五 | **滤波优化**：减少关键点抖动，输出平滑角度（当前为阈值滤波，需升级为卡尔曼或低通滤波） | ⏳ 待开始 |
| 阶段六 | **集成联调**：完整的"人动 → 识别 → 通讯 → 机械臂动"闭环 | ⏳ 待开始 |

## 硬件限制

| 项目 | 说明 |
|------|------|
| 摄像头 | USB 相机 × 1 |
| 机械臂 | 1 台，6 电机 |
| 当前测试范围 | **仅右臂**（受限于硬件数量） |

## 构建与运行

### Python 端（视觉识别进程）

**前置依赖**：
```bash
pip install -r requirements.txt
```

**用于电脑移植的固定依赖包**：
```bash
pip install -r requirements-portable.txt
```

**一键创建虚拟环境并安装依赖**：
```powershell
.\scripts\prepare_python_env.ps1 -PythonExe python -EnvDir .venv
```

若需要离线移植，先在联网电脑生成 wheel 包目录：
```powershell
python -m pip download -r requirements-portable.txt -d wheelhouse
```
再在目标电脑执行：
```powershell
.\scripts\prepare_python_env.ps1 -PythonExe python -EnvDir .venv -Offline
```

**独立运行**（PySide6 应用，含 GUI 预览）：
```bash
cd E:\PythonProject\TestOnline\VisualRoboticArm
python main.py
```

**作为 QT 子进程运行**（通过 stage1_comm 接入）：
```bash
python -m stage1_comm.python_sub.main --host 127.0.0.1 --port 5001 --name visual-arm
```

### QT C++ 端（主控程序）

**前置依赖**：
- Qt 6.5+（Core + Widgets + Network）
- MSVC 2022 或 MinGW（Windows）

**CMake 构建**：
```bash
cd qt_main_controller
mkdir build && cd build
cmake .. -DQt6_DIR=<Qt安装路径>/lib/cmake/Qt6 -DCMAKE_PREFIX_PATH=<Qt安装路径>
cmake --build . --config Release
```

**运行**：
1. 启动编译好的 `VisualRoboticArmQtController.exe`
2. 程序自动启动 Python 子进程
3. 等待 TCP 连接建立（日志区显示 "客户端已连接"）
4. 日志区开始显示实时角度数据

### 电脑移植说明

- Qt 主控界面已支持手动选择 `Python 可执行文件` 和 `Python 工作目录`，不再依赖固定电脑路径。
- 监听地址、监听端口、客户端名保留默认值，同时允许手动修改。
- Qt 启动配置会自动保存到本机 `QSettings`，下次打开后继续使用上次配置。
- Python 侧建议优先使用 `requirements-portable.txt` 对齐运行环境，避免不同电脑上的包版本漂移。

### 完整联调流程

1. 启动 QT 主控程序 → 自动拉起 Python 视觉进程
2. Python 进程启动 PySide6 应用 → 枚举相机 → 连接相机 → 开始采集
3. MediaPipe 每帧推理 → 3D 角度计算 → 运动滤波 → TCP 推送 pose 消息
4. QT 侧接收 pose 消息 → 解析 → 日志区显示实时关节角度
5. 下游模块从 QT 消息总线订阅 pose 数据 → 运动学逆解 → 电机控制

## 关键技术决策

1. **TCP + JSON Line 替代 ZeroMQ**：减少外部依赖，QT 端仅需 Qt6::Network 模块，部署更简便。JSON Line 格式人类可读，调试友好。

2. **QProcess 管理 Python 生命周期**：Python 崩溃不影响 QT 主控，两个项目可独立开发、独立调试。退出时自动 terminate。

3. **PoseCommClient 自动重连**：Python 端内建重连定时器，QT 端重启后无需手动干预。

4. **PoseMotionFilter 阈值去抖**：当前阶段使用移动阈值滤波（默认 5° 死区），后续阶段五升级为卡尔曼滤波。

5. **职责解耦**：Python 端仅输出标准化 pose 数据，不做任何电机逻辑。QT 端接收数据后仅负责展示和中转，电机控制完全由下游模块处理。

## 接口约定（唯一耦合点）

**Python 侧**：通过 `PoseCommClient.send_pose()` 推送 `{"type":"pose", "payload":{...}}`  
**QT 侧**：在 `LocalCommBridge::messageReceived` 信号中解析 `payload` JSON

任一端的实现替换，只要保持这个消息格式不变，另一端无需任何修改。
