cmake_minimum_required(VERSION 3.15)
project(parser_test_standalone)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include project directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Define the source files we need
set(PARSER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ast_nodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/source_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/error_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/source_location.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/interpreter.cpp
)

# Copy the simple_parser_test.cpp file to our directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../simple_parser_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simple_parser_test.cpp
    COPYONLY
)

# Add the test executable
add_executable(parser_test
    ${CMAKE_CURRENT_SOURCE_DIR}/simple_parser_test.cpp
    ${PARSER_SOURCES}
)

# Link against Google Test
target_link_libraries(parser_test
    gtest
    gtest_main
)

# Enable testing
enable_testing()

# Add the test
add_test(NAME parser_test COMMAND parser_test) 