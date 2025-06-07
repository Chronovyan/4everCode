cmake_minimum_required(VERSION 3.14)
project(parser_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Source files - include only the minimal set needed
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/source_location.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/source_file.cpp
)

# Add test executable
add_executable(parser_test
    parser_test.cpp
    ${SOURCE_FILES}
)

# Link against Google Test
target_link_libraries(parser_test
    gtest
    gtest_main
)

# Enable testing
enable_testing()
add_test(NAME parser_test COMMAND parser_test) 