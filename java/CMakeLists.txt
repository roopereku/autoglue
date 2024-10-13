cmake_minimum_required(VERSION 3.5)
project(AutoglueJava VERSION 1.0)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake")
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
		DESTINATION ${AUTOGLUE_DESTINATION}
	)

	set(CPACK_PACKAGE_NAME "autoglue-java")
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION "Autoglue Java")
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endif()
