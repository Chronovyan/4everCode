cmake_minimum_required(VERSION 3.14)
project(chronovyan_lexer_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Download and build Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Find source files
file(GLOB_RECURSE COMPILER_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler/src/Diagnostic.cpp
)

# Add test source files
set(TEST_SOURCES
    lexer_comprehensive_test.cpp
    chronoscript_lexer_test.cpp
)

# Create test executables for each test file
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Extract the test name from the filename (remove .cpp)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Add the test executable
    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
        ${COMPILER_SOURCES}
    )
    
    # Set compiler flags
    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)
    endif()
    
    # Link Google Test
    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main)
    
    # Add the test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Enable CTest verbose output
    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1")
    
    # Make sure the test is built when running the test target
    # Note: The 'test' target is created by enable_testing()
endforeach()

# Add standalone lexer tool
add_executable(chronoscript_lexer
    test_lexer.cpp
    ${COMPILER_SOURCES}
)

# Set compiler flags for the standalone lexer tool
if(MSVC)
    target_compile_options(chronoscript_lexer PRIVATE /W4 /WX)
else()
    target_compile_options(chronoscript_lexer PRIVATE -Wall -Wextra -Werror)
endif()

# Link libraries for the standalone lexer tool
target_link_libraries(chronoscript_lexer
    PRIVATE
    GTest::gtest
)

# Enable testing
enable_testing()

# Make CTest verbose
set(CTEST_OUTPUT_ON_FAILURE ON)
