
#--------------------------------------------------------------------
# Math Tests
#--------------------------------------------------------------------

vx_add_test(test_math_scalar        "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/scalar.cpp")
vx_add_test(test_math_vec2          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec2.cpp")
vx_add_test(test_math_vec3          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec3.cpp")
vx_add_test(test_math_vec4          "math" "${CMAKE_CURRENT_SOURCE_DIR}/types/vec4.cpp")

vx_add_test(test_math_comparison    "math" "${CMAKE_CURRENT_SOURCE_DIR}/comparison.cpp")
vx_add_test(test_math_common        "math" "${CMAKE_CURRENT_SOURCE_DIR}/common.cpp")
