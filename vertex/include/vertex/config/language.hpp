#pragma once

///////////////////////////////////////////////////////////////////////////////
// C++ Standard Detection
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && defined(_MSVC_LANG)
#    define VX_CPP_STD _MSVC_LANG
#elif defined(__cplusplus)
#    define VX_CPP_STD __cplusplus
#else
#    define VX_CPP_STD 0L
#endif

// Determine the C++ standard version
#if VX_CPP_STD >= 202302L
#   define VX_CPP_STANDARD 23
#elif VX_CPP_STD >= 202002L
#   define VX_CPP_STANDARD 20
#elif VX_CPP_STD >= 201703L
#   define VX_CPP_STANDARD 17
#elif VX_CPP_STD >= 201402L
#   define VX_CPP_STANDARD 14
#elif VX_CPP_STD >= 201103L
#   define VX_CPP_STANDARD 11
#elif VX_CPP_STD >= 199711L
#   define VX_CPP_STANDARD 98
#else
#   define VX_CPP_STANDARD 0
#endif

// Cleanup the temporary macro after usage
#undef VX_CPP_STD
