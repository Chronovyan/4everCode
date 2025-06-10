# Add the timeline library
add_library(timeline
    timeline.cpp
)

target_include_directories(timeline
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(timeline
    PRIVATE
        Threads::Threads
)

# Install the library
install(TARGETS timeline
    EXPORT ChronovyanTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Add tests if enabled
if(BUILD_TESTS)
    add_executable(timeline_test
        ${CMAKE_CURRENT_SOURCE_DIR}/../../test/timeline_test.cpp
    )
    
    target_link_libraries(timeline_test
        PRIVATE
            timeline
            gtest
            gtest_main
            Threads::Threads
    )
    
    # Enable CTest
    enable_testing()
    
    # Add the test to CTest
    add_test(NAME timeline_test
        COMMAND timeline_test
    )
    
    # Create a custom target for building tests
    add_custom_target(run_timeline_tests
        COMMAND ctest --output-on-failure
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS timeline_test
    )
    
    # Create a custom target to run tests
    add_custom_target(run_tests
        DEPENDS run_timeline_tests
        COMMENT "Running timeline tests..."
    )
endif()
