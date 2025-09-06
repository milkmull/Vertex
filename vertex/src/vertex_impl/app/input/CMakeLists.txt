#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_INPUT_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/mouse_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mouse.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/touch_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/touch.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/pen_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pen.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_INPUT_SOURCE_FILES})