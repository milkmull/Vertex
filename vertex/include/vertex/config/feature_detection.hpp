#pragma once

//=========================================================================
// Feature Detection Helpers
//=========================================================================

#if defined(__has_builtin)
    #define VX_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define VX_HAS_BUILTIN(x) 0
#endif

#if defined(__has_include)
    #define VX_HAS_INCLUDE(header) __has_include(header)
#else
    #define VX_HAS_INCLUDE(header) 0
#endif

#if defined(__has_attribute)
    #define VX_HAS_COMPILER_ATTRIBUTE(x) __has_attribute(x)
#else
    #define VX_HAS_COMPILER_ATTRIBUTE(x) 0
#endif

#if defined(__has_cpp_attribute)
    #define VX_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define VX_HAS_ATTRIBUTE(x) 0
#endif
