cmake_minimum_required(VERSION 3.5)
project(gen)

add_subdirectory(gen)

if(NOT DEFINED build_clang_backend)
	set(build_clang_backend OFF)
endif()

if(build_clang_backend)
	add_subdirectory(clang)
endif()
