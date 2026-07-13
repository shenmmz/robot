cmake_minimum_required(VERSION 3.16)

project(Robot_R06 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 项目使用 C++17 标准。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt Widgets 模块，优先使用 Qt6，如果没有则回退到 Qt5。
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort Network)

# 根 CMakeLists 必须保持简洁，大型项目应使用子目录模块。
add_subdirectory(library)
add_subdirectory(ZmotionAPI)
add_subdirectory(src)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    PointAdjustDialog.ui
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Robot_R06
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# 如果使用 Qt 6 并且目标是 Android，可以在此设置 Android 包源目录：
#    set_property(TARGET Robot_R06 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# 更多信息请参见：https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Robot_R06 SHARED
            ${PROJECT_SOURCES}
        )
# 如果使用 Qt 5 并且目标是 Android，可在 find_package() 调用后设置 Android 包源目录：
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Robot_R06
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Robot_R06 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    robot_models
    robot_hardware
    robot_logic
    robot_utils
    robot_ui
    robot_vision         # 外部 VisualRoboticArmQtController.exe 子进程管理
    zmotion_sdk
)

target_include_directories(Robot_R06 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Qt 6.1 之后，iOS 平台会自动设置 MACOSX_BUNDLE_GUI_IDENTIFIER。
# 如果你是开发 iOS 或 macOS 应用，建议手动设置一个固定的 bundle identifier。
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Robot_R06)
endif()
set_target_properties(Robot_R06 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
)

include(GNUInstallDirs)
install(TARGETS Robot_R06
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ============================================================
# 编译时将 SDK 的 DLL/LIB 文件拷贝到构建输出目录
# ============================================================
set(SDK_LIB_DIR ${CMAKE_SOURCE_DIR}/library)
set(SDK_FILES
    zauxdll.dll
    zmotion.dll
    zauxdll.lib
    zmotion.lib
)

foreach(SDK_FILE ${SDK_FILES})
    set(SRC_FILE ${SDK_LIB_DIR}/${SDK_FILE})
    if(EXISTS ${SRC_FILE})
        add_custom_command(TARGET Robot_R06 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SRC_FILE}"
                "$<TARGET_FILE_DIR:Robot_R06>/${SDK_FILE}"
            COMMENT "Copying ${SDK_FILE} to build output directory"
        )
    else()
        message(WARNING "SDK file not found: ${SRC_FILE}")
    endif()
endforeach()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Robot_R06)
endif()

# ============================================================
# 构建后自动部署 Qt 运行时依赖（仅 Windows + Qt6）
# 每次编译后自动执行 windeployqt 并创建 qt.conf
# ============================================================
if(WIN32 AND QT_VERSION_MAJOR EQUAL 6)
    get_filename_component(_qt6_cmake_dir "${Qt6_DIR}" DIRECTORY)
    get_filename_component(_qt6_lib_dir "${_qt6_cmake_dir}" DIRECTORY)
    get_filename_component(_qt6_prefix "${_qt6_lib_dir}" DIRECTORY)
    set(_windeployqt "${_qt6_prefix}/bin/windeployqt.exe")

    if(EXISTS "${_windeployqt}")
        file(WRITE "${CMAKE_BINARY_DIR}/deploy_qt.cmake"
"execute_process(COMMAND \"${_windeployqt}\" \"\${TARGET_FILE}\" --no-translations RESULT_VARIABLE _ret)
if(NOT _ret EQUAL 0)
    message(WARNING \"windeployqt failed with exit code \${_ret}\")
endif()
file(WRITE \"\${TARGET_DIR}/qt.conf\" \"[Paths]\\nPlugins = .\\n\")
")
        add_custom_command(TARGET Robot_R06 POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -DTARGET_FILE="$<TARGET_FILE:Robot_R06>"
                -DTARGET_DIR="$<TARGET_FILE_DIR:Robot_R06>"
                -P "${CMAKE_BINARY_DIR}/deploy_qt.cmake"
            COMMENT "Deploying Qt runtime (windeployqt + qt.conf)"
        )
    else()
        message(WARNING "windeployqt not found: ${_windeployqt}")
    endif()
endif()
