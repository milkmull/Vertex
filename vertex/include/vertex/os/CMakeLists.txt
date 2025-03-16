
#--------------------------------------------------------------------
# Dummy files
#--------------------------------------------------------------------

macro(add_dummy_files)

    file(GLOB VX_OS_PLATFORM_HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_file_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_shared_library_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_thread_impl_data.hpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_HEADER_FILES})

endmacro()

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/include/vertex/os
file(GLOB VX_OS_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/file_impl_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/shared_library_impl_data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/thread_impl_data.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/atomic.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/locale.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mutex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/native_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_info.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.hpp"
)

target_sources(Vertex PRIVATE ${VX_OS_HEADER_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

if(VX_BUILD_DUMMY_OS)

    add_dummy_files()

elseif(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_types.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_handle.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_file_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_shared_library_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_thread_impl_data.hpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_HEADER_FILES})
    
#elseif(VX_CMAKE_UNIX_SYS)
    
    # Source files for linux
    file(GLOB VX_OS_PLATFORM_HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_handle.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_file_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_shared_library_impl_data.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_thread_impl_data.hpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_HEADER_FILES})
    
else()

    add_dummy_files()

endif()