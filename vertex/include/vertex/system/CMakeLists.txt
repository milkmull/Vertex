# Source files for vertex/include/vertex/system
file(GLOB VX_SYSTEM_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/config/dummy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/config/windows.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/log.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform_config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform_defines.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/profiler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/validate.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp"
)

target_sources(Vertex PRIVATE ${VX_SYSTEM_HEADER_FILES})