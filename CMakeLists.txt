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

# Enable c language in case we need to check for c headers or anything
enable_language(C)

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

#--------------------------------------------------
# Build type / linkage
#--------------------------------------------------

# Option to build shared libraries if available
if(VX_SHARED_AVAILABLE)
    vx_set_option(VX_BUILD_SHARED_LIBS BOOL FALSE "Build shared libraries.")
endif()

#--------------------------------------------------
# Tooling and runtime behavior
#--------------------------------------------------

# Enable AddressSanitizer (ASan)
#vx_set_option(VX_ENABLE_ASAN        BOOL FALSE "Enable AddressSanitizer for Debug builds")

#--------------------------------------------------
# Platform / Implementation targets
#--------------------------------------------------

# Module-specific build options
vx_set_option(VX_DUMMY_PLATFORM     BOOL FALSE "Build the dummy os implimentation")

#--------------------------------------------------
# Library Feature Modules
#--------------------------------------------------

vx_set_option(VX_MATH_SIMD_ENABLED   BOOL FALSE "Enable simd optimization for math library if available")
vx_set_option(VX_IMAGE_ENABLED       BOOL TRUE  "Enable image loading and writing capabilities.")
#vx_set_option(VX_NETWORK_ENABLED      BOOL TRUE  "Enable networking capabilities.")
vx_set_option(VX_APP_ENABLED         BOOL FALSE "Enable building the application features.")

#--------------------------------------------------
# Application Features
#--------------------------------------------------

# App options (maybe only show these after configuring with VX_ENABLE_APP?)
if(VX_APP_ENABLED)

    vx_set_option(VX_GUI_APP                    BOOL FALSE "Build as a GUI application (no console window on supported platforms).")
    vx_set_option(VX_APP_VIDEO_ENABLED          BOOL TRUE  "Enable building the video backend.")
    #vx_set_option(VX_APP_RENDERER_ENABLED       BOOL TRUE  "Enable building the rendering backend.")
    #vx_set_option(VX_APP_AUDIO_ENABLED          BOOL TRUE  "Enable building the audio backend.")
    
endif()

#--------------------------------------------------
# Testing and Sandbox
#--------------------------------------------------

vx_set_option(VX_BUILD_TESTS        BOOL FALSE "Enable building of test executables.")
vx_set_option(VX_INSTALL_TESTS      BOOL FALSE "Enable installation of test executables.")
vx_set_option(VX_BUILD_SANDBOX      BOOL TRUE  "Enable building of the sandbox application.")

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
# Uninstall Target
#--------------------------------------------------------------------

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake"
)

#--------------------------------------------------------------------
# Configuration Summary
#--------------------------------------------------------------------

message(STATUS "-----------------------------------------")
message(STATUS "Vertex Configuration Summary:")
message(STATUS "-----------------------------------------")

# Build / Runtime
print_option(VX_BUILD_SHARED_LIBS    "Shared Libraries")
print_option(VX_DUMMY_PLATFORM       "Dummy Platform")
print_option(VX_MATH_SIMD_ENABLED    "SIMD Math")
print_option(VX_GUI_APP              "GUI Application")
# print_option(VX_ENABLE_ASAN        "AddressSanitizer") # uncomment if needed

# Library Modules
print_option(VX_IMAGE_ENABLED        "Image Module")
# print_option(VX_NETWORK_ENABLED    "Network Module") 

# App Features
print_option(VX_APP_ENABLED          "Application Features")
if(VX_APP_ENABLED)
    print_option(VX_APP_VIDEO_ENABLED     "  Video Backend")
    print_option(VX_APP_RENDERER_ENABLED  "  Renderer Backend")
    print_option(VX_APP_AUDIO_ENABLED     "  Audio Backend")
endif()

# Testing / Sandbox
print_option(VX_BUILD_TESTS          "Build Tests")
print_option(VX_INSTALL_TESTS        "Install Tests")
print_option(VX_BUILD_SANDBOX        "Build Sandbox")

message(STATUS "-----------------------------------------")
