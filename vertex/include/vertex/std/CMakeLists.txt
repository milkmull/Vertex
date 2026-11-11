# Source files for vertex/include/vertex/std
file(GLOB_RECURSE VX_STD_HEADER_FILES CONFIGURE_DEPENDS

    # Base utilities
    "${CMAKE_CURRENT_SOURCE_DIR}/util.hpp"

    # Error
    "${CMAKE_CURRENT_SOURCE_DIR}/error/error_defs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/error/error_code.hpp"

    # Memory
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/allocator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/deleter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/_priv/memory_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/_priv/memory_core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/_priv/compressed_pair.hpp"

    # Iterators
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator/_priv/pointer_iterator.hpp"

    # Strings
    "${CMAKE_CURRENT_SOURCE_DIR}/string/char_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string/char_utils.hpp"

    # UTF
    "${CMAKE_CURRENT_SOURCE_DIR}/utf/utf.hpp"

    # SIMD
    "${CMAKE_CURRENT_SOURCE_DIR}/_simd/min_max.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_simd/simd_algorithms.hpp"
)

target_sources(Vertex PRIVATE ${VX_STD_HEADER_FILES})
