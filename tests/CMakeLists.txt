add_executable(tsdb_tests test_insert.cpp)
target_link_libraries(tsdb_tests libs GTest::gtest_main) # Use GTest::gtest_main
target_include_directories(tsdb_tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
)
