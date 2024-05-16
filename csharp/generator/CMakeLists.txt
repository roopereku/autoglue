cmake_minimum_required(VERSION 3.5)
project(CSharpGenerator)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# TODO: Once a C# backend exists, maybe expose the generator as Autoglue::CSharp::Generator?

add_library(CSharpGenerator SHARED ${Sources})
target_link_libraries(CSharpGenerator Autoglue)

target_include_directories(CSharpGenerator PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(TARGETS CSharpGenerator
	EXPORT AutoglueTargets
)

install(DIRECTORY include/ DESTINATION include)
