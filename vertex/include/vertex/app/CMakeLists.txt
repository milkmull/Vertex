
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Header files for vertex/vertex/app
file(GLOB VX_APP_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/device_id.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_HEADER_FILES})

#--------------------------------------------------------------------
# Modules
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/event")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/input")

if(VX_BUILD_APP_VIDEO)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/video")
endif()