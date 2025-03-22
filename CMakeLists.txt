#--------------------------------------------------------------------
# Vertex - Top Level CMake File
#--------------------------------------------------------------------

cmake_minimum_required(VERSION 3.12)

#--------------------------------------------------------------------
# CMake Policy Setup
#--------------------------------------------------------------------

# This ensures that the MSVC runtime library can be configured through the MSVC_RUNTIME_LIBRARY target property.
# This is especially useful for projects where different runtime libraries need to be selected (e.g., static vs. dynamic).
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

#--------------------------------------------------------------------
# Project Configuration
#--------------------------------------------------------------------

# Load the version information from Version.cmake
include(cmake/Version.cmake)
message(STATUS "Vertex Version: ${VX_PROJECT_VERSION}")

# Define the project
project(Vertex VERSION ${VX_PROJECT_VERSION} LANGUAGES CXX)

# Check if this is the main project or part of another
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(VX_MAIN_PROJECT TRUE)
    message(STATUS "Building Vertex as the main project.")
else()
    set(VX_MAIN_PROJECT FALSE)
    message(STATUS "Building Vertex as a subproject.")
endif()

#--------------------------------------------------------------------
# CMake Modules & Utilities
#--------------------------------------------------------------------

# Include standard CMake modules
include(CheckCXXCompilerFlag)
include(CMakePushCheckState)
include(GNUInstallDirs)
include(CheckFunctionExists)
include(CheckSymbolExists)

# Include Vertex-specific modules
include(cmake/Config.cmake)
include(cmake/Macros.cmake)
include(cmake/Functions.cmake)
include(cmake/Checks.cmake)

#--------------------------------------------------------------------
# Shared Library Availability
#--------------------------------------------------------------------

# Determines whether shared libraries can be built
set(VX_SHARED_AVAILABLE TRUE)

#--------------------------------------------------------------------
# Project Options Configuration
#--------------------------------------------------------------------

message(STATUS "Configuring Vertex build options...")

# Option to build shared libraries if available
if(VX_SHARED_AVAILABLE)
    vx_set_option(VX_BUILD_SHARED_LIBS BOOL TRUE "Build shared libraries.")
endif()

# Module-specific build options
vx_set_option(VX_BUILD_DUMMY_OS     BOOL FALSE "Build the dummy os implimentation")
vx_set_option(VX_BUILD_APP_WINDOW   BOOL TRUE  "Enable building the windowing backend (Enabled by default if rendering is enabled).")
vx_set_option(VX_BUILD_APP_RENDERER BOOL TRUE  "Enable building the rendering backend.")
vx_set_option(VX_BUILD_APP_AUDIO    BOOL TRUE  "Enable building the audio backend.")
vx_set_option(VX_BUILD_IMAGE        BOOL TRUE  "Enable image loading and saving capabilities.")
vx_set_option(VX_BUILD_NETWORK      BOOL TRUE  "Enable networking capabilities.")

# Test and Sandbox options
vx_set_option(VX_BUILD_TESTS        BOOL FALSE "Enable building of test executables.")
vx_set_option(VX_BUILD_SANDBOX      BOOL TRUE  "Enable building of the sandbox application.")

# New option for installing tests
vx_set_option(VX_INSTALL_TESTS      BOOL FALSE "Enable installation of test executables.")

# Automatically enable tests if installation of tests is requested
if(VX_INSTALL_TESTS)
    message(STATUS "VX_INSTALL_TESTS is enabled, forcing VX_BUILD_TESTS to be enabled.")
    set(VX_BUILD_TESTS TRUE)
endif()

#--------------------------------------------------------------------
# Directory Organization & Subdirectories
#--------------------------------------------------------------------

# Enable folder grouping for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#--------------------------------------------------------------------
# Adding Subdirectories
#--------------------------------------------------------------------

message(STATUS "Adding Vertex subdirectories...")

# Add the main Vertex source directory
add_subdirectory("${CMAKE_SOURCE_DIR}/vertex")

# Add the test directory if tests are enabled
if(VX_BUILD_TESTS)
    message(STATUS "Building Tests: Enabled")
    add_subdirectory("${CMAKE_SOURCE_DIR}/test")
else()
    message(STATUS "Building Tests: Disabled")
endif()

# Add the sandbox directory if enabled
if(VX_BUILD_SANDBOX)
    message(STATUS "Building Sandbox: Enabled")
    add_subdirectory("${CMAKE_SOURCE_DIR}/sandbox")
else()
    message(STATUS "Building Sandbox: Disabled")
endif()

#--------------------------------------------------------------------
# Configuration Summary
#--------------------------------------------------------------------

message(STATUS "Vertex Configuration Summary:")
message(STATUS "  Build Dummy OS          : ${VX_BUILD_DUMMY_OS}")
message(STATUS "  Build Shared Libraries  : ${VX_BUILD_SHARED_LIBS}")
message(STATUS "  Use Static Std Libraries: ${VX_USE_STATIC_STD_LIBS}")
message(STATUS "  Build Window Backend    : ${VX_BUILD_APP_WINDOW}")
message(STATUS "  Build Renderer Backend  : ${VX_BUILD_APP_RENDERER}")
message(STATUS "  Build Audio Backend     : ${VX_BUILD_APP_AUDIO}")
message(STATUS "  Build Image Module      : ${VX_BUILD_IMAGE}")
message(STATUS "  Build Network Module    : ${VX_BUILD_NETWORK}")
message(STATUS "  Build Tests             : ${VX_BUILD_TESTS}")
message(STATUS "  Install Tests           : ${VX_INSTALL_TESTS}")
message(STATUS "  Build Sandbox           : ${VX_BUILD_SANDBOX}")
