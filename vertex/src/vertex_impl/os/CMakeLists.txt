# Source files for vertex/src/vertex_impl/os
file(GLOB VX_OS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/file.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/locale.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/random.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system_info.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.cpp"
)

target_sources(Vertex PRIVATE ${VX_OS_SOURCE_FILES})

# platform specific os files
if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_header.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_tools.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_tools.cpp"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_file.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_filesystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_locale.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_locale.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_memory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_memory.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_process.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_random.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_random.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_shared_library.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_system_info.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_system_info.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_time.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_time.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})
    
else()

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_locale.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_memory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_random.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_system_info.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/dummy/dummy_time.hpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})

endif()