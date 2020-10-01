# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10.0)

# 项目名称
project(mymine)

#设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Windows must set these(Because libraries are not in a default path)
set(SDL2_PATH "C:/Workspace/3rdLib/SDL2-2.0.12/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "C:/Workspace/3rdLib/SDL2_image-2.0.5/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

message("incDir: ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}")
include_directories(${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

#添加子目录，使子目录中的CMakelists.txt被执行
add_subdirectory(src)