cmake_minimum_required(VERSION 3.12)

# Project name
project(Vertex VERSION 0.1.0 LANGUAGES CXX)

set(VX_MAIN_PROJECT OFF)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(VX_MAIN_PROJECT ON)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------------------------------------
# Build Version
#--------------------------------------------------------------------

# Configuration options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
message(STATUS "Buid type: ${CMAKE_BUILD_TYPE}")

#--------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------

# Include the configuration file
include(cmake/Config.cmake)

#--------------------------------------------------------------------
# Options
#--------------------------------------------------------------------

set(VX_BUILD_SHARED_LIBS OFF)# CACHE BOOL "Build Shared Libraries")
#set(VX_BUILD_EXAMPLES    OFF CACHE BOOL "Build Examples")
#set(VX_BUILD_TESTS       OFF CACHE BOOL "Build Tests")
set(VX_BUILD_SANDBOX     ON  CACHE BOOL "Build Sandbox")
set(VX_BUILD_WINDOW      ON  CACHE BOOL "Build Window")

#--------------------------------------------------------------------
# Compile Definitions
#--------------------------------------------------------------------

# Global defines
add_compile_definitions("$<$<CONFIG:Debug>:VX_DEBUG>")
add_compile_definitions("$<$<CONFIG:Release>:VX_RELEASE>")

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
add_compile_definitions("_UNICODE")

if(VX_BUILD_SHARED_LIBS)
    add_compile_definitions("VX_BUILD_SHARED_LIBS")
endif()

# Global compile flags
if(VX_COMPILER_MSVC)

    # Warning level
    add_compile_options("$<$<CONFIG:Debug>:-W3>")
    add_compile_options("$<$<CONFIG:Release>:-W4>")
    
    # External warning level
    add_compile_options("-external:W0")
    
    # Optimization level
    add_compile_options("$<$<CONFIG:Debug>:-Od>")
    add_compile_options("$<$<CONFIG:Release>:-O2>")
    
    # String pooling
    add_compile_options("$<$<CONFIG:Debug>:-GF>")

    # NDEBUG
    add_compile_definitions("$<$<CONFIG:Release>:NDEBUG>")
    
    if(VX_BUILD_SHARED_LIBS)
        # If shared libs are ON, use /MDd for debug and /MD for release
        add_compile_options("$<$<CONFIG:Debug>:-MDd>")
        add_compile_options("$<$<CONFIG:Release>:-MD>")
    else()
        # If shared libs are OFF, use /MTd for debug and /MT for release
        add_compile_options("$<$<CONFIG:Debug>:-MTd>")
        add_compile_options("$<$<CONFIG:Release>:-MT>")
    endif()
    
elseif(VX_COMPILER_GCC OR VX_COMPILER_CLANG)

    # Warning level
    add_compile_options("-Wall")
    
    # Optimization level
    add_compile_options("$<$<CONFIG:Debug>:-O0>")
    add_compile_options("$<$<CONFIG:Release>:-O2>")
    
    # Strict aliasing
    add_compile_options("$<$<CONFIG:Release>:-fno-strict-aliasing>")

    # NDEBUG
    add_compile_definitions("$<$<CONFIG:Release>:NDEBUG>")

    if(VX_BUILD_SHARED_LIBS)
        # If shared libs are ON, use -fPIC
        add_compile_options("-fPIC")
    endif()
    
endif()

#--------------------------------------------------------------------
# Output Files
#--------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/Debug/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib/Release/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

#--------------------------------------------------------------------
# Subdirectories
#--------------------------------------------------------------------

add_subdirectory("${CMAKE_SOURCE_DIR}/vertex")

if (VX_BUILD_SANDBOX)
    add_subdirectory("${CMAKE_SOURCE_DIR}/sandbox")
endif()