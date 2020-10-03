add_executable(${PROJECT_NAME} main.c game.c map.c render.c block.c menu.c)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wl,-subsystem,windows")
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/inc ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})