#pragma once

#include <iostream>

#include "vertex/config/assert.hpp"

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
// Assert
///////////////////////////////////////////////////////////////////////////////

#define VX_ASSERT_HANDLER(condition, msg, file, line, function) \
    do \
    { \
        const bool condition_check = !!(condition); /* Do this to avoid constant expression warning */ \
        if (!condition_check) \
        { \
            std::cerr << "Assertion failed: " << #condition << '\n' \
            << "Message: " << msg << '\n' \
            << "File: " << file << '\n' \
            << "Line: " << line << '\n' \
            << "Function: " << function << '\n'; \
            std::abort(); /* Terminate the program */ \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#if (VX_ASSERT_LEVEL >= 2)

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) VX_ASSERT_HANDLER(condition, msg, VX_FILE, VX_LINE, VX_FUNCTION)
#   define VX_ASSERT_MESSAGE(condition, msg) VX_ASSERT_RELEASE_MESSAGE(condition, msg)

#elif (VX_ASSERT_LEVEL == 1)

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) VX_ASSERT_HANDLER(condition, msg, VX_FILE, VX_LINE, VX_FUNCTION)
#   define VX_ASSERT_MESSAGE(condition, msg) ((void)0)

#else

#   define VX_ASSERT_RELEASE_MESSAGE(condition, msg) ((void)0)
#   define VX_ASSERT_MESSAGE(condition, msg) ((void)0)

#endif

#if defined(VX_ASSERT)
#   undef VX_ASSERT
#endif

#define VX_ASSERT(condition) VX_ASSERT_MESSAGE(condition, "assertion failed")
#define VX_ASSERT_RELEASE(condition) VX_ASSERT_RELEASE_MESSAGE(condition, "assertion failed")
#define VX_STATIC_ASSERT(condition, msg) static_assert(condition, msg)

