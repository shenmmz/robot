@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================================
echo   VisualRoboticArm - 一键环境初始化
echo ============================================
echo.

:: 1. 检测 Python 3.11+
echo [1/4] 检测 Python 环境...
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未检测到 Python，请先安装 Python 3.11+
    echo        下载地址: https://www.python.org/downloads/
    pause
    exit /b 1
)

for /f "tokens=2" %%v in ('python --version 2^>^&1') do set PYVER=%%v
echo        检测到 Python %PYVER%

for /f "tokens=1,2 delims=." %%a in ("%PYVER%") do (
    set MAJOR=%%a
    set MINOR=%%b
)
if %MAJOR% lss 3 (
    echo [错误] Python 版本过低，需要 3.11+
    pause
    exit /b 1
)
if %MAJOR% equ 3 if %MINOR% lss 11 (
    echo [错误] Python 版本过低（当前 %PYVER%），需要 3.11+
    pause
    exit /b 1
)
echo        Python 版本满足要求
echo.

:: 2. 创建虚拟环境
echo [2/4] 检查虚拟环境...
if exist ".venv\Scripts\python.exe" (
    echo        虚拟环境已存在，跳过创建
) else (
    echo        正在创建虚拟环境...
    python -m venv .venv
    if %errorlevel% neq 0 (
        echo [错误] 虚拟环境创建失败
        pause
        exit /b 1
    )
    echo        虚拟环境创建成功
)
echo.

:: 3. 安装依赖
echo [3/4] 安装项目依赖...
.venv\Scripts\python.exe -m pip install --upgrade pip -q
.venv\Scripts\python.exe -m pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo [错误] 依赖安装失败，请检查网络连接后重试
    pause
    exit /b 1
)
echo        依赖安装完成
echo.

:: 4. 检查模型文件
echo [4/4] 检查模型文件...
if exist "models\hand_landmarker.task" (
    echo        模型文件 hand_landmarker.task 已就绪
) else (
    echo [警告] 模型文件 models\hand_landmarker.task 不存在！
    echo        请确保该文件已从仓库下载（Git LFS 或手动获取）
)
echo.

echo ============================================
echo   环境初始化完成！
echo.
echo   运行方式：
echo     .venv\Scripts\python.exe main.py
echo   或
echo     .venv\Scripts\python.exe main.py --port 5001
echo ============================================

endlocal
pause
