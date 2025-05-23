
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math.hpp"
    
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
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x2b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x2i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x2u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x2f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x3b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x3i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x3u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x3f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x4b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x4i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x4u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x4f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x2b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x2i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x2u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x2f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x3b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x3i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x3u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x3f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x4b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x4i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x4u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x4f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x2b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x2i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x2u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x2f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x3b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x3i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x3u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x3f.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x4b.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x4i.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x4u.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x4f.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/quat.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/functions/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/exponential.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/geometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/interpolation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/trigonometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/matrix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/quaternion_exponential.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})