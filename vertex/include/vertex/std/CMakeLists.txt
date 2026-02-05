# Source files for vertex/include/vertex/std
file(GLOB VX_STD_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ranges.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stack_dynamic_array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unique_ptr.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/pointer_iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/dynamic_array_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/stack_dynamic_array_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/min_max.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/simd_algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/invoke.hpp"
)

target_sources(Vertex PRIVATE ${VX_STD_HEADER_FILES})
