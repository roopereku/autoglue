cmake_minimum_required(VERSION 3.5)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(Generator SHARED ${Sources})
ag_apply_target_options(Generator)
target_link_libraries(Generator Autoglue::Autoglue)

target_include_directories(
	Generator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

set_target_properties(
	Generator PROPERTIES
	OUTPUT_NAME "JavaGenerator"
)

install(
	TARGETS Generator
	EXPORT AutoglueJavaTargets
)

install(
	DIRECTORY include/
	DESTINATION include
)
