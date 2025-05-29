
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
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat2x4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat3x4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types/mat4x4.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/types/quat.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/functions/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/exponential.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/geometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/interpolation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/trigonometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/matrix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/quaternion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions/quaternion_exponential.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})