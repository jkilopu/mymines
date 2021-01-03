# 最低CMake版本要求
cmake_minimum_required(VERSION 3.12.0)

# 项目名称
project(mymines)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# 设置输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

if(WIN32)
    message(STATUS ">>> Windows")
    # Windows must set these(Because libraries are not in a default path)
    if(MSVC)
        message(STATUS ">>> MSVC")
        set(SDL2_PATH "C:/Workspace/3rdLib/SDL2-2.0.14-VC")
        set(SDL2_IMAGE_PATH "C:/Workspace/3rdLib/SDL2_image-2.0.5-VC")
        set(SDL2_NET_PATH "C:/Workspace/3rdLib/SDL2_net-2.0.1-VC")
    elseif(MINGW)
        message(STATUS ">>> MINGW")
        set(SDL2_PATH "C:/Workspace/3rdLib/SDL2-2.0.12/x86_64-w64-mingw32")
        set(SDL2_IMAGE_PATH "C:/Workspace/3rdLib/SDL2_image-2.0.5/x86_64-w64-mingw32")
        set(SDL2_NET_PATH "C:/Workspace/3rdLib/SDL2_net-2.0.1/x86_64-w64-mingw32")
    else()
        message(STATUS ">>> Unknown compiler")
    endif()
elseif(LINUX)
    message(STATUS ">>> Linux")
else()
    message(STATUS ">>> Unknown OS")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_net REQUIRED)

message(STATUS "incDir: ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_NET_INCLUDE_DIRS}")

# 添加子目录，使子目录中的CMakelists.txt被执行
add_subdirectory(prng_lib)
add_subdirectory(src)

# Copy res to bin for Debug 
file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
