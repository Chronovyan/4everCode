cmake_minimum_required(VERSION 3.10)
project(StandaloneLexerTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(standalone_lexer
    ../test_lexer_standalone.cpp
)

target_include_directories(standalone_lexer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
