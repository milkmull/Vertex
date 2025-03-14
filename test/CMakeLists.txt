
#--------------------------------------------------------------------
# Create Tests
#--------------------------------------------------------------------

macro(vx_add_test TARGET PATH SOURCES)

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
    
    if(VX_BUILD_SHARED_LIBS)
    
        # Add a post-build command to copy the Vertex shared library to the test directory
        add_custom_command(
            TARGET ${TARGET}
            POST_BUILD                                      # Execute after the target has been built
            COMMAND ${CMAKE_COMMAND} -E copy_if_different   # Only copy if the file has changed or doesn't exist
                "$<TARGET_FILE:Vertex>"                     # Path to the Vertex shared library
                "$<TARGET_FILE_DIR:${TARGET}>"              # Destination directory (where the test binary is)
            COMMENT "Ensuring Vertex shared library is up-to-date for ${TARGET}."
            DEPENDS Vertex                                  # Ensure the Vertex library is built before the copy
        )
        
    endif()

endmacro()

#--------------------------------------------------------------------
# Util Tests
#--------------------------------------------------------------------

vx_add_test(test_bit "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/bit/bit.cpp")
vx_add_test(test_endian "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/bit/endian.cpp")

vx_add_test(test_base64 "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/encode/base64.cpp")
vx_add_test(test_crypto "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/crypto/crypto.cpp")

file(GLOB VX_RANDOM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/random/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/random/random.cpp"
)
vx_add_test(test_random "util" "${VX_RANDOM_SRC_FILES}")

vx_add_test(test_string "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/string/string.cpp")
vx_add_test(test_regex "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/string/regex.cpp")

vx_add_test(test_time "util" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/util/time/time.cpp")

#--------------------------------------------------------------------
# OS Tests
#--------------------------------------------------------------------

file(GLOB VX_OS_PATH_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/path.cpp"
)
vx_add_test(test_path "os" "${VX_OS_PATH_SRC_FILES}")

vx_add_test(test_file "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/file.cpp")
vx_add_test(test_filesystem "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/filesystem.cpp")
vx_add_test(test_os_time "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/time.cpp")

#--------------------------------------------------------------------

vx_add_test(child_process "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/child_process.cpp")
vx_add_test(test_process "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/process.cpp")
add_dependencies(test_process child_process)

#--------------------------------------------------------------------

vx_add_test(test_thread "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/thread.cpp")

#--------------------------------------------------------------------

add_library(shared_library SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/shared_library_lib.cpp")
set_target_properties(shared_library PROPERTIES FOLDER "Tests/os")
vx_set_stdlib(shared_library)
vx_add_common_compiler_flags(shared_library)
vx_hide_public_symbols(shared_library)

vx_add_test(test_shared_library "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/shared_library.cpp")
add_dependencies(test_shared_library shared_library)

#--------------------------------------------------------------------

vx_add_test(test_system_info "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/system_info.cpp")
vx_add_test(test_locale "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/locale.cpp")
vx_add_test(test_system_memory "os" "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_test/os/system_memory.cpp")