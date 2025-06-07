# This file configures tests for Chronovyan

# Set output directory for test executables
set(TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/tests)
file(MAKE_DIRECTORY ${TEST_OUTPUT_DIR})

# Function to add a test with proper configuration
function(chronovyan_add_test TEST_NAME TEST_SRC)
    add_executable(${TEST_NAME} ${TEST_SRC})
    
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
            ${GTEST_INCLUDE_DIRS}
    )
    
    target_compile_definitions(${TEST_NAME}
        PRIVATE
            GTEST_OUTPUT_XML=1
            GTEST_OUTPUT_DIR="${TEST_OUTPUT_DIR}"
    )
    
    target_link_libraries(${TEST_NAME}
        PRIVATE
            GTest::GTest
            GTest::Main
            chronovyan_core_lib
    )
    
    # Set output directory for the test executable
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIR}
    )
    
    # Add the test to CTest with XML output
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME} --gtest_output=xml:${TEST_OUTPUT_DIR}/${TEST_NAME}.xml
    )
    
    # Make sure the output directory exists
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_OUTPUT_DIR}
    )
endfunction()

# Add core minimal test
chronovyan_add_test(core_minimal_test core_minimal_test.cpp)

# Add character literal test
chronovyan_add_test(character_literal_test character_literal_test.cpp)
target_include_directories(character_literal_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/compiler/include
        ${CMAKE_SOURCE_DIR}/src/compiler/src
)

# Add the main test executable
chronovyan_add_test(chronovyan_test chronovyan_test.cpp)

# Set collection test
chronovyan_add_test(set_test collections/SetTest.cpp)
target_link_libraries(set_test
    PRIVATE
        chronovyan_stdlib
)

# Array collection test
chronovyan_add_test(array_test collections/ArrayTest.cpp)
target_link_libraries(array_test
    PRIVATE
        chronovyan_stdlib
)

# Map collection test
chronovyan_add_test(map_test collections/MapTest.cpp)
target_link_libraries(map_test
    PRIVATE
        chronovyan_stdlib
)

# AST Printer Test
chronovyan_add_test(ast_printer_test test_ast_printer.cpp)
target_link_libraries(ast_printer_test
    PRIVATE
        chronovyan_compiler_lib
        chronovyan_lib
        chronovyan_parser_lib
)

# Set custom command for ast_printer_test
set_tests_properties(ast_printer_test PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND $<TARGET_FILE:ast_printer_test> ${CMAKE_SOURCE_DIR}/test_ast.chron
)

# Parser tests for legacy and advanced constructs
chronovyan_add_test(parser_test_legacy_constructs parser_test_legacy_constructs.cpp)
target_link_libraries(parser_test_legacy_constructs
    PRIVATE
        chronovyan_lib
)

chronovyan_add_test(parser_test_advanced_constructs parser_test_advanced_constructs.cpp)
target_link_libraries(parser_test_advanced_constructs
    PRIVATE
        chronovyan_lib
)

# Resource management tests
chronovyan_add_test(resource_tracker_test resource_tracker_test.cpp)
target_link_libraries(resource_tracker_test
    PRIVATE
        chronovyan_lib
)

chronovyan_add_test(resource_visualizer_test resource_visualizer_test.cpp)
target_link_libraries(resource_visualizer_test
    PRIVATE
        chronovyan_lib
)

chronovyan_add_test(resource_processor_test resource_processor_test.cpp)
target_link_libraries(resource_processor_test
    PRIVATE
        chronovyan_lib
)

chronovyan_add_test(resource_optimizer_test resource_optimizer_test.cpp)
target_link_libraries(resource_optimizer_test
    PRIVATE
        chronovyan_lib
)

# Real-time dashboard test
chronovyan_add_test(real_time_dashboard_test real_time_dashboard_test.cpp)
target_link_libraries(real_time_dashboard_test
    PRIVATE
        chronovyan_lib
)

# Temporal debt system tests
chronovyan_add_test(temporal_debt_system_test temporal_debt_system_test.cpp)
target_link_libraries(temporal_debt_system_test 
    PRIVATE 
        chronovyan_lib
)

chronovyan_add_test(temporal_debt_test temporal_debt_test.cpp)
target_link_libraries(temporal_debt_test 
    PRIVATE 
        chronovyan_lib
)

chronovyan_add_test(core_resource_optimizer_test core_resource_optimizer_test.cpp)
target_link_libraries(core_resource_optimizer_test 
    PRIVATE 
        chronovyan_lib
)

