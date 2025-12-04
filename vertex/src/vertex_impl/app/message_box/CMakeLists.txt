#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_MESSAGE_BOX_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/platform_message_box.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/message_box_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/message_box.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_MESSAGE_BOX_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_MESSAGE_BOX_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_message_box.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_message_box.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_MESSAGE_BOX_PLATFORM_SOURCE_FILES})
    
endif()