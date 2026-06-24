# library/CMakeLists.txt
# 定义 Zmotion SDK 的接口库，供其他模块链接。
add_library(zmotion_sdk INTERFACE)

target_include_directories(zmotion_sdk INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

if(WIN32)
    target_link_libraries(zmotion_sdk INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/zauxdll.lib
        ${CMAKE_CURRENT_LIST_DIR}/zmotion.lib
    )
endif()

# 让 Qt Creator 显示头文件和库文件（可选）。
target_sources(zmotion_sdk INTERFACE
    zauxdll2.h
    zmotion.h
)