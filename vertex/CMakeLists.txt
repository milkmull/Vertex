#--------------------------------------------------------------------
# Library
#--------------------------------------------------------------------

# Define header interface target
add_library(Vertex_Headers INTERFACE)
add_library(Vertex::Headers ALIAS Vertex_Headers)
set_property(TARGET Vertex_Headers PROPERTY EXPORT_NAME "Headers")

# Define library target for shared or static libraries
if(VX_BUILD_SHARED_LIBS)
    message(STATUS "Building shared libraries")
    add_library(Vertex SHARED)
    add_library(Vertex::Vertex_Shared ALIAS Vertex)
    set_property(TARGET Vertex PROPERTY EXPORT_NAME "Vertex_Shared")
else()
    message(STATUS "Building static libraries")
    add_library(Vertex STATIC)
    add_library(Vertex::Vertex_Static ALIAS Vertex)
    set_property(TARGET Vertex PROPERTY EXPORT_NAME "Vertex_Static")
endif()

#--------------------------------------------------------------------
# Configure library properties
#--------------------------------------------------------------------

set_target_properties(Vertex PROPERTIES LINKER_LANGUAGE CXX)
vx_add_common_compiler_flags(Vertex)
vx_check_available_functions(Vertex)

# Output directories for binary, library, and PDB files
set_target_properties(Vertex PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY     "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY     "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY     "${PROJECT_BINARY_DIR}/bin"
    
    PDB_OUTPUT_DIRECTORY         "${PROJECT_BINARY_DIR}/lib"
    COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
)

# Set postfix
if(VX_BUILD_SHARED_LIBS)
    set_target_properties(Vertex PROPERTIES
        DEBUG_POSTFIX           "_d"
    )
else()
    set_target_properties(Vertex PROPERTIES
        DEBUG_POSTFIX           "_static_d"
        RELEASE_POSTFIX         "_static"
        MINSIZEREL_POSTFIX      "_static"
        RELWITHDEBINFO_POSTFIX  "_static"
    )
endif()

if(VX_BUILD_SHARED_LIBS)

    # Hide public symbols by default
    vx_hide_public_symbols(Vertex)

    set_target_properties(Vertex PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
        DEFINE_SYMBOL "VX_DLL_EXPORT"
    )
    
else()

    # Define VX_STATIC for static libraries
    target_compile_definitions(Vertex PUBLIC "VX_STATIC")
   
endif()

# Debug macro
target_compile_definitions(Vertex PRIVATE $<$<CONFIG:Debug>:VX_CMAKE_DEBUG>)

# Build dummy library
if(VX_DUMMY_PLATFORM)
    target_compile_definitions(Vertex PRIVATE VX_DUMMY_PLATFORM)
endif()

# Propagate the Windows subsystem requirement to executables linking against
# this library. This allows for WinMain entry point to be used for any
# executable that link against Vertex.
#if(VX_CMAKE_PLATFORM_WINDOWS)
#    target_link_options(Vertex INTERFACE "/SUBSYSTEM:WINDOWS")
#endif()

# On some platforms (e.g. Raspberry Pi 3 armhf), GCC requires linking libatomic
# to use <atomic> features that aren't supported by native CPU instructions
# (64-bit atomic operations on 32-bit architecture)
#if(VX_CMAKE_COMPILER_GCC)
#
#    include(CheckCXXSourceCompiles)
#    check_cxx_source_compiles(
#    
#        "#include <atomic>
#        int main()
#        {
#            std::atomic_ullong x(1);
#            return x.fetch_add(1);
#        }"
#        
#    VX_ATOMIC_TEST)
#    
#    if(NOT VX_ATOMIC_TEST)
#        message(WARNING "Linking against libatomic due to lack of support for atomic operations.")
#        target_link_libraries(Vertex PRIVATE atomic)
#    endif()
#    
#endif()

#--------------------------------------------------------------------
# Files
#--------------------------------------------------------------------

# Include directories for headers
target_include_directories(Vertex
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"     # When building the project
    PUBLIC "$<INSTALL_INTERFACE:include>"                               # When using the installed project
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Header-only interface to be inherited by builds linking against Vertex
target_include_directories(Vertex_Headers INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"            # When building the project
    "$<INSTALL_INTERFACE:include>"                                      # When using the installed project
)

# Source files for the library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/include/vertex")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl")
vx_create_source_groups(Vertex "${CMAKE_CURRENT_SOURCE_DIR}")

# Third party header files for the library
#target_include_directories(Vertex
#    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party>" # When building the project
#    PUBLIC "$<INSTALL_INTERFACE:include>"                               # When using the installed project
#    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
#)

# stb_image
#target_include_directories(Vertex PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include")
#
#file(GLOB_RECURSE STB_IMAGE_FILES
#    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/*.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image_impl/*.cpp"
#)
#vx_create_source_group(STB_IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}")
#
#target_sources(Vertex PRIVATE ${STB_IMAGE_FILES})

# Handle Windows resource file for shared library builds
if(VX_BUILD_SHARED_LIBS AND VX_CMAKE_PLATFORM_WINDOWS)

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_impl/os/__platform/windows/version.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/vertex.rc"
        @ONLY
    )
    
    # Add rc to sources
    target_sources(Vertex PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/vertex.rc")
    
endif()

#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------

# Install headers
install(
    DIRECTORY "include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.hpp"
)

# Install and export headers interface
install(
    TARGETS Vertex_Headers
    EXPORT VertexHeaderTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Install library (Vertex shared or static)
install(
    TARGETS Vertex
    EXPORT VertexTargets
    ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"   # For static libraries
    LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"   # For shared libraries
    RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"   # For executables (if any)
    FRAMEWORK     DESTINATION "."
)

# Install PDB files (for debugging on Windows)
if(MSVC)

    if(VX_BUILD_SHARED_LIBS)
        # For shared libs we can just set it up like this
        install(FILES "$<TARGET_PDB_FILE:Vertex>" DESTINATION "${CMAKE_INSTALL_BINDIR}" CONFIGURATIONS Debug OPTIONAL)
    else()
        # For static libs we need to manually locate the pdb files in the install directory and install them
        install(CODE "
            file(GLOB_RECURSE PDB_FILES \"${CMAKE_BINARY_DIR}/lib/Vertex*.pdb\")
            foreach(PDB \${PDB_FILES})
                file(INSTALL DESTINATION \"${CMAKE_INSTALL_PREFIX}/lib\" TYPE FILE FILES \${PDB})
            endforeach()
        " CONFIGURATIONS Debug)
    endif()
    
endif()

# Install the cmake config files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/VertexConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/VertexConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/VertexConfig.cmake"
    PATH_VARS CMAKE_INSTALL_PREFIX
    INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/VertexConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VertexConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
)

#--------------------------------------------------------------------
# Export Library Targets
#--------------------------------------------------------------------

# Install export file for headers
install(EXPORT VertexHeaderTargets
    FILE "VertexHeaderTargets.cmake"
    NAMESPACE Vertex::
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
)

# Export shared or static library targets based on the build
if(VX_BUILD_SHARED_LIBS)

    install(EXPORT VertexTargets
        FILE "VertexSharedTargets.cmake"
        NAMESPACE Vertex::
        DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
    )
    
else()

    install(EXPORT VertexTargets
        FILE "VertexStaticTargets.cmake"
        NAMESPACE Vertex::
        DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
    )
    
endif()