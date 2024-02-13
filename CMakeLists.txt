cmake_minimum_required(VERSION 3.5)
project(autoglue)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_subdirectory(autoglue)

if(DEFINED build_clang_backend AND build_clang_backend)
	add_subdirectory(clang)
endif()

if(DEFINED build_java_generator AND build_java_generator)
	add_subdirectory(java/generator)
endif()
