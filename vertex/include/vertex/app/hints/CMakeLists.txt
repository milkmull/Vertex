#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_HINTS_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/hint_defs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hints.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/property.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_HINTS_HEADER_FILES})
