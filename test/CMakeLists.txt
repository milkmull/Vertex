
#--------------------------------------------------------------------
# Create Tests
#--------------------------------------------------------------------

function(vx_add_test TARGET SOURCES)

    # Group source files for IDEs (e.g., Visual Studio)
    source_group("" FILES ${SOURCES})
    # Create the test executable target
    add_executable(${TARGET} ${SOURCES})
    # Set target folder for IDEs (e.g., Visual Studio)
    set_target_properties(${TARGET} PROPERTIES FOLDER "Tests")
    # Apply the appropriate C++ standard library settings
    vx_set_stdlib(${TARGET})
    # Link the test target with its dependencies and test framework
    target_link_libraries(${TARGET} PRIVATE ${DEPENDS} Vertex)
    # Apply common compiler flags and hide public symbols
    vx_add_common_compiler_flags(${TARGET})
    vx_hide_public_symbols(${TARGET})
    
    # Post-build step to copy the DLL to the output directory
    if(VX_BUILD_SHARED_LIBS)

        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:Vertex>"         # This gets the full path to the Vertex DLL
            "$<TARGET_FILE_DIR:${TARGET}>"  # This gets the output directory of the Sandbox executable
        )
        
    endif()

endfunction()

# String Tests

vx_add_test(vx_test_string "${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib/string/string.cpp")
vx_add_test(vx_test_crypto "${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib/string/crypto.cpp")
vx_add_test(vx_test_base64 "${CMAKE_CURRENT_SOURCE_DIR}/src/stdlib/string/base64.cpp")