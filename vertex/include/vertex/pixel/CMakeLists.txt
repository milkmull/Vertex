
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/pixel
file(GLOB VX_PIXEL_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/pixel_format.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_pixel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/surface.hpp"
)

target_sources(Vertex PRIVATE ${VX_PIXEL_HEADER_FILES})