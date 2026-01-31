# Source files for vertex/src/vertex_impl/std/simd_algorithms
file(GLOB VX_STD_SIMD_ALGORITHMS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_bitset_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_count.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_find.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_find_meow_of.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_find_seq.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_mismatch.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_remove.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_replace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_reverse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_rotate.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_sort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd_algorithms.cpp"
)

target_sources(Vertex PRIVATE ${VX_STD_SIMD_ALGORITHMS_SOURCE_FILES})
