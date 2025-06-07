cmake_minimum_required(VERSION 3.15)
project(ast_printer_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the source and include directories
set(COMPILER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src/compiler/src)
set(COMPILER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src/compiler/include)

# Include directories
include_directories(${COMPILER_INCLUDE_DIR})

# Source files for the test
set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../ast_printer_test.cpp
)

# Source files for the compiler components we need
set(COMPILER_SOURCES
    ${COMPILER_SRC_DIR}/Token.cpp
    ${COMPILER_SRC_DIR}/AstNode.cpp
    ${COMPILER_SRC_DIR}/AstPrinter.cpp
)

# Create the executable
add_executable(ast_printer_test ${TEST_SOURCES} ${COMPILER_SOURCES})

# Set output directory
set_target_properties(ast_printer_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
) 