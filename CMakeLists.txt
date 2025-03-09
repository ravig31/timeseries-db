cmake_minimum_required(VERSION 3.15)
project(tsdb VERSION 1.0.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(external)

option(BUILD_BENCHMARK "Build benchmark executable" ON)
if (BUILD_BENCHMARK)
    add_subdirectory(assets)
endif()



option(BUILD_TESTS "Build tests" OFF)
# Testing
if (BUILD_TESTS)
    include(CTest)
    add_test(NAME tsdb_unit_tests COMMAND tsdb_tests)
    add_subdirectory(tests) # add the tests directory.
endif ()

# Options
