cmake_minimum_required(VERSION 3.10)

# Debugger component
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(DEBUGGER_SOURCES
    src/DejaVuDebugger.cpp
    src/DebuggerState.cpp
    src/StateHistory.cpp
    src/BreakpointManager.cpp
    src/StateVisualizer.cpp
)

# Header files
set(DEBUGGER_HEADERS
    include/compiler/debugger/DejaVuDebugger.h
)

# Create the debugger library
add_library(debugger STATIC ${DEBUGGER_SOURCES} ${DEBUGGER_HEADERS})

# Set include directories
target_include_directories(debugger PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/compiler/include
)

# Link libraries
target_link_libraries(debugger PUBLIC compiler bytecode)

# Set output directories
set_target_properties(debugger PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_executable(debugger_test test/DejaVuDebuggerTest.cpp)
    target_link_libraries(debugger_test debugger gtest gtest_main)
    add_test(NAME debugger_test COMMAND debugger_test)
    set_target_properties(debugger_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif() 