cmake_minimum_required(VERSION 3.5)
project(autoglue)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# TODO: Clang could have its own namespace and expose the backend as Autoglue::Clang::Backend.

add_library(ClangBackend SHARED ${Sources})
target_link_libraries(ClangBackend Autoglue LLVM clang-cpp)

target_include_directories(ClangBackend PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(TARGETS ClangBackend
	EXPORT AutoglueTargets
)

install(DIRECTORY include/ DESTINATION include)
