add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.c game.c map.c render.c block.c menu.c cursor.c timer.c fatal.c net.c)

if (WIN32)
    if(MINGW)
        target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-subsystem,windows")
    endif()
endif()

if (LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-rpath=./") # -Wl is used in gcc, pass the link flag after it
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/inc) 
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main SDL2::Image SDL2::Net)
target_link_libraries(${PROJECT_NAME} PRIVATE PRNG::prng)
