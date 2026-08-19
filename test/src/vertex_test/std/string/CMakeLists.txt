
vx_add_test(test_std_char_traits                    "std/string" "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.cpp")
vx_add_test(test_std_string                         "std/string" "${CMAKE_CURRENT_SOURCE_DIR}/string.cpp")
vx_add_test(test_std_string_view                    "std/string" "${CMAKE_CURRENT_SOURCE_DIR}/string_view.cpp")
vx_add_test(test_std_static_string                  "std/string" "${CMAKE_CURRENT_SOURCE_DIR}/static_string.cpp")
vx_add_test(test_std_string_utils                   "std/string" "${CMAKE_CURRENT_SOURCE_DIR}/string_utils.cpp")

file(GLOB VX_PROFILE_VX_STRING_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/string_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_string.cpp"
)
vx_add_test(test_std_profile_vx_string              "std/string" "${VX_PROFILE_VX_STRING_FILES}")

file(GLOB VX_PROFILE_STD_STRING_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/string_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_string.cpp"
)
vx_add_test(test_std_profile_std_string             "std/string" "${VX_PROFILE_STD_STRING_FILES}")
