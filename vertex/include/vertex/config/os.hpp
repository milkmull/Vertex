#pragma once

#include "vertex/config/version_number.hpp"

// https://github.com/VXorg/predef/tree/e1211a4ca467bb6512e99025772ca25afa8d6159/include/VX/predef/os

#if !defined(VX_DUMMY_PLATFORM)

///////////////////////////////////////////////////////////////////////////////
// cygwin
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/cygwin.h

#if !defined(_VX_OS_DETECTED) && defined(__CYGWIN__)

#   include <cygwin/version.h>
#   define VX_OS_CYGWIN VX_VERSION_NUMBER(CYGWIN_VERSION_API_MAJOR, CYGWIN_VERSION_API_MINOR, 0)
#   define _VX_OS_DETECTED

#endif // cygwin

///////////////////////////////////////////////////////////////////////////////
// linux
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/linux.h

#if !defined(_VX_OS_DETECTED) && ( \
    defined(linux) || defined(__linux) || \
    defined(__linux__) || defined(__gnu_linux__) \
    )

#   define VX_OS_LINUX VX_VERSION_NUMBER_AVAILABLE
#   define _VX_OS_DETECTED

#endif // linux

///////////////////////////////////////////////////////////////////////////////
// ios
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/ios.h

#if !defined(_VX_OS_DETECTED) && ( \
    defined(__APPLE__) && defined(__MACH__) && \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
    )

#   define VX_OS_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ * 1000) // E.g. 13.4 -> 13400
#   define _VX_OS_DETECTED

#endif // ios

///////////////////////////////////////////////////////////////////////////////
// macos
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/macos.h

// macos should be detected after ios

#if !defined(_VX_OS_DETECTED) && ( \
    defined(macintosh) || defined(Macintosh) || \
    (defined(__APPLE__) && defined(__MACH__)) \
    )

#   if defined(__APPLE__) && defined(__MACH__)
#       define VX_OS_MACOS VX_VERSION_NUMBER(10, 0, 0)
#   else
#       define VX_OS_MACOS VX_VERSION_NUMBER(9, 0, 0)
#   endif

#   define _VX_OS_DETECTED

#endif // macos

///////////////////////////////////////////////////////////////////////////////
// apple
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_OS_IOS) || defined(VX_OS_MACOS)

#   define VX_OS_APPLE

#endif // apple

///////////////////////////////////////////////////////////////////////////////
// bsd
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd.h

// OSX will define BSD predefs if the sys/param.h
// header is included. We can guard against that, but only if we
// detect OSX first. ^

///////////////////////////////////////
// bsdi
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd/bsdi.h

#if !defined(_VX_OS_DETECTED) && defined(__bsdi__)

#   define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
#   define VX_OS_BSD_BSDI VX_VERSION_NUMBER_AVAILABLE
#   define _VX_OS_DETECTED

#endif // bsdi

///////////////////////////////////////
// dragonfly
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd/dragonfly.h

#if !defined(_VX_OS_DETECTED) && defined(__DragonFly__)

#   define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
#   define VX_OS_DRAGONFLY_BSD VX_VERSION_NUMBER_AVAILABLE
#   define _VX_OS_DETECTED

#endif // dragonfly

///////////////////////////////////////
// free
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd/free.h

#if !defined(_VX_OS_DETECTED) && defined(__FreeBSD__)

#   define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
#   include <sys/param.h>

#   if defined(__FreeBSD_version)

#       if (__FreeBSD_version == 491000)
#           define VX_OS_BSD_FREE VX_VERSION_NUMBER(4, 10, 0)
#       elif (__FreeBSD_version == 492000)
#           define VX_OS_BSD_FREE VX_VERSION_NUMBER(4, 11, 0)
#       elif (__FreeBSD_version < 500000)
#           define VX_OS_BSD_FREE _VX_MAKE_VERSION_NUMBER_10_VRPPPP(__FreeBSD_version)
#       else
#           define VX_OS_BSD_FREE _VX_MAKE_VERSION_NUMBER_10_VVRRPPP(__FreeBSD_version)
#       endif

#   else

#       define VX_OS_BSD_FREE VX_VERSION_NUMBER_AVAILABLE

#   endif

#   define _VX_OS_DETECTED

#endif // free

///////////////////////////////////////
// net
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd/net.h

#if !defined(_VX_OS_DETECTED) && (defined(__NETBSD__) || defined(__NetBSD__))

#   define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE

#   if defined(__NETBSD__)

#       if defined(__NETBSD_version)
#           if (__NETBSD_version < 500000)
#               define VX_OS_BSD_NET _VX_MAKE_VERSION_NUMBER_10_VRP000(__NETBSD_version)
#           else
#               define VX_OS_BSD_NET _VX_MAKE_VERSION_NUMBER_10_VRR000(__NETBSD_version)
#           endif
#       else
#           define VX_OS_BSD_NET VX_VERSION_NUMBER_AVAILABLE
#       endif

#   elif defined(__NetBSD__)

#       if defined(NetBSD0_8)
#           define VX_OS_BSD_NET VX_VERSION_NUMBER(0, 8, 0)
#       elif defined(NetBSD0_9)
#           define VX_OS_BSD_NET VX_VERSION_NUMBER(0, 9, 0)
#       elif defined(NetBSD1_0)
#           define VX_OS_BSD_NET VX_VERSION_NUMBER(1, 0, 0)
#       elif defined(__NetBSD_Version)
#           define VX_OS_BSD_NET _VX_MAKE_VERSION_NUMBER_10_VVRR00PP00(__NetBSD_Version)
#       else
#           define VX_OS_BSD_NET VX_VERSION_NUMBER_AVAILABLE
#       endif

