# Source files for vertex/include/vertex/os
file(GLOB VX_OS_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/info.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/locale.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mutex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/power.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shell.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.hpp"
)

target_sources(Vertex PRIVATE ${VX_OS_HEADER_FILES})