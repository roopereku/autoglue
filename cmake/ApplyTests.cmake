function(ag_apply_tests target_name)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	)

	# Don't install GoogleTest to make it not appear in the installation prefix.
	set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	file(GLOB TestSources ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
	add_executable(${target_name}Tests ${TestSources})
	target_link_libraries(${target_name}Tests GTest::gtest_main ${target_name})

	include(GoogleTest)
	gtest_discover_tests(${target_name}Tests)
endfunction()
