# Create an INTERFACE library (a library that doesn't produce an artifact)
add_library(Stopwatch INTERFACE)

# Specify include directories for the INTERFACE library
target_include_directories(Stopwatch
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}  # Path relative to *this* CMakeLists.txt
)