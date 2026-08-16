
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

#vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
#vx_add_test(test_std_aligned_storage        "std" "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.cpp")
#vx_add_test(test_std_packed_string_array    "std" "${CMAKE_CURRENT_SOURCE_DIR}/packed_string_array.cpp")
#vx_add_test(test_std_io                     "std" "${CMAKE_CURRENT_SOURCE_DIR}/io.cpp")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vector")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/string")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/single_linked_list")

#vx_add_test(test_std_array                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/array.cpp")
#vx_add_test(test_std_span                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/span.cpp")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/string_convert")
#vx_add_test(test_std_format                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/format.cpp")
#vx_add_test(test_std_scan                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/scan.cpp")
