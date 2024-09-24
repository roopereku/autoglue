cmake_minimum_required(VERSION 3.5)
project(AutoglueJava VERSION 1.0)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../autoglue/build/prefix")
find_package(Autoglue REQUIRED)

if(DEFINED AUTOGLUE_BUILD_JAVA_GENERATOR AND AUTOGLUE_BUILD_JAVA_GENERATOR)
	add_subdirectory(generator)
	set(AUTOGLUE_INSTALL TRUE)
endif()

if(DEFINED AUTOGLUE_INSTALL)
	install(
		EXPORT AutoglueJavaTargets
		FILE AutoglueJavaTargets.cmake
		NAMESPACE Autoglue::Java::
		DESTINATION lib/cmake/autoglue
	)

	#include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CreateConfig.cmake)
endif()
