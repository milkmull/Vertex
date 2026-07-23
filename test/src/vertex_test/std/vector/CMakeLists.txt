
vx_add_test(test_std_vector                 "std/vector" "${CMAKE_CURRENT_SOURCE_DIR}/vector.cpp")
vx_add_test(test_std_static_vector          "std/vector" "${CMAKE_CURRENT_SOURCE_DIR}/static_vector.cpp")

file(GLOB VX_PROFILE_VX_VECTOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profile_vx_vector.cpp"
)
vx_add_test(test_std_profile_vx_vector              "std/vector" "${VX_PROFILE_VX_VECTOR_FILES}")

#file(GLOB VX_PROFILE_STD_VECTOR_FILES
#    "${CMAKE_CURRENT_SOURCE_DIR}/vector_profile_tools.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/profile_std_vector.cpp"
#)
#vx_add_test(test_std_profile_std_vector             "std/vector" "${VX_PROFILE_STD_VECTOR_FILES}")
