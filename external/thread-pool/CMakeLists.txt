message(STATUS "Fetching thread-pool...")
# Set options before making available
set(TP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TP_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(TP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TP_CXX_STANDARD 20 CACHE STRING "Set C++ standard for thread-pool")

FetchContent_MakeAvailable(thread-pool)
