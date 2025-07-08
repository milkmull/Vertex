
#--------------------------------------------------------------------
# Project Options Configuration
#--------------------------------------------------------------------

vx_set_option(VX_MATH_ENABLE_SIMD BOOL FALSE "Enable simd optimization for math library if available")

if(VX_MATH_ENABLE_SIMD)
    target_compile_definitions(Vertex PUBLIC "VXM_ENABLE_SIMD")
endif()

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
    "${CMAKE_CURRENT_SOURCE_DIR}/core/util.hpp"
    
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
    "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/swizzle.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/transform2d.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/transform3d.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/projection.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/core/util/hash.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/util/to_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/util/iter.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/core/easing.hpp"
    
    #--------------------------------------
    # Color files
    #--------------------------------------
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/util.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/types/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/types/color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/types/color8.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/functions/color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/functions/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/functions/comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/functions/interpolation.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/space/srgb.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/space/hsl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/space/hsv.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/util/hash.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/util/to_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color/util/iter.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/color/blend.hpp"
    
    #--------------------------------------
    # Procedural files
    #--------------------------------------
    
    "${CMAKE_CURRENT_SOURCE_DIR}/procedural/wrap.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/procedural/sdf2d.hpp"
)

target_sources(Vertex PRIVATE ${VX_MATH_HEADER_FILES})

#--------------------------------------------------------------------
# simd
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simd")