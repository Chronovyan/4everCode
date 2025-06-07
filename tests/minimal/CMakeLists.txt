cmake_minimum_required(VERSION 3.15)
project(ValueTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test package
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Enable testing
enable_testing()

# Add the test executable
add_executable(ValueTest
    ValueTest.cpp
)

# Link with gtest and the project library
target_link_libraries(ValueTest
    PRIVATE
    GTest::GTest
    GTest::Main
    Threads::Threads
    chronovyan_stdlib
)

# Include directories
target_include_directories(ValueTest
    PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include  # For chronovyan headers
    ${CMAKE_SOURCE_DIR}/..             # For other test includes
)

# Add the test
add_test(NAME ValueTest COMMAND ValueTest)
