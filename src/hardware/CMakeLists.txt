# src/hardware/CMakeLists.txt
# 定义硬件控制模块。

add_library(robot_hardware STATIC
    MotionController.cpp
    Rs485Controller.cpp
    ServoController.cpp
)

target_include_directories(robot_hardware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(robot_hardware PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::SerialPort
    robot_models
    zmotion_sdk
    zmotion_api
)

target_sources(robot_hardware PUBLIC
    MotionController.h
    Rs485Controller.h
    ServoController.h
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_hardware PRIVATE /utf-8)
endif()
