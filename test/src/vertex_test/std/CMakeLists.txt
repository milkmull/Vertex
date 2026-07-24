
#--------------------------------------------------------------------
# std Tests
#--------------------------------------------------------------------

vx_add_test(test_std_memory                 "std" "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp")
vx_add_test(test_std_aligned_storage        "std" "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.cpp")
vx_add_test(test_std_io                     "std" "${CMAKE_CURRENT_SOURCE_DIR}/io.cpp")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vector")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/string")

vx_add_test(test_std_array                  "std" "${CMAKE_CURRENT_SOURCE_DIR}/array.cpp")
vx_add_test(test_std_span                   "std" "${CMAKE_CURRENT_SOURCE_DIR}/span.cpp")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/string_convert")
