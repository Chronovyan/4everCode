# Chronovyan Interpreter Component

# Interpreter library
add_library(chronovyan_interpreter_lib
    src/BytecodeInterpreter.cpp
)

# Add include directories for the interpreter library
target_include_directories(chronovyan_interpreter_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src/compiler/include
)

# Link with compiler library and main library
target_link_libraries(chronovyan_interpreter_lib
    PUBLIC
        chronovyan_compiler_lib
        chronovyan_stdlib
)

# Interpreter executable
add_executable(chronovyan_run
    src/main.cpp
)

# Add include directories for the interpreter executable
target_include_directories(chronovyan_run
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src/compiler/include
        ${PROJECT_SOURCE_DIR}/include
)

# Link with interpreter library
target_link_libraries(chronovyan_run
    PRIVATE
        chronovyan_interpreter_lib
)

# Set output name
set_target_properties(chronovyan_run PROPERTIES OUTPUT_NAME chronovyan_run)

# Install targets
install(TARGETS 
    chronovyan_interpreter_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(TARGETS 
    chronovyan_run
    RUNTIME DESTINATION bin
) 