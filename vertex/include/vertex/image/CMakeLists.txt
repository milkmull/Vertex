
#--------------------------------------------------------------------
# Public options
#--------------------------------------------------------------------

if(VX_IMAGE_ENABLED)
    target_compile_definitions(Vertex PUBLIC VX_IMAGE_ENABLED)
endif()

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/image
file(GLOB VX_IMAGE_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/defs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/image.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/load.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/write.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/format/load_context.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/format/bmp.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/format/jpg.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/format/png.hpp"
)

target_sources(Vertex PRIVATE ${VX_IMAGE_HEADER_FILES})