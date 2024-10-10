cmake_minimum_required(VERSION 3.5)
project(AutoglueJava VERSION 1.0)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../prefix")
find_package(Autoglue REQUIRED)
include(GNUInstallDirs)

if(DEFINED AUTOGLUE_BUILD_JAVA_GENERATOR AND AUTOGLUE_BUILD_JAVA_GENERATOR)
	add_subdirectory(generator)
	set(AUTOGLUE_INSTALL TRUE)
endif()

if(DEFINED AUTOGLUE_INSTALL)
	install(
		EXPORT AutoglueJavaTargets
		FILE AutoglueJavaTargets.cmake
		NAMESPACE Autoglue::Java::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue
	)

	set(CPACK_PACKAGE_NAME "autoglue-java")
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION "Autoglue Java")
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endif()
