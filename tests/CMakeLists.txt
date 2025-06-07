# This file configures tests for Chronovyan

# Add core minimal test
add_executable(core_minimal_test core_minimal_test.cpp)
target_link_libraries(core_minimal_test
    PRIVATE
        GTest::GTest
        GTest::Main
        chronovyan_core_lib
)

# Add the test to CTest
add_test(NAME core_minimal_test COMMAND core_minimal_test)

# Add character literal test
add_executable(character_literal_test character_literal_test.cpp)
target_include_directories(character_literal_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/compiler/include
        ${CMAKE_SOURCE_DIR}/src/compiler/src
)
target_link_libraries(character_literal_test
    PRIVATE
        GTest::GTest
        GTest::Main
        chronovyan_core_lib
)

# Add the test to CTest
add_test(NAME character_literal_test COMMAND character_literal_test)

# Add the main test executable
add_executable(chronovyan_test chronovyan_test.cpp)
target_link_libraries(chronovyan_test
    PRIVATE
        GTest::GTest
        GTest::Main
        chronovyan_core_lib
)

# Add the test to CTest
add_test(NAME chronovyan_test COMMAND chronovyan_test)

# Set collection test
add_executable(set_test collections/SetTest.cpp)
target_link_libraries(set_test
    PRIVATE
    chronovyan_stdlib
    gtest
    gtest_main
)
add_test(NAME set_test COMMAND set_test)

# Array collection test
add_executable(array_test collections/ArrayTest.cpp)
target_link_libraries(array_test
    PRIVATE
    chronovyan_stdlib
    gtest
    gtest_main
)
add_test(NAME array_test COMMAND array_test)

# Map collection test
add_executable(map_test collections/MapTest.cpp)
target_link_libraries(map_test
    PRIVATE
    chronovyan_stdlib
    gtest
    gtest_main
)
add_test(NAME map_test COMMAND map_test)

# AST Printer Test
add_executable(ast_printer_test test_ast_printer.cpp)
target_link_libraries(ast_printer_test
    PRIVATE
    chronovyan_compiler_lib
    chronovyan_lib
    chronovyan_parser_lib
    gtest
    gtest_main
)
add_test(NAME ast_printer_test 
    COMMAND ast_printer_test ${CMAKE_SOURCE_DIR}/test_ast.chron
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Parser tests for legacy and advanced constructs
add_executable(parser_test_legacy_constructs parser_test_legacy_constructs.cpp)
target_link_libraries(parser_test_legacy_constructs
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME parser_test_legacy_constructs COMMAND parser_test_legacy_constructs)

add_executable(parser_test_advanced_constructs parser_test_advanced_constructs.cpp)
target_link_libraries(parser_test_advanced_constructs
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME parser_test_advanced_constructs COMMAND parser_test_advanced_constructs)

# Resource management tests
add_executable(resource_tracker_test resource_tracker_test.cpp)
target_link_libraries(resource_tracker_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME resource_tracker_test COMMAND resource_tracker_test)

add_executable(resource_visualizer_test resource_visualizer_test.cpp)
target_link_libraries(resource_visualizer_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME resource_visualizer_test COMMAND resource_visualizer_test)

add_executable(resource_processor_test resource_processor_test.cpp)
target_link_libraries(resource_processor_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME resource_processor_test COMMAND resource_processor_test)

add_executable(resource_optimizer_test resource_optimizer_test.cpp)
target_link_libraries(resource_optimizer_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME resource_optimizer_test COMMAND resource_optimizer_test)

# Real-time dashboard test
add_executable(real_time_dashboard_test real_time_dashboard_test.cpp)
target_link_libraries(real_time_dashboard_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME real_time_dashboard_test COMMAND real_time_dashboard_test)

# Temporal debt system tests
add_executable(temporal_debt_system_test temporal_debt_system_test.cpp)
target_link_libraries(temporal_debt_system_test PRIVATE chronovyan_lib gtest_main)
add_test(NAME temporal_debt_system_test COMMAND temporal_debt_system_test)

add_executable(temporal_debt_test temporal_debt_test.cpp)
target_link_libraries(temporal_debt_test 
    PRIVATE 
    chronovyan_lib 
    gtest 
    gtest_main
)
add_test(NAME temporal_debt_test COMMAND temporal_debt_test)

add_executable(core_resource_optimizer_test core_resource_optimizer_test.cpp)
target_link_libraries(core_resource_optimizer_test PRIVATE chronovyan_lib gtest_main)
add_test(NAME core_resource_optimizer_test COMMAND core_resource_optimizer_test)

# Resource export test
add_executable(resource_export_test resource_export_test.cpp)
target_link_libraries(resource_export_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME resource_export_test COMMAND resource_export_test)

# Map type test
add_executable(map_type_test map_type_test.cpp)
target_link_libraries(map_type_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME map_type_test COMMAND map_type_test)

# Vector type test
add_executable(vector_type_test vector_type_test.cpp)
target_link_libraries(vector_type_test
    PRIVATE
    chronovyan_lib
    gtest
    gtest_main
)
add_test(NAME vector_type_test COMMAND vector_type_test)

# Chronovyan test runner
add_executable(chronovyan_test_runner chronovyan_test_runner.cpp)
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
add_executable(temporal_sequence_test
    temporal_sequence_test.cpp
)
target_link_libraries(temporal_sequence_test PRIVATE 
    chronovyan_lib
    GTest::gtest_main
)
add_test(
    NAME temporal_sequence_test
    COMMAND temporal_sequence_test
)

# Tests for the user-definable TYPE system
add_executable(type_system_test
    type_system_test.cpp
)
target_link_libraries(type_system_test PRIVATE 
    chronovyan_lib
    GTest::gtest_main
)
add_test(
    NAME type_system_test
    COMMAND type_system_test
)

# Debt visualization test
add_executable(debt_visualization_test
    debt_visualization_test.cpp
)
target_link_libraries(debt_visualization_test PRIVATE 
    chronovyan_lib
    GTest::gtest_main
)
add_test(
    NAME debt_visualization_test
    COMMAND debt_visualization_test
)

# Enhanced REWIND_FLOW test
add_executable(rewind_flow_test
    rewind_flow_manual_test.cpp
)
target_link_libraries(rewind_flow_test PRIVATE 
    chronovyan_lib
)
add_test(
    NAME rewind_flow_test
    COMMAND rewind_flow_test
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