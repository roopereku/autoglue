cmake_minimum_required(VERSION 3.5)
project(backend_clang)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(backend_clang ${Sources})
target_link_libraries(backend_clang gen)
target_include_directories(backend_clang PUBLIC include)
