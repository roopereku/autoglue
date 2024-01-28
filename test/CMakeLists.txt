cmake_minimum_required(VERSION 3.5)
project(gentest)

set(build_clang_backend ON)
set(build_java_generator ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(../ gen)

add_executable(gentest main.cc)
target_link_libraries(gentest gen backend_clang java_generator)
