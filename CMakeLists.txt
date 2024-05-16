cmake_minimum_required(VERSION 3.5)
project(autoglue)

set(Autoglue_VERSION 1.0)

add_subdirectory(autoglue)

if(DEFINED AUTOGLUE_BUILD_CLANG_BACKEND AND AUTOGLUE_BUILD_CLANG_BACKEND)
	add_subdirectory(clang)
endif()

if(DEFINED AUTOGLUE_BUILD_JAVA_GENERATOR AND AUTOGLUE_BUILD_JAVA_GENERATOR)
	# TODO: Only add java when a Java backend exists.
	add_subdirectory(java/generator)
endif()

if(DEFINED AUTOGLUE_BUILD_CSHARP_GENERATOR AND AUTOGLUE_BUILD_CSHARP_GENERATOR)
	# TODO: Only add csharp when a C# backend exists.
	add_subdirectory(csharp/generator)
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/AutoglueConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AutoglueConfig.cmake
    INSTALL_DESTINATION lib/cmake/MyLib
)

write_basic_package_version_file(
	"AutoglueConfigVersion.cmake"
	VERSION ${Autoglue_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/AutoglueConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/AutoglueConfigVersion.cmake
    DESTINATION lib/cmake/autoglue
)
