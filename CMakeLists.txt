cmake_minimum_required(VERSION 3.10)
project(Chronovyan VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find required packages
find_package(Threads REQUIRED)

# Set compiler flags
if(MSVC)
    add_compile_options(
        /W4         # Warning level 4
        /WX         # Warnings as errors
        /permissive- # Standards conformance
    )
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Werror
        -fexceptions
    )
    
    if(NOT APPLE)
        add_compile_options(-fPIC)
    endif()
endif()

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# Create the main library
add_library(chronovyan_core_lib STATIC ${SOURCES})
target_link_libraries(chronovyan_core_lib PRIVATE Threads::Threads)

# Create an alias for backward compatibility
add_library(chronovyan_lib ALIAS chronovyan_core_lib)

# Enable testing
enable_testing()

# Add tests if requested
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    # Download and build GoogleTest using FetchContent
    include(FetchContent)

    # Set policy for FetchContent if available
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()

    # Download and build GoogleTest
    message(STATUS "Downloading and building GoogleTest...")

    # Use GitHub archive URL which is more reliable
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
        URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
    )

    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # Populate GTest
    FetchContent_MakeAvailable(googletest)

    # Create alias for consistency
    if(NOT TARGET GTest::GTest AND TARGET gtest)
        add_library(GTest::GTest ALIAS gtest)
        add_library(GTest::Main ALIAS gtest_main)
    endif()
    
    # Add test directory
    add_subdirectory(tests)
endif()
