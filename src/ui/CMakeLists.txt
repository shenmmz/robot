# src/ui/CMakeLists.txt
# 定义UI组件库（目前只含舵机调试组件）。

add_library(robot_ui STATIC
    ServoTestWidget.cpp
    ServoTestWidgetKeypad.cpp
)

target_include_directories(robot_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(robot_ui PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    robot_hardware   # ServoController 在该库中定义
)

target_sources(robot_ui PUBLIC
    ServoTestWidget.h
    ServoTestWidgetKeypad.h
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_ui PRIVATE /utf-8)
endif()
