#pragma once

//=========================================================================
// Debug / Release
//=========================================================================

// Allow explicit override from the build system: -DVX_DEBUG=0/1
#ifndef VX_DEBUG
    #if defined(VX_CMAKE_DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
        #define VX_DEBUG 1
    #else
        #define VX_DEBUG 0
    #endif
#endif

#if VX_DEBUG
    #define VX_RELEASE    0
    #define VX_BUILD_TYPE "Debug"
#else
    #define VX_RELEASE    1
    #define VX_BUILD_TYPE "Release"
#endif
