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
// Text
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define __VX_TEXT(x) L##x
#else
#   define __VX_TEXT(x) x
#endif

#define VX_TEXT(x) __VX_TEXT(x)