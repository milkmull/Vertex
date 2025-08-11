
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Header files for vertex/vertex/app
file(GLOB VX_APP_EVENT_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/event.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_EVENT_HEADER_FILES})