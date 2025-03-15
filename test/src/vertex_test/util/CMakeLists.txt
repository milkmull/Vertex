
#--------------------------------------------------------------------
# Util Tests
#--------------------------------------------------------------------

vx_add_test(test_util_bit         "util" "${CMAKE_CURRENT_SOURCE_DIR}/bit/bit.cpp")
vx_add_test(test_util_endian      "util" "${CMAKE_CURRENT_SOURCE_DIR}/bit/endian.cpp")
vx_add_test(test_util_base64      "util" "${CMAKE_CURRENT_SOURCE_DIR}/encode/base64.cpp")
vx_add_test(test_util_crypto      "util" "${CMAKE_CURRENT_SOURCE_DIR}/crypto/crypto.cpp")

file(GLOB VX_RANDOM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/random/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random/random.cpp"
)
vx_add_test(test_util_random      "util" "${VX_RANDOM_SRC_FILES}")

vx_add_test(test_util_string      "util" "${CMAKE_CURRENT_SOURCE_DIR}/string/string.cpp")
vx_add_test(test_util_regex       "util" "${CMAKE_CURRENT_SOURCE_DIR}/string/regex.cpp")
vx_add_test(test_util_time        "util" "${CMAKE_CURRENT_SOURCE_DIR}/time/time.cpp")
