@echo off
REM 加载 Visual Studio 2026 开发者环境（提供 cl/link/lib 与 MSBuild 所需 PATH/INCLUDE/LIB）
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64
cd /d C:\Users\111\Desktop\Robot_R06
echo ===== [1/2] 配置 (cmake) =====
cmake -B build -S . -DCMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64
if errorlevel 1 (
    echo CONFIGURE FAILED
    exit /b 1
)
echo ===== [2/2] 编译 (cmake --build, Debug) =====
cmake --build build --config Debug --target Robot_R06
