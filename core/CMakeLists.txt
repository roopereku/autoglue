cmake_minimum_required(VERSION 3.11)
project(Autoglue VERSION 1.0)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake")

ag_create_target(Core)
ag_create_package(Core "Autoglue Core" Autoglue)
ag_apply_tests(Core)
