
#--------------------------------------------------------------------
# Math Tests
#--------------------------------------------------------------------

vx_add_test(test_math_scalar        "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/scalar.cpp")
vx_add_test(test_math_vec2          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2.cpp")
vx_add_test(test_math_vec3          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3.cpp")
vx_add_test(test_math_vec4          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4.cpp")

vx_add_test(test_math_comparison    "math" "${CMAKE_CURRENT_SOURCE_DIR}/functions/comparison.cpp")
vx_add_test(test_math_common        "math" "${CMAKE_CURRENT_SOURCE_DIR}/functions/common.cpp")
vx_add_test(test_math_exponential   "math" "${CMAKE_CURRENT_SOURCE_DIR}/functions/exponential.cpp")
vx_add_test(test_math_trigonometric "math" "${CMAKE_CURRENT_SOURCE_DIR}/functions/trigonometric.cpp")
vx_add_test(test_math_geometric     "math" "${CMAKE_CURRENT_SOURCE_DIR}/functions/geometric.cpp")
