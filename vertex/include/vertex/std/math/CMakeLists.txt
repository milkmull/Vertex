# Source files for vertex/include/vertex/std/math
file(GLOB VX_STD_MATH_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/limits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/float_bits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/uint_n.hpp"
)

target_sources(Vertex PRIVATE ${VX_STD_MATH_HEADER_FILES})
