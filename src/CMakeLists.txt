add_library(libs
    table.cpp
    chunk.cpp
    tree.cpp
    db.cpp
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} libs)

target_include_directories(libs
    PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include" 
)