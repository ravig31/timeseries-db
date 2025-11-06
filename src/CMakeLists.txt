add_library(libs STATIC
    table.cpp
    chunk.cpp
    tree.cpp
    db.cpp
	cli.cpp
)

target_link_libraries(libs 
    PUBLIC 
        dp::thread-pool 
        Stopwatch
)

target_include_directories(libs
    PUBLIC
        "${PROJECT_SOURCE_DIR}/src/include"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(libs PRIVATE -fexperimental-library)
endif()

# Create main executable
add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        libs
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_SOURCE_DIR}/src/include"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -fexperimental-library)
endif()