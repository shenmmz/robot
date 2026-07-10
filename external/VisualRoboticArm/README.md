# VisualRoboticArm

VisualRoboticArm 是一个基于 PySide6 + MediaPipe 的实时视觉机械臂姿态项目。程序从摄像头采集画面，在后台线程中执行人体姿态推理，计算左右手臂方向角，并将结果显示在界面中，也可以通过 TCP/串口链路输出给外部控制模块。

## 项目特点

- 使用 Qt 摄像头管线采集实时画面。
- 使用后台线程执行推理，避免阻塞 UI。
- 使用 MediaPipe Pose 获取人体关键点。
- 使用三维 world landmarks 计算左右手臂 6 维方向角。
- 使用过滤模块抑制小幅抖动，避免电机频繁微动。
- 通过 TCP payload 输出电机控制所需的紧凑数据。
- 固定 Python 3.12，便于环境统一和后续移植。

## 推理核心逻辑

当前推理方案不依赖外部模型文件，也不再使用独立手部模型。推理过程由 `app/services.py` 封装为实时服务：

```text
摄像头画面
    -> Qt QVideoFrame
    -> QImage
    -> 后台推理线程
    -> OpenCV BGR 图像
    -> MediaPipe Pose
    -> pose_world_landmarks
    -> 左右臂 6 维方向角
    -> payload
    -> UI 显示 / TCP 输出
```

角度计算思路：

1. 从 MediaPipe Pose 中读取肩、肘、腕、拇指、食指、小指等关键点。
2. 对单侧手臂构造三根方向向量：
   - 肩到肘：上臂方向
   - 肘到腕：前臂方向
   - 手部三点中心到腕：手腕参考方向
3. 将每根方向向量投影到全局 XZ 平面。
4. 计算投影向量与 Z 轴、X 轴的夹角。
5. 每侧手臂输出 6 个角度：

```text
shoulder pitch / shoulder yaw
elbow pitch    / elbow yaw
wrist pitch    / wrist yaw
```

这些角度表示“骨骼方向相对全局坐标轴的方向角”，更适合当前姿态跟随和电机映射验证；它们不是严格的人体解剖关节角。

## 电机输出

推理 payload 会保留完整角度信息，并进一步压缩成外部控制端需要的电机字段：

- 1 号电机：肩部上下，来源 `shoulder_flex_deg`
- 2 号电机：肩部左右，来源 `shoulder_abduction_deg`
- 3 号电机：肘部上下，来源 `elbow_flex_deg`
- 4 号电机：肘部左右，来源 `elbow_abduction_deg`
- 5 号电机：手腕上下，来源 `wrist_pitch_deg`
- 6 号电机：手腕左右翻转，来源 `signed_roll_deg`

通信模块输出的紧凑字段包括：

```text
motor_3
motor_4
motor_6
motor_commands
summary
```

## 项目结构

```text
VisualRoboticArm/
├── main.py                  # 程序入口，初始化 Qt 主窗口
├── app/
│   ├── __init__.py           # 包导出层，打印 PyTorch 运行时信息
│   ├── main_window.py        # Qt 主界面、摄像头、显示、通信调度
│   ├── worker.py             # 后台帧处理线程
│   ├── services.py           # 实时推理服务
│   ├── models.py             # 姿态、角度、电机命令数据结构
│   ├── pose_filter.py        # 角度与电机命令稳定过滤
│   ├── pose_comm.py          # TCP 姿态输出
│   └── rs485.py              # 串口/RS485 通信
├── solver/
│   ├── angle_calc.py         # 角度计算导出层
│   ├── shoulder.py           # 肩部角度计算
│   ├── elbow.py              # 肘部角度计算
│   ├── wrist.py              # 腕部角度与 6 号电机计算
│   ├── fusion.py             # 多路角度融合
│   ├── motor_mapping.py      # 角度名称与电机编号映射
│   └── utils.py              # 几何工具函数
├── stage1_comm/              # TCP 通信子进程/桥接模块
├── scripts/
│   ├── prepare_python_env.ps1 # 在线/离线创建 Python 环境
│   └── build_portable_package.ps1 # 生成可移植 wheelhouse 依赖包
├── requirements.txt          # Python 依赖
├── setup_env.bat             # Python 3.12 环境初始化脚本
└── README.md                 # 项目说明
```

## 环境要求

- Windows 10/11
- Python 3.12
- 摄像头设备
- 可选 NVIDIA GPU

当前实时推理主要由 MediaPipe/TFLite 执行。PyTorch 已列入依赖，用于启动时输出运行环境信息，并为后续深度学习模型扩展预留。

## 在线环境搭建

进入项目根目录：

```powershell
cd <project-root>
```

推荐使用批处理脚本：

```powershell
.\setup_env.bat
```

或使用 PowerShell 脚本：

```powershell
.\scripts\prepare_python_env.ps1
```

手动创建环境：

```powershell
py -3.12 -m venv .venv
.\.venv\Scripts\python.exe -m pip install --upgrade pip
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
```

运行程序：

```powershell
.\.venv\Scripts\python.exe main.py
```

指定 TCP 通信参数：

