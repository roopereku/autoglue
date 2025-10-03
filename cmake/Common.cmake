# Set the install destination,
include(GNUInstallDirs)
set(AUTOGLUE_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue")

# Point CMake to the prefix for locating Autoglue dependencies.
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../prefix")

# Set a C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cannot be called inside a function so let's do this here for CTest to work.
enable_testing()

include("${CMAKE_CURRENT_LIST_DIR}/CreateTarget.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/CreatePackage.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ApplyTests.cmake")
