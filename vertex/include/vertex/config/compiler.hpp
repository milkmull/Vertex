#pragma once

#include "vertex/config/version_number.hpp"

// https://www.VX.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/VX_comp_compiler_macros.html
// https://github.com/VXorg/predef/blob/e1211a4ca467bb6512e99025772ca25afa8d6159/include/VX/predef/compiler.h

///////////////////////////////////////////////////////////////////////////////
// clang
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/compiler/clang.h

#if defined(__clang__)

#   define VX_COMPILER_CLANG VX_VERSION_NUMBER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#   define _VX_COMPILER_DETECTED

#endif // clang

///////////////////////////////////////////////////////////////////////////////
// gcc
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/compiler/gcc.h

// compilers capable of emulating gcc: clang, tcc
// Clang and tcc can emulate gcc, so they should be checked for first

#if defined(__GNUC__)

// Detect version

#   if defined(__GNUC_PATCHLEVEL__)
#       define _VX_COMPILER_GNUC_DETECTION VX_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#   else
#       define _VX_COMPILER_GNUC_DETECTION VX_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, 0)
#   endif

// Emulated?

#   if defined(_VX_COMPILER_DETECTED)
#       define VX_COMPILER_GNUC_EMULATED _VX_COMPILER_GNUC_DETECTION
#   else
#       define VX_COMPILER_GNUC _VX_COMPILER_GNUC_DETECTION
#       define _VX_COMPILER_DETECTED
#   endif

// Cleanup

#   undef _VX_COMPILER_GNUC_DETECTION

#endif // gcc

///////////////////////////////////////////////////////////////////////////////
// msvc
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/compiler/visualc.h

// Clang can emulate msvc, so it should be checked for first

#if defined(_MSC_VER)

// Detect version

#   if !defined(_MSC_FULL_VER)

#       define _VX_COMPILER_MSVC_BUILD 0

#   else

#       if _MSC_FULL_VER / 10000 == _MSC_VER
#           define _VX_COMPILER_MSVC_BUILD (_MSC_FULL_VER % 10000)
#       elif _MSC_FULL_VER / 100000 == _MSC_VER
#           define _VX_COMPILER_MSVC_BUILD (_MSC_FULL_VER % 100000)
#       else
#           error "Cannot determine build number from _MSC_FULL_VER"
#       endif

#   endif

#   define _VX_COMPILER_MSVC_DETECTION VX_VERSION_NUMBER(_MSC_VER / 100 - 6, _MSC_VER % 100, _VX_COMPILER_MSVC_BUILD)

// Emulated?

#   if defined(_VX_COMPILER_DETECTED)
#       define VX_COMPILER_MSVC_EMULATED _VX_COMPILER_MSVC_DETECTION
#   else
#       define VX_COMPILER_MSVC_DETECTION _VX_COMPILER_MSVC_DETECTION
#       define _VX_COMPILER_DETECTED
#   endif

// Cleanup

#   undef _VX_COMPILER_MSVC_BUILD

#endif // msvc

///////////////////////////////////////////////////////////////////////////////
// cleanup
///////////////////////////////////////////////////////////////////////////////

#ifndef _VX_COMPILER_DETECTED
#   define VX_COMPILER_UNKNOWN VX_VERSION_NUMBER_ZERO
#endif

#if defined(_VX_COMPILER_DETECTED)
#   undef _VX_COMPILER_DETECTED
#endif