```powershell
.\.venv\Scripts\python.exe main.py --host 127.0.0.1 --port 5001 --name visual-arm
```

## 可移植环境打包与迁移

为了便于在无网络或弱网络电脑上部署，可以先在有网络的同系统开发电脑上生成离线依赖包，再把项目源码和依赖包一起复制到目标电脑。推荐迁移源码和 `portable_package`，不推荐直接复制 `.venv`，因为虚拟环境中包含本机路径和平台相关文件。

适用前提：

- 打包电脑和目标电脑均为 Windows 64 位。
- 目标电脑已安装 Python 3.12。
- 目标电脑的 Python 架构与打包电脑一致，通常为 64 位。
- 若目标电脑需要使用摄像头、串口或 TCP 通信，应提前确认对应驱动和权限可用。

### 1. 在打包电脑上准备环境

```powershell
cd <project-root>
.\scripts\prepare_python_env.ps1
```

该命令会创建或复用 `.venv`，检查 Python 3.12，并按 `requirements.txt` 在线安装依赖。

### 2. 下载离线 wheel 依赖包

```powershell
.\scripts\build_portable_package.ps1
```

该命令会执行 `pip download`，把项目依赖及其递归依赖下载到 `portable_package\wheelhouse`。默认会生成：

```text
portable_package/
├── wheelhouse/              # 离线 wheel 依赖
├── requirements.txt         # 依赖清单
├── setup_env.bat            # 初始化脚本副本
└── scripts/
    └── prepare_python_env.ps1
```

### 3. 校验离线包完整性

在打包电脑上执行 dry-run，不真正安装，只验证 `wheelhouse` 是否能满足 `requirements.txt`：

```powershell
.\.venv\Scripts\python.exe -m pip install --dry-run --ignore-installed --no-index --find-links .\portable_package\wheelhouse -r .\portable_package\requirements.txt
```

若命令最后输出 `Would install ...` 且没有 `No matching distribution found`，说明离线依赖解析完整。

### 4. 复制到目标电脑

将以下内容复制到目标电脑的项目目录：

```text
VisualRoboticArm/
├── app/
├── solver/
├── stage1_comm/
├── scripts/
├── portable_package/
├── main.py
├── requirements.txt
├── setup_env.bat
└── README.md
```

建议不要复制这些运行期目录：

```text
.venv/
__pycache__/
.pytest_cache/
```

### 5. 在目标电脑离线创建环境

```powershell
cd <project-root>
.\scripts\prepare_python_env.ps1 -Offline -Wheelhouse .\portable_package\wheelhouse
```

离线模式会使用 `--no-index --find-links`，只从本地 `wheelhouse` 安装依赖，不访问网络。

### 6. 在目标电脑验证运行

检查依赖一致性：

```powershell
.\.venv\Scripts\python.exe -m pip check
```

检查 PyTorch 版本和运行设备：

```powershell
.\.venv\Scripts\python.exe -c "import torch; print('torch:', torch.__version__); print('cuda:', torch.cuda.is_available()); print('device:', torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'CPU')"
```

启动程序：

```powershell
.\.venv\Scripts\python.exe main.py
```

### GPU / PyTorch 说明

离线包默认根据 `requirements.txt` 下载通用依赖。若 `requirements.txt` 中只写 `torch`，`pip download` 会从默认 PyPI 源选择可用 wheel，不保证与某个 CUDA 版本绑定。

若目标电脑必须使用特定 CUDA 版本的 PyTorch，需要在打包电脑上使用相同 Python 版本、相同系统平台和相同 PyTorch 依赖源重新生成 `wheelhouse`。生成后先用上面的 dry-run 命令验证，再迁移到目标电脑。当前实时推理由 MediaPipe/TFLite 执行，不依赖 PyTorch GPU；PyTorch 主要用于启动时输出运行环境信息，并为后续深度学习模型扩展预留。

## 验证命令

语法检查，不写入 `.pyc`：

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'
.\.venv\Scripts\python.exe -B -c "import pathlib; [compile(p.read_text(encoding='utf-8'), str(p), 'exec') for p in pathlib.Path('.').rglob('*.py') if '.venv' not in p.parts]"
```

服务级空帧推理验证：

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'
.\.venv\Scripts\python.exe -B -c "import numpy as np; from app.services import MediaPipePoseHandService; s=MediaPipePoseHandService(); frame=np.zeros((120,160,3), dtype=np.uint8); out,p=s.process_bgr(frame); print(out.shape, p['source'], p['control_stage'], p['motor_6']['reason']); s.close()"
```

依赖检查：

```powershell
.\.venv\Scripts\python.exe -m pip check
```

## 依赖

```text
mediapipe==0.10.18
opencv-python==4.11.0.86
numpy==1.26.4
PySide6==6.11.1
protobuf==4.25.9
torch
```

## 注意事项

- 源码注释和文档统一使用中文 UTF-8。
- OpenCV 的 `putText` 对中文支持有限，推理画面上的角度叠字保留英文缩写。
- MediaPipe 初始化时可能打印 TFLite/XNNPACK 日志，这是正常底层提示。
- 当前项目不依赖固定本地视频文件，也不会在推理阶段阻塞等待按键。
