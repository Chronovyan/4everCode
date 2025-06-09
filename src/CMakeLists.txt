# Add the temporal_synchronizer implementation to the build
add_library(temporal_synchronizer
    temporal_synchronizer_impl.cpp
)

target_include_directories(temporal_synchronizer
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(temporal_synchronizer
    PRIVATE
        Threads::Threads
)

# Install the library
install(TARGETS temporal_synchronizer
    EXPORT ChronovyanTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/chronovyan/
    DESTINATION include/chronovyan
    FILES_MATCHING PATTERN "*.hpp"
)
