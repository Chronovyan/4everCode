cmake_minimum_required(VERSION 3.12)
project(error_handling_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Chronovyan source directory
include_directories(${CMAKE_SOURCE_DIR}/../../include)

# Add the test executable
add_executable(error_handling_test error_handling_test.cpp)

# Link with Chronovyan library
target_link_libraries(error_handling_test chronovyan_lib) 