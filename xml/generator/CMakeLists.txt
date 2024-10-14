cmake_minimum_required(VERSION 3.5)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake")

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
add_library(Serializer SHARED ${Sources})
ag_apply_target_options(Serializer)
target_link_libraries(Serializer
	Autoglue::Autoglue
	tinyxml2
)

target_include_directories(
	Serializer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

set_target_properties(
	Serializer PROPERTIES
	OUTPUT_NAME "XmlSerializer"
)

install(
	TARGETS Serializer
	EXPORT AutoglueXmlTargets
)

install(
	DIRECTORY include/
	DESTINATION include
)
