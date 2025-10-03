function(ag_create_package target_name target_description target_namespace)
	install(
		TARGETS ${target_name}
		EXPORT ${target_name}Targets
	)

	install(
		DIRECTORY include/
		DESTINATION include
	)

	install(
		EXPORT ${target_name}Targets
		FILE Autoglue${target_name}Targets.cmake
		NAMESPACE ${target_namespace}::
		DESTINATION ${AUTOGLUE_DESTINATION}
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

	set(CPACK_PACKAGE_NAME ${PROJECT_NAME}${target_name})
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION ${target_description})
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endfunction()
