cmake_minimum_required(VERSION 3.12)

project(Vertex VERSION 1.0.0 LANGUAGES CXX)

set(VX_MAIN_PROJECT OFF)
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(VX_MAIN_PROJECT ON)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration options
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
message(STATUS "Buid type: ${CMAKE_BUILD_TYPE}")

set(VX_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries")
set(VX_BUILD_EXAMPLES OFF CACHE BOOL "Build Examples")
set(VX_BUILD_TESTS OFF CACHE BOOL "Build Tests")
set(VX_BUILD_SANDBOX ON CACHE BOOL "Build Sandbox")

# Global defines
add_compile_definitions($<$<CONFIG:Debug>:VX_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:VX_RELEASE>)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(VX_PLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(VX_PLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_definitions(VX_PLATFORM_MACOS)
else()
    add_compile_definitions(VX_PLATFORM_UNKNOWN)
endif()
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Global compile flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}      -W3 -external:W0 -Od -Ob0     -D _UNICODE          ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}    -W4 -external:W0 -O2 -Ob2 -GF -D _UNICODE -D NDEBUG")
    
    if (VX_BUILD_SHARED_LIBS)
        # If shared libs are ON, use /MDd for debug and /MD for release
        set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -MDd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -MD")
    else()
        # If shared libs are OFF, use /MTd for debug and /MT for release
        set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -MT")
    endif()
    
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g -Wall -O0                      -D _UNICODE          ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}    -Wall -O2 -fno-strict-aliasing -D _UNICODE -D NDEBUG")
    
    # Check if VX_BUILD_SHARED_LIBS is ON
    if (VX_BUILD_SHARED_LIBS)
        # If shared libs are ON, use -fPIC
        set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -fPIC")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")
    endif()
    
endif()


# Output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

if (VX_BUILD_SHARED_LIBS)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
endif()

# Subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/vertex)
if (VX_BUILD_SANDBOX)
    add_subdirectory(${CMAKE_SOURCE_DIR}/sandbox)
endif()
