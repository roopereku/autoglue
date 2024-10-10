cmake_minimum_required(VERSION 3.5)
project(Autoglue VERSION 1.0)

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE Sources *.cc)

add_library(Autoglue SHARED ${Sources})

target_include_directories(
	Autoglue PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

install(
	TARGETS Autoglue
	EXPORT AutoglueTargets
)

install(
	DIRECTORY include/
	DESTINATION include
)

install(
	EXPORT AutoglueTargets
	FILE AutoglueTargets.cmake
	NAMESPACE Autoglue::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/../cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
	INSTALL_DESTINATION lib/cmake/Autoglue
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
	DESTINATION lib/cmake/Autoglue
)

set(CPACK_PACKAGE_NAME "autoglue-core")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Autoglue Core")
set(CPACK_GENERATOR "ZIP")
include(CPack)
