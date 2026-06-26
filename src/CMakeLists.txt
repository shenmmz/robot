# src/CMakeLists.txt
# 本文件将各个功能模块组织为独立库，便于大规模项目维护。

add_subdirectory(models)
add_subdirectory(hardware)
add_subdirectory(logic)
add_subdirectory(utils)
add_subdirectory(ui)
add_subdirectory(vision)
