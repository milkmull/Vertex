#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_INPUT_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/usb_ids.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/platform_mouse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mouse_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mouse.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/platform_keyboard.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyboard.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/touch_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/touch.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/pen_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pen.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_INPUT_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

# Header files for dummy modules
file(GLOB VX_APP_INPUT_DUMMY_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_mouse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_keyboard.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_touch.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_pen.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_INPUT_DUMMY_SOURCE_FILES})

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_INPUT_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_raw_input.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_raw_input.cpp"

        #"${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_game_input.hpp"
        #"${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_game_input.cpp"
            
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_mouse.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_mouse.cpp"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_keyboard.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_keyboard.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_INPUT_PLATFORM_SOURCE_FILES})
    
endif()