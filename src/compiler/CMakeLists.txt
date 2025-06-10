cmake_minimum_required(VERSION 3.5)

# Add compiler include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

# Find Matplot++ (optional)
find_package(Matplot++ QUIET)
if(NOT Matplot++_FOUND)
    message(STATUS "Matplot++ not found, disabling graphical visualization")
    add_definitions(-DCHRONOVYAN_NO_GRAPHICAL_VISUALIZATION)
endif()

# Add the main compiler executable as a standalone without dependency on chronovyan_compiler_lib
add_executable(chronovyan_compiler src/main_fixed.cpp)
target_compile_features(chronovyan_compiler PUBLIC cxx_std_17)
target_compile_options(chronovyan_compiler PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

set_target_properties(chronovyan_compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add the run_chronovyan executable (commented out as the source file is missing)
# add_executable(run_chronovyan
#     ${CMAKE_SOURCE_DIR}/src/run_chronovyan.cpp
# )
# target_compile_features(run_chronovyan PUBLIC cxx_std_17)
# target_compile_options(run_chronovyan PRIVATE
#     $<$<CXX_COMPILER_ID:MSVC>:/W4>
#     $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
# )
# 
# # Add include directories for the run_chronovyan target
# target_include_directories(run_chronovyan PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/compiler/include
#     ${CMAKE_SOURCE_DIR}/src/interpreter/include
#     ${CMAKE_SOURCE_DIR}/include
# )
# 
# # Link the run_chronovyan executable with the compiler library and interpreter
# if(TARGET chronovyan_compiler_lib AND TARGET chronovyan_interpreter_lib)
#     target_link_libraries(run_chronovyan PRIVATE 
#         chronovyan_compiler_lib
#         chronovyan_interpreter_lib
#     )
# endif()
# 
# set_target_properties(run_chronovyan PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
# )
# 
# # Install run_chronovyan executable
# install(TARGETS run_chronovyan
#         RUNTIME DESTINATION bin
#         OPTIONAL
# )

# Add include directories for targets
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

# Install compiler executable
install(TARGETS chronovyan_compiler
        RUNTIME DESTINATION bin
        OPTIONAL
)

# Compiler library - include essential components
if(1)
    add_library(chronovyan_compiler_lib STATIC
        src/Lexer.cpp
        src/Parser.cpp
        src/SemanticAnalyzer.cpp
        src/CodeGenerator.cpp
        src/DiagnosticReporter.cpp
        src/SymbolTable.cpp
        src/TypeChecker.cpp
        src/Token.cpp
        src/AstNode.cpp
        src/Diagnostic.cpp
        src/CompilerDriver.cpp
        src/IRGenerator.cpp
        src/BytecodeGenerator.cpp
        src/IR.cpp
        src/OptimizationManager.cpp
        src/AstPrinter.cpp
    )

    target_compile_features(chronovyan_compiler_lib PUBLIC cxx_std_17)
    target_compile_options(chronovyan_compiler_lib PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
    )

    # Add compiler test programs
    add_executable(compiler_test src/compiler_test.cpp)
    target_link_libraries(compiler_test PRIVATE chronovyan_compiler_lib)
    target_compile_features(compiler_test PUBLIC cxx_std_17)
    set_target_properties(compiler_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # Add lexer test program
    add_subdirectory(test/lexer)

    # Install the compiler library
    install(TARGETS chronovyan_compiler_lib
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            OPTIONAL
    )

    # Install compiler test program
    install(TARGETS compiler_test
            RUNTIME DESTINATION bin
            OPTIONAL
    )
endif()

# Install headers
install(DIRECTORY include/
    DESTINATION include
)

# Create a directory for sample programs
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../samples/
    DESTINATION share/chronovyan/samples
    OPTIONAL
)

# Create a directory for bytecode headers
if(0)
    add_custom_command(
        TARGET chronovyan_compiler_lib
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/include/chronovyan/bytecode
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/include/compiler/bytecode/Bytecode.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/compiler/bytecode/BytecodeModule.h
            ${CMAKE_SOURCE_DIR}/include/chronovyan/bytecode/
        COMMENT "Copying bytecode headers to include directory"
    )
endif()

# Comment out the debugger and proving_grounds components for now
# add_subdirectory(debugger)
# add_subdirectory(proving_grounds)

# Create a symlink or copy bytecode headers to the expected location
# add_custom_command(
#     TARGET chronovyan_bytecode
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/include/chronovyan/bytecode
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/include/compiler/bytecode/Bytecode.h
#         ${CMAKE_CURRENT_SOURCE_DIR}/include/compiler/bytecode/BytecodeModule.h
#         ${CMAKE_SOURCE_DIR}/include/chronovyan/bytecode/
#     COMMENT "Copying bytecode headers to include directory"
# ) 