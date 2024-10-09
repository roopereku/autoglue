cmake_minimum_required(VERSION 3.5)
project(AutoglueTest)
include(GNUInstallDirs)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../prefix/${CMAKE_INSTALL_LIBDIR}/cmake/Autoglue")
find_package(Autoglue REQUIRED)

add_executable(autogluetest main.cc)
target_link_libraries(autogluetest
	Autoglue::Autoglue
	Autoglue::Java::Generator
	Autoglue::CSharp::Generator
	Autoglue::Clang::Backend
)
