
#--------------------------------------------------------------------
# Public options
#--------------------------------------------------------------------

# Whether to build as a Windows GUI application (no console)
# Public so headers and other projects can check it
if(VX_GUI_APP)
    target_compile_definitions(Vertex PUBLIC VX_GUI_APP)
endif()

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Header files for vertex/vertex/app
file(GLOB VX_APP_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/device_id.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/owner_ptr.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/main_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_HEADER_FILES})

#--------------------------------------------------------------------
# Modules
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hints")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/event")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/input")

if(VX_BUILD_APP_VIDEO)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/video")
endif()