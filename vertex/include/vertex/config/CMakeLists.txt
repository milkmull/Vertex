# Source files for vertex/include/vertex/system
file(GLOB VX_SYSTEM_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/architecture.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/language.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/language_config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/os.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/util.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/version_number.hpp"
)

target_sources(Vertex PRIVATE ${VX_SYSTEM_HEADER_FILES})