
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
vx_add_test(test_std_aligned_storage        "std" "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.cpp")
vx_add_test(test_std_array                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/array.cpp")
vx_add_test(test_std_dynamic_array          "std" "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_array.cpp")

file(GLOB VX_PROFILE_DYNAMIC_ARRAY_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_dynamic_array.cpp"
)
vx_add_test(test_std_profile_dynamic_array  "std" "${VX_PROFILE_DYNAMIC_ARRAY_FILES}")


file(GLOB VX_PROFILE_STD_VECTOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_vector.cpp"
)
vx_add_test(test_std_profile_std_vector             "std" "${VX_PROFILE_STD_VECTOR_FILES}")

vx_add_test(test_std_char_traits                    "std" "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.cpp")
