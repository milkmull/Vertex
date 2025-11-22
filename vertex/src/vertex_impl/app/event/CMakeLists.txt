
#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

file(GLOB VX_APP_EVENT_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/event_watch.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/event_watch.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/temporary_memory_pool.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/platform_event.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/event_internal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/signal.cpp"
)

target_sources(Vertex PRIVATE ${VX_APP_EVENT_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

# Header files for dummy modules
file(GLOB VX_APP_EVENT_DUMMY_SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_event.hpp"
)

target_sources(Vertex PRIVATE ${VX_APP_EVENT_DUMMY_SOURCE_FILES})

if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_APP_EVENT_PLATFORM_SOURCE_FILES

        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_event.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_event.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_APP_EVENT_PLATFORM_SOURCE_FILES})
    
endif()