#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_HINTS_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/hints_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hints.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_HINTS_SOURCE_FILES})