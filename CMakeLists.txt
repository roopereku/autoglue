cmake_minimum_required(VERSION 3.5)
project(gen)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_subdirectory(gen)

if(NOT DEFINED build_clang_backend)
	set(build_clang_backend OFF)
endif()

if(build_clang_backend)
	add_subdirectory(clang)
endif()
