cmake_minimum_required(VERSION 3.12.0)

project(prng)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/prng_alleged_rc4.c ${CMAKE_CURRENT_SOURCE_DIR}/src/prng_lcg.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(PRNG::prng ALIAS ${PROJECT_NAME})
