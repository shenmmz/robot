# src/models/CMakeLists.txt
# 定义机器人数据模型模块。

add_library(robot_models STATIC
    RobotState.cpp
    RobotConfig.cpp
    RobotKinematics.cpp
    RobotModels.cpp
)

target_include_directories(robot_models PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(robot_models PUBLIC
    RobotState.h
    RobotConfig.h
    RobotKinematics.h
    RobotModels.h
    RobotModelId.h
)

# 链接 Qt 模块
target_link_libraries(robot_models PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_models PRIVATE /utf-8)
endif()
