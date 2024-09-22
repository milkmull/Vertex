# Define executable
add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox.cpp")

# Statically link to the c runtime libraries
vx_set_stdlib(Sandbox)

# Files
file(GLOB_RECURSE SB_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
vx_create_source_group(SB_FILES "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(Sandbox PRIVATE ${SB_FILES})

# Add include directories
target_include_directories(Sandbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add link libraries
target_link_libraries(Sandbox PRIVATE Vertex)

# Post-build step to copy the DLL to the output directory
if(VX_BUILD_SHARED_LIBS)

    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Vertex>"       # This gets the full path to the Vertex DLL
        "$<TARGET_FILE_DIR:Sandbox>"  # This gets the output directory of the Sandbox executable
    )
    
endif()