# src/logic/CMakeLists.txt
# 定义业务逻辑模块。

add_library(robot_logic STATIC
    RobotController.cpp
    HomingController.cpp
    UiController.cpp
    TaskExecutor.cpp
)

target_include_directories(robot_logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../models
    ${CMAKE_CURRENT_SOURCE_DIR}/../hardware
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(robot_logic PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    robot_models
    robot_hardware
    zmotion_api
)

target_sources(robot_logic PUBLIC
    RobotController.h
    HomingController.h
    UiController.h
    TaskExecutor.h
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_logic PRIVATE /utf-8)
endif()
