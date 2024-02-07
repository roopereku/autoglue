cmake_minimum_required(VERSION 3.5)
project(backend_clang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

add_library(backend_clang ${Sources})
target_link_libraries(backend_clang gen LLVM clang-cpp)
target_include_directories(backend_clang PUBLIC include)
