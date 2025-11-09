
#--------------------------------------------------------------------
# Public options
#--------------------------------------------------------------------

# Whether to build as a Windows GUI application (no console)
# Public so headers and other projects can check it
if(VX_GUI_APP)
    target_compile_definitions(Vertex PUBLIC VX_GUI_APP)
endif()

if(VX_APP_VIDEO_ENABLED)
    target_compile_definitions(Vertex PUBLIC VX_APP_VIDEO_ENABLED)
endif()

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Header files for vertex/vertex/app
file(GLOB VX_APP_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/id.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/main_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_HEADER_FILES})

#--------------------------------------------------------------------
# Modules
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hints")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/event")

if(VX_APP_VIDEO_ENABLED)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/input")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/video")
endif()