
#--------------------------------------------------------------------
# Dummy files
#--------------------------------------------------------------------

macro(add_dummy_files)

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_locale.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_random.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_system_info.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_system_memory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_thread.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/dummy/dummy_time.hpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})

endmacro()

#--------------------------------------------------------------------
# General files
#--------------------------------------------------------------------

# Source files for vertex/src/vertex_impl/os
file(GLOB VX_OS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/filesystem.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/locale.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/random.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/shared_library.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/system_info.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/system_memory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/__platform/time.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/file.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/locale.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_info.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.cpp"
)

target_sources(Vertex PRIVATE ${VX_OS_SOURCE_FILES})

#--------------------------------------------------------------------
# Platform specific files
#--------------------------------------------------------------------

if(VX_BUILD_DUMMY_OS)

    add_dummy_files()

elseif(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_header.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_tools.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_tools.cpp"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_filesystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_locale.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_locale.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_process.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_random.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_random.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_system_info.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_system_info.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_system_memory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_system_memory.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_thread.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_time.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/windows/windows_time.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})
    
elseif(VX_CMAKE_UNIX_SYS)

    # Source files for linux
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_header.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_tools.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_tools.cpp"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_filesystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_locale.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_locale.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_process.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_random.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_random.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_system_info.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_system_info.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_system_memory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_system_memory.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_thread.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_time.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/__platform/unix/unix_time.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})
    
else()

    add_dummy_files()

endif()