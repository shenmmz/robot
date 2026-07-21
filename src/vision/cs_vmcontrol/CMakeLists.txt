# src/vision/cs_vmcontrol/CMakeLists.txt
# 视觉模块桥接层：通过 QProcess 启动外部 CS-VMC-Dome.exe
# 不再内嵌 VisionMaster SDK，无 ActiveX / 编码 / DLL 依赖

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(vm_control STATIC
    VisionMasterBridge.h
    VisionMasterBridge.cpp
)

target_include_directories(vm_control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vm_control
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)
