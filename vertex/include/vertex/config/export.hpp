#pragma once

//=========================================================================
// DLL Import / Export
//=========================================================================

#if !defined(VX_BUILD_STATIC)

    #if defined(_MSC_VER)
        #define VX_API_EXPORT __declspec(dllexport)
        #define VX_API_IMPORT __declspec(dllimport)
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define VX_API_EXPORT __attribute__((visibility("default")))
        #define VX_API_IMPORT __attribute__((visibility("default")))
    #else
        #define VX_API_EXPORT
        #define VX_API_IMPORT
    #endif

#else
    #define VX_API_EXPORT
    #define VX_API_IMPORT
#endif

#if defined(VX_BUILD_SHARED)
    #define VX_API VX_API_EXPORT
#else
    #define VX_API VX_API_IMPORT
#endif
