# platform specific os files
if(VX_CMAKE_PLATFORM_WINDOWS)

    # Source files for windows
    file(GLOB VX_OS_PLATFORM_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_file.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_file.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_filesystem.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_filesystem.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_header.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_info.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_locale.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_memory.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_power.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_process.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_random.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_shared_library.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_shared_library.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_shell.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_time.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_tools.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/_platform/windows/windows_tools.cpp"
    )
    
    target_sources(Vertex PRIVATE ${VX_OS_PLATFORM_SOURCE_FILES})

endif()


# Source files for vertex/src/vertex_impl/os
file(GLOB VX_OS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/file.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/file.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shared_library.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.cpp"
)

target_sources(Vertex PRIVATE ${VX_OS_SOURCE_FILES})