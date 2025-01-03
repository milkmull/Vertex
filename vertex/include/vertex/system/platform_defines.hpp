#pragma once

#include "vertex/system/platform.hpp"

///////////////////////////////////////////////////////////////////////////////
// Null Device
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define VX_NULL_DEVICE "NUL:"
#else
#   define VX_NULL_DEVICE "/dev/null"
#endif

///////////////////////////////////////////////////////////////////////////////
// Line End
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define VX_LINE_END "\r\n"
#else
#   define VX_LINE_END "\n"
#endif

///////////////////////////////////////////////////////////////////////////////
// Null While Loop
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_NULL_WHILE_LOOP_CONDITION (0,0)
#else
#   define VX_NULL_WHILE_LOOP_CONDITION (0)
#endif

///////////////////////////////////////////////////////////////////////////////
// Native Text
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_PLATFORM_WINDOWS)
#   define VX_NATIVE_TEXT(x) L##x
#else
#   define VX_NATIVE_TEXT(x) x
#endif