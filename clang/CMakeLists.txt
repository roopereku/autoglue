cmake_minimum_required(VERSION 3.5)
project(AutoglueClang VERSION 1.0)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../prefix")
find_package(Autoglue REQUIRED)
include(GNUInstallDirs)

if(DEFINED AUTOGLUE_BUILD_CLANG_BACKEND AND AUTOGLUE_BUILD_CLANG_BACKEND)
	if(CMAKE_EXPORT_COMPILE_COMMANDS)
		set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
	endif()

	file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

	find_package(LLVM REQUIRED CONFIG)
	find_package(Clang REQUIRED CONFIG)

	add_library(Backend SHARED ${Sources})
	target_link_libraries(Backend Autoglue::Autoglue LLVM clang-cpp)

	target_include_directories(
		Backend PUBLIC  
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
		$<INSTALL_INTERFACE:include>
	)

	set_target_properties(
		Backend PROPERTIES
		OUTPUT_NAME "ClangBackend"
	)

	install(
		TARGETS Backend
		EXPORT AutoglueClangTargets
	)

	install(
		DIRECTORY include/
		DESTINATION include
	)

	install(
		EXPORT AutoglueClangTargets
		FILE AutoglueClangTargets.cmake
		NAMESPACE Autoglue::Clang::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue
	)

	set(CPACK_PACKAGE_NAME "autoglue-clang")
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION "Autoglue Clang")
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endif()
