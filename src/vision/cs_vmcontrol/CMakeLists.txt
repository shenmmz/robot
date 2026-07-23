# src/vision/cs_vmcontrol/CMakeLists.txt
# 视觉模块：VisionMaster SDK 静态库内嵌 + QProcess 桥接（备选）

# ── VM SDK 路径 ──────────────────────────────────────────────
set(VM_SDK_DIR ${CMAKE_SOURCE_DIR}/external/CS-VMC-Dome/libs/VisonMaster_SDK4.4)
set(VM_INCLUDE_DIRS ${VM_SDK_DIR}/include)
set(VM_LIB_DIR ${VM_SDK_DIR}/lib)
set(VM_BIN_DIR ${VM_SDK_DIR}/bin)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(vm_control STATIC
    # 原 CS-VMC-Dome VisionController（静态库内嵌）
    vmtypes.h
    vmbase.h
    vmbase.cpp
    vmcontrol.h
    vmcontrol.cpp
    VisionRobotLink.h
    VisionRobotLink.cpp
)

target_include_directories(vm_control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(vm_control PRIVATE
    ${VM_INCLUDE_DIRS}
    ${VM_SDK_DIR}/QT
)

# PUBLIC: 任何通过 vmbase.h / vmcontrol.h 间接引用 SDK 头文件的目标也需要这些路径
target_include_directories(vm_control PUBLIC
    ${VM_INCLUDE_DIRS}
    ${VM_SDK_DIR}/QT
)

target_link_directories(vm_control PUBLIC
    ${VM_LIB_DIR}
)

target_link_libraries(vm_control
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        iMVS-6000PlatformSDK.lib
)

# VM SDK 头文件为 GBK 编码，与 UTF-8 源文件混编产生 C4828/C4091 警告，无害故全局屏蔽
if(MSVC)
    target_compile_options(vm_control PRIVATE /wd4828 /wd4091)
endif()

# vmbase.cpp / vmcontrol.cpp 需要 /utf-8 保证与 SDK GBK 头文件兼容，同时 /Zc:__cplusplus 用于 Qt6
if(MSVC)
    set_source_files_properties(
        vmbase.cpp vmcontrol.cpp
        PROPERTIES COMPILE_FLAGS "/utf-8 /Zc:__cplusplus"
    )
endif()
