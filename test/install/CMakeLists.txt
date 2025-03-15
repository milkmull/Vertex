cmake_minimum_required(VERSION 3.10)

#--------------------------------------------------------------------
# Test Header Interface
#--------------------------------------------------------------------

project(header_interface_test)

# Define a test executable
add_executable(header_interface_test header_main.cpp)

# Find the Vertex package
find_package(Vertex REQUIRED)

# Test linking against the headers interface
if(TARGET Vertex::Headers)

    message(STATUS "Vertex headers interface found.")
    target_link_libraries(header_interface_test PRIVATE Vertex::Headers)
    
else()

    message(FATAL_ERROR "Vertex headers interface not found.")
    
endif()

#--------------------------------------------------------------------
# Test Shared Library
#--------------------------------------------------------------------

# Test linking against the shared library
if(TARGET Vertex::Vertex_Shared)

    message(STATUS "Linking against Vertex shared library.")
    add_executable(shared_library_test shared_main.cpp)
    target_link_libraries(shared_library_test PRIVATE Vertex::Headers Vertex::Vertex_Shared)
    
    # Print shared library location
    get_target_property(SHARED_LIB_LOCATION Vertex::Vertex_Shared LOCATION)
    message(STATUS "Found shared library at: ${SHARED_LIB_LOCATION}")

    # Copy DLL to the executable directory (for Windows systems)
    if(WIN32 AND SHARED_LIB_LOCATION)
    
        add_custom_command(TARGET shared_library_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Vertex::Vertex_Shared>"
            "$<TARGET_FILE_DIR:shared_library_test>"
        )
        message(STATUS "Configured to copy DLL to executable directory.")
        
    endif()
    
else()

    message(WARNING "Vertex shared library not found. Skipping shared library test.")
    
endif()

#--------------------------------------------------------------------
# Test Static Library
#--------------------------------------------------------------------

# Test linking against the static library
if(TARGET Vertex::Vertex_Static)

    message(STATUS "Linking against Vertex static library.")
    add_executable(static_library_test static_main.cpp)
    target_link_libraries(static_library_test PRIVATE Vertex::Headers Vertex::Vertex_Static)

    # Print static library location
    get_target_property(STATIC_LIB_LOCATION Vertex::Vertex_Static LOCATION)
    message(STATUS "Found static library at: ${STATIC_LIB_LOCATION}")
    
else()

    message(WARNING "Vertex static library not found. Skipping static library test.")
    
endif()
