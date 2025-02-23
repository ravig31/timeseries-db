cmake_minimum_required(VERSION 3.15)
project(tsdb VERSION 1.0.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/table.cpp
    src/chunk.cpp
    src/tree.cpp
    src/db.cpp
)

option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_ASAN "Address Thread Sanitizer" OFF)

if(ENABLE_ASAN)
    message(STATUS "Address Sanitizer enabled")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -g -O1)
    target_link_libraries(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()

if(ENABLE_TSAN)
    message(STATUS "Thread Sanitizer enabled")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=thread -g -O1)
    target_link_libraries(${PROJECT_NAME} PRIVATE -fsanitize=thread)
endif()

# --- General Compiler Flags ---
# Add these *after* the TSan flags, so TSan flags take precedence.
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)