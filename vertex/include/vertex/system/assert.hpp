#pragma once

#include <cassert>
#include <iostream>

#include "vertex/system/compiler.hpp"

///////////////////////////////////////////////////////////////////////////////
// Assert Level
///////////////////////////////////////////////////////////////////////////////

#if !defined(VX_ASSERT_LEVEL)

#   if (VX_DEBUG)
#       define VX_ASSERT_LEVEL 2
#   else
#       define VX_ASSERT_LEVEL 1
#   endif

#endif

///////////////////////////////////////////////////////////////////////////////
// File and Line Information
///////////////////////////////////////////////////////////////////////////////

#define VX_FILE __FILE__
#define VX_LINE __LINE__

///////////////////////////////////////////////////////////////////////////////
// Function Info
///////////////////////////////////////////////////////////////////////////////

#if VX_CPP_STANDARD >= 11 // C++11 and later
#   define VX_FUNCTION __func__
#elif ((defined(__GNUC__) && (__GNUC__ >= 2)) || defined(_MSC_VER) || defined (__WATCOMC__))
#   define VX_FUNCTION __FUNCTION__
#else
#   define VX_FUNCTION "???"
#endif

///////////////////////////////////////////////////////////////////////////////
// Assert
///////////////////////////////////////////////////////////////////////////////

#define VX_ASSERT_HANDLER(condition, msg, file, line, function) \
    do \
    { \
        if (!(condition)) \
        { \
            std::cerr << "Assertion failed: " << #condition << '\n' \
            << "Message: " << msg << '\n' \
            << "File: " << file << '\n' \
            << "Line: " << line << '\n' \
            << "Function: " << function << '\n'; \
            std::abort(); /* Terminate the program */ \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#if VX_ASSERT_LEVEL >= 2

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) VX_ASSERT_HANDLER(condition, msg, VX_FILE, VX_LINE, VX_FUNCTION)
#   define VX_ASSERT_MESSAGE(condition, msg) VX_ASSERT_RELEASE_MESSAGE(condition, msg)

#elif VX_ASSERT_LEVEL == 1

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) VX_ASSERT_HANDLER(condition, msg, VX_FILE, VX_LINE, VX_FUNCTION)
#   define VX_ASSERT_MESSAGE(condition, msg) ((void)0)

#else

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) ((void)0)
#   define VX_ASSERT_MESSAGE(condition, msg) ((void)0)

#endif

#define VX_ASSERT_RELEASE(condition) VX_ASSERT_RELEASE_MESSAGE(condition, "assertion failed")
#define VX_ASSERT(condition) VX_ASSERT_MESSAGE(condition, "assertion failed")

