# Set the install destination,
include(GNUInstallDirs)
set(AUTOGLUE_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue")

# Point CMake to the prefix for locating Autoglue dependencies.
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../prefix")

# Set a C++ standard.
set(CMAKE_CXX_STANDARD 17)

function(ag_apply_target_options target_name)
	# Enable most warnings and set a low tolerance for them.
	if(MSVC)
	  target_compile_options(${target_name} PRIVATE /W4 /WX)
	else()
	  target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endfunction()
