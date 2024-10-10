cmake_minimum_required(VERSION 3.5)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(Deserializer SHARED ${Sources})
target_link_libraries(Deserializer Autoglue::Autoglue)

target_include_directories(
	Deserializer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

set_target_properties(
	Deserializer PROPERTIES
	OUTPUT_NAME "XmlDeserializer"
)

install(
	TARGETS Deserializer
	EXPORT AutoglueXmlTargets
)

install(
	DIRECTORY include/
	DESTINATION include
)
