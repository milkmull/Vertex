# Source files for vertex/include/vertex/std
file(GLOB VX_STD_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ranges.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/array.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_array.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/pointer_iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_priv/dynamic_array_base.hpp"
)

target_sources(Vertex PRIVATE ${VX_STD_HEADER_FILES})
