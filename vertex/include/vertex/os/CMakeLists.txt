
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/os
file(GLOB VX_OS_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/atomic.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/locale.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mutex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/native_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_info.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_id.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.hpp"
)

target_sources(Vertex PRIVATE ${VX_OS_HEADER_FILES})
