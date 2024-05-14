cmake_minimum_required(VERSION 3.5)
project(autoglue)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE Sources *.cc)

add_library(Autoglue SHARED ${Sources})

target_include_directories(Autoglue PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(TARGETS Autoglue
	EXPORT AutoglueTargets
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT AutoglueTargets
	FILE AutoglueTargets.cmake
	NAMESPACE Autoglue::
	DESTINATION lib/cmake/autoglue
)
