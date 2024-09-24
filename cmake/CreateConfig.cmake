include(CMakePackageConfigHelpers)

if(NOT DEFINED PROJECT_NAME)
	message(FATAL_ERROR "PROJECT_NAME not defined")
endif()

if(NOT DEFINED PROJECT_VERSION)
	message(FATAL_ERROR "PROJECT_VERSION not defined")
endif()

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
	INSTALL_DESTINATION lib/cmake/Autoglue
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
	DESTINATION lib/cmake/Autoglue
)
