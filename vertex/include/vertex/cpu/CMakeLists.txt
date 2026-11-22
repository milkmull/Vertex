
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/cpu
file(GLOB VX_CPU_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/endian.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory_order.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fence.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lock_free.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pause.hpp"
)

target_sources(Vertex PRIVATE ${VX_CPU_HEADER_FILES})
