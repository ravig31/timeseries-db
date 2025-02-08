cmake_minimum_required(VERSION 3.15)
project(tsdb CXX)

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add executable
add_executable(PROJECT_NAME
    src/main.cpp
)

# Include directories
target_include_directories(PROJECT_NAME PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
