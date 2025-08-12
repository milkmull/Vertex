
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/vertex_impl/app
file(GLOB VX_APP_SOURCE_FILES
    
)

target_sources(Vertex PRIVATE ${VX_APP_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_header.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_PLATFORM_SOURCE_FILES})
    
endif()

#--------------------------------------------------------------------
# Modules
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/event")

if(VX_BUILD_APP_VIDEO)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/video")
endif()