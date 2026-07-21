@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================================
echo   VisualRoboticArm - Python 3.12 环境初始化
echo ============================================
echo.

set "PYTHON_CMD=py -3.12"

echo [1/3] 检查 Python 3.12...
%PYTHON_CMD% -c "import sys; raise SystemExit(0 if sys.version_info[:2] == (3, 12) else 1)" >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未检测到可用的 Python 3.12。
    echo        请安装 Python 3.12，或确认 py launcher 可执行：py -3.12
    pause
    exit /b 1
)
for /f "tokens=*" %%v in ('%PYTHON_CMD% --version 2^>^&1') do set PYVER=%%v
echo        检测到 !PYVER!
echo.

echo [2/3] 检查虚拟环境 .venv...
if exist ".venv\Scripts\python.exe" (
    .venv\Scripts\python.exe -c "import sys; raise SystemExit(0 if sys.version_info[:2] == (3, 12) else 1)" >nul 2>&1
    if !errorlevel! neq 0 (
        echo [错误] 现有 .venv 不是 Python 3.12，请先手动删除 .venv 后重试。
        pause
        exit /b 1
    )
    echo        .venv 已存在，且为 Python 3.12
) else (
    echo        正在创建 .venv...
    %PYTHON_CMD% -m venv .venv
    if !errorlevel! neq 0 (
        echo [错误] 虚拟环境创建失败
        pause
        exit /b 1
    )
    echo        .venv 创建成功
)
echo.

echo [3/3] 安装项目依赖...
.venv\Scripts\python.exe -m pip install --upgrade pip -q
.venv\Scripts\python.exe -m pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo [错误] 依赖安装失败，请检查网络连接或 PyTorch 安装源。
    pause
    exit /b 1
)
echo        依赖安装完成
echo.

echo ============================================
echo   环境初始化完成
echo.
echo   当前推理模块：MediaPipe Pose / video_inf 实时推理
echo   已不再需要 models\hand_landmarker.task
echo.
echo   运行方式：
echo     .venv\Scripts\python.exe main.py
echo   或：
echo     .venv\Scripts\python.exe main.py --port 5001
echo ============================================

endlocal
pause
