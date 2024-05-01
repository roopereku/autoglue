cmake_minimum_required(VERSION 3.5)
project(autogluetest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Autoglue REQUIRED)

add_executable(autogluetest main.cc)
target_link_libraries(autogluetest
	Autoglue::Autoglue
	Autoglue::ClangBackend
	Autoglue::JavaGenerator
)
