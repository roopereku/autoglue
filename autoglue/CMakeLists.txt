cmake_minimum_required(VERSION 3.5)
project(autoglue)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE Sources *.cc)

add_library(autoglue ${Sources})
target_include_directories(autoglue PUBLIC include)
