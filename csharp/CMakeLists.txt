cmake_minimum_required(VERSION 3.5)
project(AutoglueCSharp VERSION 1.0)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake")
find_package(Autoglue REQUIRED)

if(DEFINED AUTOGLUE_BUILD_CSHARP_GENERATOR AND AUTOGLUE_BUILD_CSHARP_GENERATOR)
	add_subdirectory(generator)
	set(AUTOGLUE_INSTALL TRUE)
endif()

if(DEFINED AUTOGLUE_INSTALL)
	install(
		EXPORT AutoglueCSharpTargets
		FILE AutoglueCSharpTargets.cmake
		NAMESPACE Autoglue::CSharp::
		DESTINATION ${AUTOGLUE_DESTINATION}
	)

	set(CPACK_PACKAGE_NAME "autoglue-csharp")
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION "Autoglue C#")
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endif()
