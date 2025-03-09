add_executable(${PROJECT_NAME}
    main.cpp
    table.cpp
    chunk.cpp
    tree.cpp
    db.cpp
    cli.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE dp::thread-pool Stopwatch)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include" 
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -fexperimental-library
)
