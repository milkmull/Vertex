# Source files for sandbox/include/sandbox
file(GLOB SB_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/sandbox.hpp"
)

target_sources(Sandbox PRIVATE ${SB_HEADER_FILES})