# Standard Library CMakeLists.txt

# Core library components
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core/Core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/Value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/Error.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/Clock.cpp
)

# Collections
set(COLLECTIONS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/collections/Array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/collections/Set.cpp
)

# I/O components
set(IO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/io/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io/Console.cpp
)

# Existing standard library components
set(STDLIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/paradox_management.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pattern_management.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource_management.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/exploration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chronovyan_stdlib.cpp
)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/include
)

# Add library as static to avoid DLL export/import issues
add_library(chronovyan_stdlib STATIC
    ${CORE_SOURCES}
    ${COLLECTIONS_SOURCES}
    ${IO_SOURCES}
    ${STDLIB_SOURCES}
)

# Set include directories for the target
target_include_directories(chronovyan_stdlib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Remove the DLL export definition when building the library
# target_compile_definitions(chronovyan_stdlib 
#     PRIVATE 
#         CHRONOVYAN_STDLIB_BUILDING_DLL
# )

# Set the output directory for the static library
set_target_properties(chronovyan_stdlib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install
install(TARGETS chronovyan_stdlib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 