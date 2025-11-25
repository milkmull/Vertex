
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_INPUT_HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/clipboard.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mouse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keycode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/scancode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/touch.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pen.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_INPUT_HEADER_FILES})