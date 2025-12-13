
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
vx_add_test(test_std_dynamic_array          "std" "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_array.cpp")
vx_add_test(test_std_dynamic_array_profile  "std" "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_array_profile.cpp")
