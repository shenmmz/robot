cmake_minimum_required(VERSION 3.21)

project(VisualRoboticArmQtController VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 路径不写死到某一台电脑，移植到新电脑时在 CMake 配置中传入：
#   -DQt6_DIR=C:/Qt/6.x.x/msvc2022_64/lib/cmake/Qt6
#   -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2022_64
# 也可以在 Qt Creator / Visual Studio 的 CMake 设置中填写同样的值。

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

add_executable(VisualRoboticArmQtController
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/LocalCommBridge.h
    src/LocalCommBridge.cpp
    src/PythonProcessController.h
    src/PythonProcessController.cpp
    src/JsonProtocol.h
)

target_link_libraries(VisualRoboticArmQtController
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)

if (MSVC)
    target_compile_options(VisualRoboticArmQtController PRIVATE /utf-8)
endif()

set_target_properties(VisualRoboticArmQtController PROPERTIES
    WIN32_EXECUTABLE ON
)