#   endif

#   define _VX_OS_DETECTED

#endif // net

///////////////////////////////////////
// open
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd/open.h

#if !defined(_VX_OS_DETECTED) && defined(__OpenBSD__)

#   define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE

#   if defined(OpenBSD2_0)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 0, 0)
#   elif defined(OpenBSD2_1)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 1, 0)
#   elif defined(OpenBSD2_2)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 2, 0)
#   elif defined(OpenBSD2_3)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 3, 0)
#   elif defined(OpenBSD2_4)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 4, 0)
#   elif defined(OpenBSD2_5)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 5, 0)
#   elif defined(OpenBSD2_6)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 6, 0)
#   elif defined(OpenBSD2_7)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 7, 0)
#   elif defined(OpenBSD2_8)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 8, 0)
#   elif defined(OpenBSD2_9)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 9, 0)
#   elif defined(OpenBSD3_0)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 0, 0)
#   elif defined(OpenBSD3_1)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 1, 0)
#   elif defined(OpenBSD3_2)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 2, 0)
#   elif defined(OpenBSD3_3)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 3, 0)
#   elif defined(OpenBSD3_4)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 4, 0)
#   elif defined(OpenBSD3_5)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 5, 0)
#   elif defined(OpenBSD3_6)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 6, 0)
#   elif defined(OpenBSD3_7)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 7, 0)
#   elif defined(OpenBSD3_8)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 8, 0)
#   elif defined(OpenBSD3_9)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 9, 0)
#   elif defined(OpenBSD4_0)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 0, 0)
#   elif defined(OpenBSD4_1)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 1, 0)
#   elif defined(OpenBSD4_2)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 2, 0)
#   elif defined(OpenBSD4_3)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 3, 0)
#   elif defined(OpenBSD4_4)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 4, 0)
#   elif defined(OpenBSD4_5)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 5, 0)
#   elif defined(OpenBSD4_6)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 6, 0)
#   elif defined(OpenBSD4_7)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 7, 0)
#   elif defined(OpenBSD4_8)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 8, 0)
#   elif defined(OpenBSD4_9)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 9, 0)
#   elif defined(OpenBSD5_0)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 0, 0)
#   elif defined(OpenBSD5_1)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 1, 0)
#   elif defined(OpenBSD5_2)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 2, 0)
#   elif defined(OpenBSD5_3)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 3, 0)
#   elif defined(OpenBSD5_4)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 4, 0)
#   elif defined(OpenBSD5_5)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 5, 0)
#   elif defined(OpenBSD5_6)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 6, 0)
#   elif defined(OpenBSD5_7)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 7, 0)
#   elif defined(OpenBSD5_8)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 8, 0)
#   elif defined(OpenBSD5_9)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 9, 0)
#   elif defined(OpenBSD6_0)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 0, 0)
#   elif defined(OpenBSD6_1)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 1, 0)
#   elif defined(OpenBSD6_2)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 2, 0)
#   elif defined(OpenBSD6_3)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 3, 0)
#   elif defined(OpenBSD6_4)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 4, 0)
#   elif defined(OpenBSD6_5)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 5, 0)
#   elif defined(OpenBSD6_6)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 6, 0)
#   elif defined(OpenBSD6_7)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 7, 0)
#   elif defined(OpenBSD6_8)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 8, 0)
#   elif defined(OpenBSD6_9)
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 9, 0)
#   else
#       define VX_OS_BSD_OPEN VX_VERSION_NUMBER_AVAILABLE
#   endif

#   define _VX_OS_DETECTED

#endif // open

///////////////////////////////////////
// generic bsd fallback
///////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/bsd.h

#if !defined(_VX_OS_DETECTED) && (defined(BSD) || defined(_SYSTYPE_BSD))

#   include <sys/param.h>

#   if defined(BSD4_4)
#       define VX_OS_BSD BOOST_VERSION_NUMBER(4, 4, 0)
#   elif defined(BSD4_3)
#       define VX_OS_BSD BOOST_VERSION_NUMBER(4, 3, 0)
#   elif defined(BSD4_2)
#       define VX_OS_BSD BOOST_VERSION_NUMBER(4, 2, 0)
#   elif defined(BSD)
#       define VX_OS_BSD _VX_MAKE_VERSION_NUMBER_10_VVRR(BSD)
#   else
#       define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
#   endif

#   define _VX_OS_DETECTED

#endif // bsd

///////////////////////////////////////////////////////////////////////////////
// windows
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/os/windows.h

#if !defined(_VX_OS_DETECTED) && ( \
    defined(_WIN32) || defined(_WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__) \
    )

#   define VX_OS_WINDOWS VX_VERSION_NUMBER_AVAILABLE
#   define _VX_OS_DETECTED

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

#ifndef _VX_OS_DETECTED
#   define VX_OS_DUMMY VX_VERSION_NUMBER_AVAILABLE
#   define _VX_OS_DETECTED
#endif

///////////////////////////////////////////////////////////////////////////////
// cleanup
///////////////////////////////////////////////////////////////////////////////

#if defined(_VX_OS_DETECTED)
#   undef _VX_OS_DETECTED
#endif