#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/vertex_impl/app/video
file(GLOB VX_APP_VIDEO_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/video.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/video.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/video.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_VIDEO_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

# Header files for dummy modules
file(GLOB VX_APP_VIDEO_DUMMY_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_video.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_VIDEO_DUMMY_SOURCE_FILES})

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_VIDEO_PLATFORM_SOURCE_FILES
            
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_video.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_video.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_VIDEO_PLATFORM_SOURCE_FILES})
    
endif()