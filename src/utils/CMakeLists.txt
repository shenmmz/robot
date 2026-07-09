# src/utils/CMakeLists.txt
# 定义通用工具模块。

add_library(robot_utils STATIC
    Logger.cpp
    CodeEditor.cpp
    OsKEventFilter.cpp
)

target_include_directories(robot_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(robot_utils PUBLIC
    Logger.h
    CodeEditor.h
    OsKEventFilter.h
)

# 链接 Qt 模块
target_link_libraries(robot_utils PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(robot_utils PRIVATE /utf-8)
endif()
