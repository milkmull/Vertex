
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/type_traits.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/common.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})