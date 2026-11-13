
vx_add_test(test_std_hash_set "std/hash_set" "${CMAKE_CURRENT_SOURCE_DIR}/hash_set.cpp")

file(GLOB VX_PROFILE_VX_SET_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../hash_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_set.cpp"
)
vx_add_test(test_std_profile_vx_set              "std/hash_set" "${VX_PROFILE_VX_SET_FILES}")

file(GLOB VX_PROFILE_STD_SET_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../hash_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_set.cpp"
)
vx_add_test(test_std_profile_std_set              "std/hash_set" "${VX_PROFILE_STD_SET_FILES}")
