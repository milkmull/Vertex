
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
vx_add_test(test_std_aligned_storage        "std" "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.cpp")
vx_add_test(test_std_array                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/array.cpp")
vx_add_test(test_std_vector                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/vector.cpp")
vx_add_test(test_std_static_vector          "std" "${CMAKE_CURRENT_SOURCE_DIR}/static_vector.cpp")
vx_add_test(test_std_span                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/span.cpp")

file(GLOB VX_PROFILE_VX_VECTOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_vector.cpp"
)
vx_add_test(test_std_profile_vx_vector              "std" "${VX_PROFILE_VX_VECTOR_FILES}")


file(GLOB VX_PROFILE_STD_VECTOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_vector.cpp"
)
vx_add_test(test_std_profile_std_vector             "std" "${VX_PROFILE_STD_VECTOR_FILES}")

vx_add_test(test_std_char_traits                    "std" "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.cpp")
vx_add_test(test_std_string                         "std" "${CMAKE_CURRENT_SOURCE_DIR}/string.cpp")
vx_add_test(test_std_string_view                    "std" "${CMAKE_CURRENT_SOURCE_DIR}/string_view.cpp")
vx_add_test(test_std_static_string                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/static_string.cpp")
vx_add_test(test_std_string_utils                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/string_utils.cpp")
vx_add_test(test_std_string_convert                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/string_convert.cpp")
