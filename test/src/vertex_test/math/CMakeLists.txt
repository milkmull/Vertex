
#--------------------------------------------------------------------
# Math Tests
#--------------------------------------------------------------------

vx_add_test(test_math_scalar        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/scalar.cpp")

vx_add_test(test_math_vec2          "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec2.cpp")
vx_add_test(test_math_vec3          "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec3.cpp")
vx_add_test(test_math_vec4          "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/vec4.cpp")

vx_add_test(test_math_mat2x2        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x2.cpp")
vx_add_test(test_math_mat2x3        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x3.cpp")
vx_add_test(test_math_mat2x4        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat2x4.cpp")
vx_add_test(test_math_mat3x2        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x2.cpp")
vx_add_test(test_math_mat3x3        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/types/mat3x3.cpp")

vx_add_test(test_math_comparison    "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/comparison.cpp")
vx_add_test(test_math_common        "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/common.cpp")
vx_add_test(test_math_exponential   "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/exponential.cpp")
vx_add_test(test_math_trigonometric "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/trigonometric.cpp")
vx_add_test(test_math_geometric     "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/geometric.cpp")
vx_add_test(test_math_interpolation "math" "${CMAKE_CURRENT_SOURCE_DIR}/core/functions/interpolation.cpp")
