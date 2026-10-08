# Source files for vertex/include/vertex/util
file(GLOB VX_UTIL_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/io/iostream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/io/quoted.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/io.hpp"
)

target_sources(Vertex PRIVATE ${VX_UTIL_HEADER_FILES})
