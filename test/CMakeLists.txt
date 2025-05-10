#--------------------------------------------------------------------
# Vertex Tests CMake File
#--------------------------------------------------------------------

message(STATUS "Configuring Vertex Tests...")

#--------------------------------------------------------------------
# Helper Function: Test Installation
#--------------------------------------------------------------------

function(vx_configure_test_install TARGET)

    if(VX_INSTALL_TESTS)
    
        install(TARGETS ${TARGET}
            ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/test"   # For static libraries
            LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/test"   # For shared libraries
            RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/test"   # For executables
        )
        
    endif()
    
endfunction()

#--------------------------------------------------------------------
# Macro: Add Test
#--------------------------------------------------------------------

add_library(vertex_test_interface INTERFACE)
# Include the test directory for this target
target_include_directories(vertex_test_interface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

function(vx_add_test TARGET PATH SOURCES)

    # Group source files for IDEs
    source_group("" FILES ${SOURCES})

    # Create the test executable
    add_executable(${TARGET} ${SOURCES})
    set_target_properties(${TARGET} PROPERTIES FOLDER "Tests/${PATH}")

    # Apply standard library settings and common flags
    vx_add_common_compiler_flags(${TARGET})
    vx_hide_public_symbols(${TARGET})

    # Link against Vertex
    target_link_libraries(${TARGET} PRIVATE Vertex)
    # Link against the test interface to inherit include directories
    target_link_libraries(${TARGET} PRIVATE vertex_test_interface)

    # Add testing definition
    target_compile_definitions(${TARGET} PRIVATE -DVX_TESTING)

    # Handle shared library copying for tests
    if(VX_BUILD_SHARED_LIBS)
    
        add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:Vertex>"
                "$<TARGET_FILE_DIR:${TARGET}>"
            COMMENT "Ensuring Vertex shared library is up-to-date for ${TARGET}."
            DEPENDS Vertex
        )
        
    endif()

    # Configure installation if enabled
    vx_configure_test_install(${TARGET})
    
endfunction()

#--------------------------------------------------------------------
# Install the Vertex shared library if necessary
#--------------------------------------------------------------------

if(VX_INSTALL_TESTS AND VX_BUILD_SHARED_LIBS)
    install(FILES "$<TARGET_FILE:Vertex>" DESTINATION "${CMAKE_INSTALL_PREFIX}/test")
endif()

#--------------------------------------------------------------------
# Util Tests
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util")

#--------------------------------------------------------------------
# OS Tests
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os")

#--------------------------------------------------------------------
# Math Tests
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/math")

#--------------------------------------------------------------------
# Summary Message
#--------------------------------------------------------------------

message(STATUS "Vertex Tests Configuration Complete.")
