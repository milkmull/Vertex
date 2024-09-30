#--------------------------------------------------------------------
# CMake Setup
#--------------------------------------------------------------------

cmake_minimum_required(VERSION 3.12)

# The CMP0091 policy allows setting the MSVC runtime library through the MSVC_RUNTIME_LIBRARY target property.
# This policy must be set to "NEW" in order to use the "MultiThreaded$<$<CONFIG:Debug>:Debug>" option, which specifies
# whether to use the debug or release version of the runtime library based on the build configuration.
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

# Version
include(cmake/Version.cmake)
message(STATUS "Vertex version: ${VX_PROJECT_VERSION}")

# Project name
project(Vertex VERSION ${VX_PROJECT_VERSION} LANGUAGES CXX)

# Check if we are compiling Vertex by itsself or in a separate project
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(VX_MAIN_PROJECT FALSE)
else()
    set(VX_MAIN_PROJECT TRUE)
endif()

# CMake Includes
include(CheckCXXCompilerFlag)
include(CMakePushCheckState)
include(GNUInstallDirs)

# Vertex Includes
include(cmake/Config.cmake)
include(cmake/Macros.cmake)
include(cmake/Functions.cmake)

#--------------------------------------------------------------------
# Shared Availability
#--------------------------------------------------------------------

set(VX_SHARED_AVAILABLE TRUE)

#--------------------------------------------------------------------
# Set Project Options
#--------------------------------------------------------------------

# Set the build configuration
if(VX_SHARED_AVAILABLE)
    vx_set_option(VX_BUILD_SHARED_LIBS      BOOL TRUE  "Build shared libraries.")
endif()

if(VX_CMAKE_PLATFORM_WINDOWS)
    vx_set_option(VX_USE_STATIC_STD_LIBS    BOOL FALSE "Statically link to the standard libraries.")
endif()

# Add options to select which modules to build
vx_set_option(VX_BUILD_APP_WINDOW           BOOL TRUE  "Enable building the windowing backend. Enabled by default if rendering is enabled.")
vx_set_option(VX_BUILD_APP_RENDERER         BOOL TRUE  "Enable building the rendering backend.")
vx_set_option(VX_BUILD_APP_AUDIO            BOOL TRUE  "Enable building the audio backend.")
vx_set_option(VX_BUILD_IMAGE                BOOL TRUE  "Enable image loading and saving capabilities.")
vx_set_option(VX_BUILD_NETWORK              BOOL TRUE  "Enable networking capabilities.")
    
vx_set_option(VX_BUILD_TESTS                BOOL FALSE "h")
vx_set_option(VX_BUILD_SANDBOX              BOOL TRUE  "h")

#--------------------------------------------------------------------
# Subdirectories
#--------------------------------------------------------------------

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("${CMAKE_SOURCE_DIR}/vertex")

if(VX_BUILD_TESTS)
    add_subdirectory("${CMAKE_SOURCE_DIR}/test")
endif()

if(VX_BUILD_SANDBOX)
    add_subdirectory("${CMAKE_SOURCE_DIR}/sandbox")
endif()