# Resource export test
chronovyan_add_test(resource_export_test resource_export_test.cpp)
target_link_libraries(resource_export_test
    PRIVATE
        chronovyan_lib
)

# Map type test
chronovyan_add_test(map_type_test map_type_test.cpp)
target_link_libraries(map_type_test
    PRIVATE
        chronovyan_lib
)

# Vector type test
chronovyan_add_test(vector_type_test vector_type_test.cpp)
target_link_libraries(vector_type_test
    PRIVATE
        chronovyan_lib
)

# Chronovyan test runner
chronovyan_add_test(chronovyan_test_runner chronovyan_test_runner.cpp)
target_link_libraries(chronovyan_test_runner
    PRIVATE
        chronovyan_lib
)

# Test for script files in the tests directory
# Temporarily commented out until chronovyan_interpreter is available
# add_test(
#     NAME ChronovyanScriptTests 
#     COMMAND chronovyan_test_runner --test-dir ${CMAKE_SOURCE_DIR}/tests/scripts --interpreter $<TARGET_FILE:chronovyan_interpreter>
# )

# Test for example script files in the examples directory
# Temporarily commented out until chronovyan_interpreter is available
# add_test(
#     NAME ChronovyanExampleTests 
#     COMMAND chronovyan_test_runner --test-dir ${CMAKE_SOURCE_DIR}/examples --interpreter $<TARGET_FILE:chronovyan_interpreter>
# )

# Tests for the Temporal Sequence data structure
chronovyan_add_test(temporal_sequence_test temporal_sequence_test.cpp)
target_link_libraries(temporal_sequence_test PRIVATE 
    chronovyan_lib
)

# Tests for the user-definable TYPE system
chronovyan_add_test(type_system_test type_system_test.cpp)
target_link_libraries(type_system_test PRIVATE 
    chronovyan_lib
)

# Debt visualization test
chronovyan_add_test(debt_visualization_test debt_visualization_test.cpp)
target_link_libraries(debt_visualization_test PRIVATE 
    chronovyan_lib
)

# Enhanced REWIND_FLOW test
chronovyan_add_test(rewind_flow_test rewind_flow_manual_test.cpp)
target_link_libraries(rewind_flow_test PRIVATE 
    chronovyan_lib
)

# New visitor component tests - temporarily disabled until we fix the tests
# ExpressionVisitor tests
# add_executable(expression_visitor_test
#     ExpressionVisitorTests.cpp
# )
# target_link_libraries(expression_visitor_test PRIVATE
#     chronovyan_lib
#     gtest
#     gtest_main
# )
# add_test(
#     NAME expression_visitor_test
#     COMMAND expression_visitor_test
# )

# StatementVisitor tests
# add_executable(statement_visitor_test
#     StatementVisitorTests.cpp
# )
# target_link_libraries(statement_visitor_test PRIVATE
#     chronovyan_lib
#     gtest
#     gtest_main
# )
# add_test(
#     NAME statement_visitor_test
#     COMMAND statement_visitor_test
# )

# TemporalVisitor tests
# add_executable(temporal_visitor_test
#     TemporalVisitorTests.cpp
# )
# target_link_libraries(temporal_visitor_test PRIVATE
#     chronovyan_lib
#     gtest
#     gtest_main
# )
# add_test(
#     NAME temporal_visitor_test
#     COMMAND temporal_visitor_test
# )

# ResourceManager tests
# add_executable(resource_manager_test
#     ResourceManagerTests.cpp
# )
# target_link_libraries(resource_manager_test PRIVATE
#     chronovyan_lib
#     gtest
#     gtest_main
# )
# add_test(
#     NAME resource_manager_test
#     COMMAND resource_manager_test
# )

# Integration test for all interpreter components
# add_executable(interpreter_components_integration_test
#     InterpreterComponentsIntegrationTest.cpp
# )
# target_link_libraries(interpreter_components_integration_test PRIVATE
#     chronovyan_lib
#     gtest
#     gtest_main
# )
# add_test(
#     NAME interpreter_components_integration_test
#     COMMAND interpreter_components_integration_test
# )

# Advanced Control Flow tests
add_executable(advanced_control_flow_test
    AdvancedControlFlowTest.cpp
)
target_link_libraries(advanced_control_flow_test PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(
    NAME advanced_control_flow_test
    COMMAND advanced_control_flow_test
)

# Note: Other tests are disabled as they require additional dependencies
# that are not yet implemented 