
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/pixel
file(GLOB VX_PIXEL_HEADER_FILES
    
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_pixel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_transform.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/surface.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pixel_format.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sampler.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/filter_box.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/filter_nearest.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/filter_bilinear.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/filter_bicubic.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/blit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/draw.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/mipmaps.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mask.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/surface_transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/surface_manip.hpp"
)

target_sources(Vertex PRIVATE ${VX_PIXEL_HEADER_FILES})