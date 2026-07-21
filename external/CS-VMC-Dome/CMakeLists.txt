cmake_minimum_required(VERSION 3.19)
project(CS-VMC-Dome LANGUAGES CXX)

# 设置博派的API路径
set(BoPai_DIR ${CMAKE_SOURCE_DIR}/libs/bopai_MSVC)
set(BoPai_INCLUDE_DIRS ${BoPai_DIR}/include)
set(BoPai_LIB_DIR ${BoPai_DIR}/lib)

#设置海康VisionMaster的SDK路径,需要根据本地电脑路径进行修改(本地安装版本)
#本地仓库-已全部转换为utf-8编码
set(VM_DIR ${CMAKE_SOURCE_DIR}/libs/VisonMaster_SDK4.4)
set(VM_INCLUDE_DIRS ${VM_DIR}/include)
set(VM_LIB_DIR ${VM_DIR}/lib)
set(VM_Bin_DIR ${VM_DIR}/bin)
set(VM_QT_DIR ${VM_DIR}/QT)


find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    SerialPort #串口通信模块
    AxContainer #海康VM控件需要
    OPTIONAL_COMPONENTS
    Core5Compat
)

qt_standard_project_setup()

#屏蔽错误警告
if (MSVC)
    add_compile_options(/wd4091 /wd4828)
endif()

qt_add_executable(CS-VMC-Dome
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    VisionController/vmcontrol.h
    VisionController/vmcontrol.cpp
    VisionController/vmtypes.h
    VisionController/vmbase.h
    VisionController/vmbase.cpp

    ${RESOURCES}
)

#添加头文件路径,必须在qt_add_executable之后
target_include_directories(CS-VMC-Dome PRIVATE
    ${BoPai_INCLUDE_DIRS} #博派API的头文件
    ${VM_INCLUDE_DIRS} #海康的VisionMaster的SDK的头文件
)

#添加静态库文件路径
target_link_directories(CS-VMC-Dome PRIVATE
    ${BoPai_LIB_DIR}
    ${VM_LIB_DIR}
)
target_link_libraries(CS-VMC-Dome
    PRIVATE
        Qt::Core
        Qt::Widgets
)

target_link_libraries(CS-VMC-Dome
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SerialPort #串口通信库
        Qt::AxContainer
        # $<$<TARGET_EXISTS:Qt5::Core>:Qt5::Core>
        # $<$<TARGET_EXISTS:Qt5::Widgets>:Qt5::Widgets>
        # $<$<TARGET_EXISTS:Qt5::SerialPort>:Qt5::SerialPort>
        # $<$<TARGET_EXISTS:Qt5::AxContainer>:Qt5::AxContainer>
        MultiCard.lib   #博派的静态库文件
        iMVS-6000PlatformSDK.lib   #海康VM的SDK的静态链接库
)

include(GNUInstallDirs)

install(TARGETS CS-VMC-Dome
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 复制 DLL 和数据文件到输出目录
add_custom_command(TARGET CS-VMC-Dome POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${BoPai_DIR}/bin/MultiCard.dll
        ${VM_Bin_DIR}/my_cross_platform.dll
        #${CMAKE_SOURCE_DIR}/defaultHGProjFile/default.HGproj
        $<TARGET_FILE_DIR:CS-VMC-Dome>
)


qt_generate_deploy_app_script(
    TARGET CS-VMC-Dome
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
