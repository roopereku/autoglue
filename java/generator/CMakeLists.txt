cmake_minimum_required(VERSION 3.5)
project(java_generator)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(java_generator ${Sources})
target_link_libraries(java_generator gen)
target_include_directories(java_generator PUBLIC include)
