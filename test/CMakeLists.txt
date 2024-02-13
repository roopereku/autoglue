cmake_minimum_required(VERSION 3.5)
project(autogluetest)

set(build_clang_backend ON)
set(build_java_generator ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(../ autoglue)

add_executable(autogluetest main.cc)
target_link_libraries(autogluetest autoglue backend_clang java_generator)
