# ZmotionAPI/CMakeLists.txt
# 定义 Zmotion C SDK 的 C++ 封装层静态库。

add_library(zmotion_api STATIC
    ZmotionAPI.cpp
)

target_include_directories(zmotion_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zmotion_api PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    zmotion_sdk
)

target_sources(zmotion_api PUBLIC
    ZmotionAPI.h
)

# 设置源文件编码为 UTF-8
if(MSVC)
    target_compile_options(zmotion_api PRIVATE /utf-8)
endif()
