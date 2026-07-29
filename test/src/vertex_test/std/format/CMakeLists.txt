
file(GLOB VX_TEST_FORMAT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/format_test_base.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/char_test_cases.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_test_cases.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/format.cpp"
)
vx_add_test(test_std_format "std/format" "${VX_TEST_FORMAT_FILES}")
