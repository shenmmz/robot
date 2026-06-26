# src/vision/CMakeLists.txt
# 外部 VisualRoboticArmQtController.exe 子进程控制器。
# 用 QProcess 启动/监控外部主控程序，自身只做进程管理，不重复实现 TCP/Python 逻辑。

add_library(robot_vision STATIC
    VisionController.cpp
)

target_include_directories(robot_vision PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(robot_vision PUBLIC
    Qt${QT_VERSION_MAJOR}::Core   # QObject / QProcess
)

target_sources(robot_vision PUBLIC
    VisionController.h
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_vision PRIVATE /utf-8)
endif()