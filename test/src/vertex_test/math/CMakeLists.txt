
#--------------------------------------------------------------------
# Math Tests
#--------------------------------------------------------------------

vx_add_test(test_math_scalar                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/scalar.cpp")
            
vx_add_test(test_math_vec2                  "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec2.cpp")
vx_add_test(test_math_vec3                  "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec3.cpp")
vx_add_test(test_math_vec4                  "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec4.cpp")
            
vx_add_test(test_math_mat2x2                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x2.cpp")
vx_add_test(test_math_mat2x3                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x3.cpp")
vx_add_test(test_math_mat2x4                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x4.cpp")
vx_add_test(test_math_mat3x2                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x2.cpp")
vx_add_test(test_math_mat3x3                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x3.cpp")
vx_add_test(test_math_mat3x4                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x4.cpp")
vx_add_test(test_math_mat4x2                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x2.cpp")
vx_add_test(test_math_mat4x3                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x3.cpp")
vx_add_test(test_math_mat4x4                "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat4x4.cpp")
            
vx_add_test(test_math_quat                  "math/core/types" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/quat.cpp")
            
vx_add_test(test_math_comparison            "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/comparison.cpp")
vx_add_test(test_math_common                "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/common.cpp")
vx_add_test(test_math_exponential           "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/exponential.cpp")
vx_add_test(test_math_trigonometric         "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/trigonometric.cpp")
vx_add_test(test_math_geometric             "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/geometric.cpp")
vx_add_test(test_math_interpolation         "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/interpolation.cpp")
vx_add_test(test_math_matrix                "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/matrix.cpp")
vx_add_test(test_math_quaternion            "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/quaternion.cpp")
vx_add_test(test_math_rotation_cast         "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/rotation_cast.cpp")
vx_add_test(test_math_euler_angles          "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/euler_angles.cpp")
vx_add_test(test_math_swizzle               "math/core/functions" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/swizzle.cpp")

vx_add_test(test_math_transform2d           "math/core/transform" "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/transform2d.cpp")
vx_add_test(test_math_transform3d           "math/core/transform" "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/transform3d.cpp")
vx_add_test(test_math_projection            "math/core/transform" "${CMAKE_CURRENT_SOURCE_DIR}/core/transform/projection.cpp")
