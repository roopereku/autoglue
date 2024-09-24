cmake_minimum_required(VERSION 3.5)
project(AutoglueCSharp VERSION 1.0)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../autoglue/build/prefix")
find_package(Autoglue REQUIRED)

if(DEFINED AUTOGLUE_BUILD_CSHARP_GENERATOR AND AUTOGLUE_BUILD_CSHARP_GENERATOR)
	add_subdirectory(generator)
	set(AUTOGLUE_INSTALL TRUE)
endif()

if(DEFINED AUTOGLUE_BUILD_CSHARP_BACKEND AND AUTOGLUE_BUILD_CSHARP_BACKEND)
	add_subdirectory(backend)
	set(AUTOGLUE_INSTALL TRUE)
endif()

if(DEFINED AUTOGLUE_INSTALL)
	install(
		EXPORT AutoglueCSharpTargets
		FILE AutoglueCSharpTargets.cmake
		NAMESPACE Autoglue::CSharp::
		DESTINATION lib/cmake/autoglue
	)

	#include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CreateConfig.cmake)
endif()
