cmake_minimum_required(VERSION 3.5)
project(autogluetest)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_PREFIX_PATH}")
find_package(Autoglue REQUIRED)

add_executable(autogluetest main.cc)
target_link_libraries(autogluetest
	Autoglue::Autoglue
	Autoglue::ClangBackend
	Autoglue::JavaGenerator
	Autoglue::CSharpGenerator
)
