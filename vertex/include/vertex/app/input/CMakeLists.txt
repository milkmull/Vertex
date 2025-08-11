
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_INPUT_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/mouse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_INPUT_HEADER_FILES})