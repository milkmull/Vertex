# Source files for vertex/include/vertex/system
file(GLOB VX_SYSTEM_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/log.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profiler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/validate.hpp"
)

target_sources(Vertex PRIVATE ${VX_SYSTEM_HEADER_FILES})