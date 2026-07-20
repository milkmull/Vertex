
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
vx_add_test(test_std_aligned_storage        "std" "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.cpp")

vx_add_test(test_std_math_uint_n            "std" "${CMAKE_CURRENT_SOURCE_DIR}/uint_n.cpp")

vx_add_test(test_std_array                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/array.cpp")
vx_add_test(test_std_vector                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/vector.cpp")
vx_add_test(test_std_static_vector          "std" "${CMAKE_CURRENT_SOURCE_DIR}/static_vector.cpp")
vx_add_test(test_std_span                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/span.cpp")

file(GLOB VX_PROFILE_VX_VECTOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_vector.cpp"
)
vx_add_test(test_std_profile_vx_vector              "std" "${VX_PROFILE_VX_VECTOR_FILES}")


#file(GLOB VX_PROFILE_STD_VECTOR_FILES
#    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_vector.cpp"
#)
#vx_add_test(test_std_profile_std_vector             "std" "${VX_PROFILE_STD_VECTOR_FILES}")

vx_add_test(test_std_char_traits                    "std" "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.cpp")
vx_add_test(test_std_string                         "std" "${CMAKE_CURRENT_SOURCE_DIR}/string.cpp")
vx_add_test(test_std_string_view                    "std" "${CMAKE_CURRENT_SOURCE_DIR}/string_view.cpp")
vx_add_test(test_std_static_string                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/static_string.cpp")
vx_add_test(test_std_string_utils                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/string_utils.cpp")

file(GLOB VX_TEST_STRING_CONVERT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/integer_to_string_test_data.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/float_fixed_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/float_scientific_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/float_hex_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/float_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/float_from_string_test_cases.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_fixed_to_string_test_cases_1.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_fixed_to_string_test_cases_2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_fixed_to_string_test_cases_3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_fixed_to_string_test_cases_4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_scientific_to_string_test_cases_1.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_scientific_to_string_test_cases_2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_scientific_to_string_test_cases_3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_scientific_to_string_test_cases_4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_hex_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert/double_from_string_test_cases.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/float_to_string_test_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_from_string_test_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert.cpp"
)
vx_add_test(test_std_string_convert                 "std" "${VX_TEST_STRING_CONVERT_FILES}")
