
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/type_traits.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/traits.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4f.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/fn/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/exponential.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/geometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/interpolation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/trigonometric.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/fn/vec.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})