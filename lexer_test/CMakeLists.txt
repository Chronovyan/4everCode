cmake_minimum_required(VERSION 3.5)

project(lexer_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    test_lexer.cpp
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src
)

# Add the test executable
add_executable(lexer_test ${SOURCES})

# Add compiler source files
target_sources(lexer_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Diagnostic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Token.cpp
)

# Set output directory
set_target_properties(lexer_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
