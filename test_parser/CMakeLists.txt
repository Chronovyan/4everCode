cmake_minimum_required(VERSION 3.14)
project(ChronovyanParserTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include only lexer and its dependencies for now
set(PARSER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Diagnostic.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/DiagnosticReporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Token.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Lexer.cpp"
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ast_nodes
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/bytecode
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/stdlib/collections
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/stdlib/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/stdlib/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/interpreter
)

# Add the minimal test executables
add_executable(minimal_parser_test
    ${PARSER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/minimal_parser_test.cpp
)

add_executable(test_lexer
    ${PARSER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_lexer.cpp
)

# Include directories
target_include_directories(minimal_parser_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_include_directories(test_lexer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Set compiler flags
if(MSVC)
    target_compile_options(minimal_parser_test PRIVATE /W4 /WX- /EHsc)
    target_compile_options(test_lexer PRIVATE /W4 /WX- /EHsc)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(minimal_parser_test PRIVATE -Wall -Wextra -Wno-error)
    target_compile_options(test_lexer PRIVATE -Wall -Wextra -Wno-error)
endif()

# Copy the test file to the build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test.chron
    ${CMAKE_CURRENT_BINARY_DIR}/test.chron
    COPYONLY
)

# Enable testing
enable_testing()
add_test(NAME minimal_parser_test COMMAND minimal_parser_test)
