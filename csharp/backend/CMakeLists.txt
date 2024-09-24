cmake_minimum_required(VERSION 3.5)

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(Backend SHARED ${Sources})
target_link_libraries(Backend Autoglue::Autoglue)

target_include_directories(
	Backend PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(
	TARGETS Backend
	EXPORT AutoglueCSharpTargets
)

install(
	DIRECTORY include/
	DESTINATION include
)
