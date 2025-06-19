
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/math
file(GLOB VX_MATH_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math.hpp"
    
    #--------------------------------------
    # Core files
    #--------------------------------------
    
    "${CMAKE_CURRENT_SOURCE_DIR}/core/constants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/types/quat.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/exponential.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/geometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/interpolation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/trigonometric.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/matrix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/quaternion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/quaternion_exponential.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/rotation_cast.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/euler_angles.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})