@echo off
setlocal
REM ============================================================
REM Robot_R06 一键构建脚本
REM 工具链：Qt 6.9.0 MSVC2022 64bit + Ninja
REM ============================================================

echo [1/4] 激活 MSVC 环境 (vcvars64.bat) ...
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo [ERROR] vcvars64.bat 调用失败
    exit /b 1
)
echo [OK] MSVC 环境已激活
echo.

echo [2/4] 删除旧 build 目录 ...
if exist build rmdir /S /Q build
echo [OK] build 目录已清理
echo.

echo [3/4] CMake configure (Qt6.9.0 MSVC2022_64 + Ninja + Release) ...
"C:\Qt\Tools\CMake_64\bin\cmake.exe" -S . -B build -G Ninja ^
    -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/Ninja/ninja.exe" ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.9.0/msvc2022_64" ^
    -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] CMake configure 失败
    exit /b 1
)
echo [OK] CMake configure 完成
echo.

echo [4/4] CMake build (Release) ...
"C:\Qt\Tools\CMake_64\bin\cmake.exe" --build build --config Release
if errorlevel 1 (
    echo [ERROR] CMake build 失败
    exit /b 1
)
echo.

echo ============================================================
echo [DONE] 全部构建成功！
echo       可执行文件: build\Robot_R06.exe
echo       运行命令  : build\Robot_R06.exe
echo ============================================================
endlocal