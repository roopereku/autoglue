function(ag_create_target target_name)
	file(GLOB_RECURSE Sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
	add_library(${target_name} SHARED ${Sources})

	target_include_directories(
		${target_name} PUBLIC  
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
		$<INSTALL_INTERFACE:include>
	)

	# Depend on core when the current target is not the core.
	if(NOT ${target_name} STREQUAL "Core")
		find_package(Autoglue REQUIRED CONFIG)
		target_link_libraries(${target_name} Autoglue::Core)
	endif()

	# Enable most warnings and set a low tolerance for them.
	if(MSVC)
	  target_compile_options(${target_name} PRIVATE /W4 /WX)
	else()
	  target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endfunction()
