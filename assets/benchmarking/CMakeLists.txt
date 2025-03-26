if(BUILD_BENCHMARK)
  add_executable(
    benchmark
    benchmark.cpp ${PROJECT_SOURCE_DIR}/src/table.cpp
    ${PROJECT_SOURCE_DIR}/src/chunk.cpp ${PROJECT_SOURCE_DIR}/src/tree.cpp
    ${PROJECT_SOURCE_DIR}/src/db.cpp)
  target_link_libraries(benchmark PRIVATE dp::thread-pool Stopwatch)
  target_include_directories(benchmark
                             PRIVATE "${PROJECT_SOURCE_DIR}/src/include")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(benchmark PRIVATE -fexperimental-library)
  endif()

endif()
