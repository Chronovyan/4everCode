cmake_minimum_required(VERSION 3.10)

# Proving Grounds component
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(PROVING_GROUNDS_SOURCES
    src/TemporalTestFramework.cpp
    src/ParadoxDetector.cpp
    src/TemporalBenchmark.cpp
)

# Header files
set(PROVING_GROUNDS_HEADERS
    include/compiler/proving_grounds/TemporalTestFramework.h
    include/compiler/proving_grounds/ParadoxDetector.h
    include/compiler/proving_grounds/TemporalBenchmark.h
)

# Create the proving_grounds library
add_library(proving_grounds STATIC ${PROVING_GROUNDS_SOURCES} ${PROVING_GROUNDS_HEADERS})

# Set include directories
target_include_directories(proving_grounds PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/compiler/include
)

# Link libraries
target_link_libraries(proving_grounds PUBLIC 
    compiler 
    bytecode
    debugger
)

# Set output directories
set_target_properties(proving_grounds PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_executable(proving_grounds_test test/TemporalTestFrameworkTest.cpp)
    target_link_libraries(proving_grounds_test proving_grounds gtest gtest_main)
    add_test(NAME proving_grounds_test COMMAND proving_grounds_test)
    set_target_properties(proving_grounds_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif() 