# Define executable
add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox.cpp")

# Files
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/include/sandbox")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
vx_create_source_groups(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(Sandbox PRIVATE ${SB_FILES})

# Add include directories
target_include_directories(Sandbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add link libraries
target_link_libraries(Sandbox PRIVATE Vertex)

# On Windows, set the correct subsystem automatically
if(WIN32 AND VX_GUI_APP)
    # GUI subsystem (WinMain entry point, no console)
    set_target_properties(Sandbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()

# Post-build step to copy the DLL to the output directory
if(VX_BUILD_SHARED_LIBS)

    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Vertex>"       # This gets the full path to the Vertex DLL
        "$<TARGET_FILE_DIR:Sandbox>"  # This gets the output directory of the Sandbox executable
    )
    
endif()