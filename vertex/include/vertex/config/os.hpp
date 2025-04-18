#pragma once

#include "vertex/config/version_number.hpp"

// https://github.com/VXorg/predef/tree/e1211a4ca467bb6512e99025772ca25afa8d6159/include/VX/predef/os

#if !defined(VX_DUMMY_PLATFORM)

///////////////////////////////////////////////////////////////////////////////
// cygwin
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/cygwin.h

#if !defined(__VX_OS_DETECTED) && defined(__CYGWIN__)

#   include <cygwin/version.h>
#   define VX_OS_CYGWIN VX_VERSION_NUMBER(CYGWIN_VERSION_API_MAJOR, CYGWIN_VERSION_API_MINOR, 0)
#   define __VX_OS_DETECTED

#endif // cygwin

///////////////////////////////////////////////////////////////////////////////
// linux
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/linux.h

#if !defined(__VX_OS_DETECTED) && ( \
    defined(linux) || defined(__linux) || \
    defined(__linux__) || defined(__gnu_linux__) \
    )

#   define VX_OS_LINUX VX_VERSION_NUMBER_AVAILABLE
#   define __VX_OS_DETECTED

#endif // linux

///////////////////////////////////////////////////////////////////////////////
// ios
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/ios.h

#if !defined(__VX_OS_DETECTED) && ( \
    defined(__APPLE__) && defined(__MACH__) && \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
    )

#   define VX_OS_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ * 1000) // E.g. 13.4 -> 13400
#   define __VX_OS_DETECTED

#endif // ios

///////////////////////////////////////////////////////////////////////////////
// macos
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/macos.h

// macos should be detected after ios

#if !defined(__VX_OS_DETECTED) && ( \
    defined(macintosh) || defined(Macintosh) || \
    (defined(__APPLE__) && defined(__MACH__)) \
    )

#   if defined(__APPLE__) && defined(__MACH__)
#       define VX_OS_MACOS VX_VERSION_NUMBER(10, 0, 0)
#   else
#       define VX_OS_MACOS VX_VERSION_NUMBER(9, 0, 0)
#   endif

#   define __VX_OS_DETECTED

#endif // macos

///////////////////////////////////////////////////////////////////////////////
// apple
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_OS_IOS) || defined(VX_OS_MACOS)

#   define VX_OS_APPLE

#endif // apple

///////////////////////////////////////////////////////////////////////////////
// windows
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/windows.h

#if !defined(__VX_OS_DETECTED) && ( \
    defined(_WIN32) || defined(_WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__) \
    )

#   define VX_OS_WINDOWS VX_VERSION_NUMBER_AVAILABLE
#   define __VX_OS_DETECTED

#endif // windows

///////////////////////////////////////////////////////////////////////////////
// unix
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/unix.h

#if defined(unix) || defined(__unix) || \
    defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)

#   define VX_OS_UNIX VX_VERSION_NUMBER_AVAILABLE

#endif // unix

#endif // VX_DUMMY_PLATFORM

///////////////////////////////////////////////////////////////////////////////
// dummy
///////////////////////////////////////////////////////////////////////////////

#ifndef __VX_OS_DETECTED
#   define VX_OS_DUMMY VX_VERSION_NUMBER_AVAILABLE
#   define __VX_OS_DETECTED
#endif

///////////////////////////////////////////////////////////////////////////////
// cleanup
///////////////////////////////////////////////////////////////////////////////

#if defined(__VX_OS_DETECTED)
#   undef __VX_OS_DETECTED
#endif