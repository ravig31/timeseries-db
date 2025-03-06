add_executable(${PROJECT_NAME}
    main.cpp
    table.cpp
    chunk.cpp
    tree.cpp
    db.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE dp::thread-pool)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include" 
)