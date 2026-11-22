#pragma once

//=========================================================================
// Source Location
//=========================================================================

#define VX_FILE __FILE__
#define VX_LINE __LINE__

// Basic (standardized) function name
#if VX_CPP_STANDARD >= 11
    #define VX_FUNCTION __func__
#elif defined(__FUNCTION__)
    #define VX_FUNCTION __FUNCTION__
#else
    #define VX_FUNCTION "unknown function"
#endif

// Full signature (compiler-specific, includes templates, namespaces, etc.)
#if defined(__clang__) || defined(__GNUC__)
    #define VX_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define VX_PRETTY_FUNCTION __FUNCSIG__
#else
    #define VX_PRETTY_FUNCTION VX_FUNCTION
#endif
