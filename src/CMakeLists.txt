add_executable(${PROJECT_NAME} main.c game.c map.c render.c block.c menu.c timer.c)

if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-subsystem,windows")
    message(STATUS ">>> Windows")
endif()

if (LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-rpath=./") # -Wl is used in gcc, pass the link flag after it
    message(STATUS ">>> Linux")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/inc) 
target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image)
