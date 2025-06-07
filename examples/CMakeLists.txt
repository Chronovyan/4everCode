cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directory for examples
set(EXAMPLES_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/examples)
file(MAKE_DIRECTORY ${EXAMPLES_OUTPUT_DIR})

# Add examples if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resource_demo.cpp")
    add_executable(resource_demo resource_demo.cpp)
    target_link_libraries(resource_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(resource_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exploration_demo.cpp")
    add_executable(exploration_demo exploration_demo.cpp)
    target_link_libraries(exploration_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(exploration_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/paradox_demo.cpp")
    add_executable(paradox_demo paradox_demo.cpp)
    target_link_libraries(paradox_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(paradox_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pattern_demo.cpp")
    add_executable(pattern_demo pattern_demo.cpp)
    target_link_libraries(pattern_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(pattern_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_demo.cpp")
    add_executable(stdlib_demo stdlib_demo.cpp)
    target_link_libraries(stdlib_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(stdlib_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/theme_visualization_demo.cpp")
    add_executable(theme_visualization_demo theme_visualization_demo.cpp)
    target_link_libraries(theme_visualization_demo PRIVATE 
        chronovyan_lib 
        chronovyan_stdlib
        chronovyan_parser_lib
    )
    set_target_properties(theme_visualization_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR}
    )
    
    # Install examples
    install(TARGETS 
        resource_demo
        exploration_demo
        paradox_demo
        pattern_demo
        stdlib_demo
        theme_visualization_demo
        RUNTIME DESTINATION bin/examples
    )
endif()