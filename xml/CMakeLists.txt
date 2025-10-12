cmake_minimum_required(VERSION 3.11)
project(Autoglue VERSION 1.0)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake")

find_package(tinyxml2 REQUIRED)
ag_create_target(XML)
target_link_libraries(XML tinyxml2)

ag_create_package(XML "Autoglue XML" Autoglue::XML)
ag_apply_tests(XML)
