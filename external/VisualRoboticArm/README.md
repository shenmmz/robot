---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 0721c827b1e45fa870dc229f1f8535f0_b812e073735d11f1986d525400d9a7a1
    ReservedCode1: RWHfauitHQRLxxErbHwSIH8gjS5YFAsZJqeWAosVZe624QsGZ6kL/o0uF4mE+jpNHf01R40ttq8kq/aqpwRi872tJ3RekZFsnHhrgtDdc7eqIDmumWJ61s52h8XB33ysGEHawCqVW1sRgm23r9NBYZWMbVjEfsvXAN7ahZ+f7//Ez5KTuuJQZeSXeTU=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 0721c827b1e45fa870dc229f1f8535f0_b812e073735d11f1986d525400d9a7a1
    ReservedCode2: RWHfauitHQRLxxErbHwSIH8gjS5YFAsZJqeWAosVZe624QsGZ6kL/o0uF4mE+jpNHf01R40ttq8kq/aqpwRi872tJ3RekZFsnHhrgtDdc7eqIDmumWJ61s52h8XB33ysGEHawCqVW1sRgm23r9NBYZWMbVjEfsvXAN7ahZ+f7//Ez5KTuuJQZeSXeTU=
---

# VisualRoboticArm

基于 MediaPipe 的视觉机械臂控制系统，通过摄像头实时捕捉手部关节姿态，计算关节角度并经由 TCP 发送运动指令。

## 环境要求

- **Python 3.11+**
- Windows 10/11
- 摄像头设备（USB 或内置）

## 快速开始

### 一键环境初始化

双击项目根目录下的 `setup_env.bat`，脚本将自动完成：
1. 检测 Python 版本（需 3.11+）
2. 创建虚拟环境 `.venv`
3. 安装全部依赖
4. 验证模型文件

### 手动安装

```bash
python -m venv .venv
.venv\Scripts\activate
pip install -r requirements.txt
```

## 运行方式

```bash
# 默认端口启动
.venv\Scripts\python.exe main.py

# 指定端口启动
.venv\Scripts\python.exe main.py --port 5001

# 查看所有参数
.venv\Scripts\python.exe main.py --help
```

### 环境变量（可选覆盖默认值）

| 变量名 | 说明 | 默认值 |
|--------|------|--------|
| `VISUAL_ARM_COMM_HOST` | TCP 通信主机地址 | `127.0.0.1` |
| `VISUAL_ARM_COMM_PORT` | TCP 通信端口 | `5001` |
| `VISUAL_ARM_COMM_CLIENT` | 客户端标识名称 | `VisualRoboticArm` |

## 项目结构

```
VisualRoboticArm/
├── main.py              # 入口脚本
├── app/                 # 应用层（GUI 主窗口等）
├── solver/              # 求解器（角度计算、滤波等）
├── stage1_comm/         # 通信层（TCP 客户端）
├── models/              # MediaPipe 模型文件
├── scripts/             # 辅助脚本
├── requirements.txt     # Python 依赖清单
├── setup_env.bat        # 一键环境初始化脚本
└── .gitignore
```

## 依赖

| 包名 | 版本 | 用途 |
|------|------|------|
| mediapipe | 0.10.18 | 手部关键点检测 |
| opencv-python | 4.11.0.86 | 摄像头采集与图像处理 |
| numpy | 1.26.4 | 数值计算 |
| PySide6 | 6.11.1 | GUI 界面 |
| protobuf | 4.25.9 | MediaPipe 序列化依赖 |
*（内容由AI生成，仅供参考）*
