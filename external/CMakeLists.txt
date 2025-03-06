set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

# Google Test
if (BUILD_TESTS)
    FetchContent_Declare(
      googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
  )
  add_subdirectory(googletest)
endif()

# Threadpool
FetchContent_Declare(
  thread-pool
  GIT_REPOSITORY https://github.com/DeveloperPaul123/thread-pool.git
  GIT_TAG 0.7.0 
)

add_subdirectory(thread-pool)