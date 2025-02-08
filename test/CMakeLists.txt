
#--------------------------------------------------------------------
# Create Tests
#--------------------------------------------------------------------

function(vx_add_test TARGET PATH SOURCES)

    # Group source files for IDEs (e.g., Visual Studio)
    source_group("" FILES ${SOURCES})
    # Create the test executable target
    add_executable(${TARGET} ${SOURCES})
    # Set target folder for IDEs (e.g., Visual Studio)
    set_target_properties(${TARGET} PROPERTIES FOLDER "Tests/${PATH}")
    # Apply the appropriate C++ standard library settings
    vx_set_stdlib(${TARGET})
    # Link the test target with its dependencies and test framework
    target_link_libraries(${TARGET} PRIVATE Vertex)
    # Apply common compiler flags and hide public symbols
    vx_add_common_compiler_flags(${TARGET})
    vx_hide_public_symbols(${TARGET})
    # Add testing definition
    target_compile_definitions(${TARGET} PRIVATE -DVX_TESTING)
    
    # Add include directories for the target
    target_include_directories(${TARGET} PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
    
    # Post-build step to copy the DLL to the output directory
    if(VX_BUILD_SHARED_LIBS)

        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:Vertex>"         # This gets the full path to the Vertex DLL
            "$<TARGET_FILE_DIR:${TARGET}>"  # This gets the output directory of the Test executable
        )
        
    endif()

endfunction()

#--------------------------------------------------------------------
# Util Tests
#--------------------------------------------------------------------

#vx_add_test(test_bit "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/bit/bit.cpp")
#vx_add_test(test_endian "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/bit/endian.cpp")
#
#vx_add_test(test_base64 "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/encode/base64.cpp")
#vx_add_test(test_crypto "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/crypto/crypto.cpp")
#
#file(GLOB VX_RANDOM_SRC_FILES
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/random/random.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/random/random.cpp"
#)
#vx_add_test(test_random "util" "${VX_RANDOM_SRC_FILES}")
#
#vx_add_test(test_string "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/string/string.cpp")
#vx_add_test(test_regex "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/string/regex.cpp")
#
#vx_add_test(test_time "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/time/time.cpp")

#--------------------------------------------------------------------
# OS Tests
#--------------------------------------------------------------------

file(GLOB VX_OS_PATH_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/path.cpp"
)
vx_add_test(test_path "os" "${VX_OS_PATH_SRC_FILES}")

#vx_add_test(test_file "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/file.cpp")
vx_add_test(test_filesystem "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/filesystem.cpp")