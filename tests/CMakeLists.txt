add_executable(tsdb_tests 
    test_basic.cpp
)

target_link_libraries(tsdb_tests 
    PRIVATE
        libs
        gtest_main
)

target_include_directories(tsdb_tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/include"
        "${PROJECT_SOURCE_DIR}"
)

add_test(NAME tsdb_tests COMMAND tsdb_tests)