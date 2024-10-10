cmake_minimum_required(VERSION 3.5)
project(autoglue)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(SerializationBackend SHARED ${Sources})
target_link_libraries(SerializationBackend Autoglue)

target_include_directories(SerializationBackend PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(TARGETS SerializationBackend
	EXPORT AutoglueTargets
)

install(DIRECTORY include/ DESTINATION include)
