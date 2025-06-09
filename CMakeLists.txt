cmake_minimum_required(VERSION 3.15)
project(Forever VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
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
    add_compile_options($<$<CONFIG:Debug>:/Od /Zi>)
    add_compile_options($<$<CONFIG:Release>:/O2>)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Werror
        -fexceptions
    )
    if(NOT WIN32 AND NOT APPLE)
        add_compile_options(-fPIC)
    endif()
endif()

# Windows-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00 -DNOMINMAX)
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Create a minimal library with just the core functionality
add_library(Forever_lib
    src/Forever.cpp
)

target_include_directories(Forever_lib
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Forever_lib
    PUBLIC
        Threads::Threads
)

set_target_properties(Forever_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/Forever/Forever.hpp"
)

# Enable testing
enable_testing()

# Add tests if requested
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Add test subdirectory
    add_subdirectory(test)
    
    # Add a custom target to run all tests
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests..."
        DEPENDS Forever_test
    )
endif()
