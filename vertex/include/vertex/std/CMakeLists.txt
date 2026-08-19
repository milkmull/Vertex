# Source files for vertex/include/vertex/std
file(GLOB VX_STD_HEADER_FILES

    # Core
    "${CMAKE_CURRENT_SOURCE_DIR}/array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/aligned_storage.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/defer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/io.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/span.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unique_ptr.hpp"

    # Containers
    "${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vector_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/static_vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/singly_linked_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/doubly_linked_list.hpp"

    # Strings
    "${CMAKE_CURRENT_SOURCE_DIR}/char_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/static_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_cast.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_convert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/string_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/packed_string_array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utf.hpp"

    # Math / numeric
    "${CMAKE_CURRENT_SOURCE_DIR}/math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/float_bits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/float_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/checked_arithmetic.hpp"

    # Formatting
    "${CMAKE_CURRENT_SOURCE_DIR}/format.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/scan.hpp"

    # Crypto
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto/fnv1a.hpp"

    # Internal
    "${CMAKE_CURRENT_SOURCE_DIR}/_tools/compressed_pair.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_tools/dynamic_array_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_tools/pointer_iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_tools/invoke.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_simd/min_max.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_simd/simd_algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_string_convert/string_convert_common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_string_convert/from_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_string_convert/to_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_format/format_scan_common.hpp"
)

target_sources(Vertex PRIVATE ${VX_STD_HEADER_FILES})
