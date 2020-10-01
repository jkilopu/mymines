set(CMAKE_C_FLAGS "-Wl,-subsystem,windows")
include_directories(../inc)
add_executable(main main.c game.c map.c render.c block.c menu.c)
target_link_libraries(main ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})