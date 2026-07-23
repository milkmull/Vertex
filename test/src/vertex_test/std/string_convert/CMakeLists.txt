
file(GLOB VX_TEST_STRING_CONVERT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/integer_to_string_test_data.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/float_fixed_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_scientific_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_hex_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_from_string_test_cases.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/double_fixed_to_string_test_cases_1.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_fixed_to_string_test_cases_2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_fixed_to_string_test_cases_3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_fixed_to_string_test_cases_4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_scientific_to_string_test_cases_1.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_scientific_to_string_test_cases_2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_scientific_to_string_test_cases_3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_scientific_to_string_test_cases_4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_hex_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_to_string_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/double_from_string_test_cases.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert.cpp"
)
vx_add_test(test_std_string_convert                 "std/string_convert" "${VX_TEST_STRING_CONVERT_FILES}")

file(GLOB VX_PROFILE_VX_TO_STRING
    "${CMAKE_CURRENT_SOURCE_DIR}/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_to_string.cpp"
)
vx_add_test(test_std_profile_vx_to_string "std/string_convert" "${VX_PROFILE_VX_TO_STRING}")

file(GLOB VX_PROFILE_STD_TO_CHARS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_to_chars.cpp"
)
vx_add_test(test_std_profile_std_to_chars "std/string_convert" "${VX_PROFILE_STD_TO_CHARS}")

file(GLOB VX_PROFILE_VX_FROM_STRING
    "${CMAKE_CURRENT_SOURCE_DIR}/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_from_string.cpp"
)
vx_add_test(test_std_profile_vx_from_string "std/string_convert" "${VX_PROFILE_VX_FROM_STRING}")

file(GLOB VX_PROFILE_STD_FROM_CHARS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_from_chars.cpp"
)
vx_add_test(test_std_profile_std_from_chars "std/string_convert" "${VX_PROFILE_STD_FROM_CHARS}")
