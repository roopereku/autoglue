cmake_minimum_required(VERSION 3.5)
project(JavaGenerator)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# TODO: Once a Java backend exists, maybe expose the generator as Autoglue::Java::Generator?

add_library(JavaGenerator SHARED ${Sources})
target_link_libraries(JavaGenerator Autoglue)

target_include_directories(JavaGenerator PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(TARGETS JavaGenerator
	EXPORT AutoglueTargets
)

install(DIRECTORY include/ DESTINATION include)
