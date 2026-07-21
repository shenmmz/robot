# src/vision/CMakeLists.txt
# 外部 VisualRoboticArmQtController.exe 子进程控制器。
# 用 QProcess 启动/监控外部主控程序，自身只做进程管理，不重复实现 TCP/Python 逻辑。

# ---------------------------------------------------------------------------
# 基础视觉控制器（外部 Python 视觉程序管理）- 使用 UTF-8 编码
# ---------------------------------------------------------------------------
add_library(robot_vision STATIC
    VisualRoboticArm/VisionController.cpp
)

target_include_directories(robot_vision PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/VisualRoboticArm
)

target_link_libraries(robot_vision PUBLIC
    Qt${QT_VERSION_MAJOR}::Core     # QObject / QProcess / QTimer
    Qt${QT_VERSION_MAJOR}::Network  # QTcpSocket
)

target_sources(robot_vision PUBLIC
    VisualRoboticArm/VisionController.h
)

# ---------------------------------------------------------------------------
# cs_vmcontrol: VisionMaster 桥接模块（CS-VMC-Dome 适配层）
# ---------------------------------------------------------------------------
add_subdirectory(cs_vmcontrol)