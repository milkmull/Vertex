#pragma once

// OS detection, derived from Boost.Predef
// (https://github.com/boostorg/predef), Copyright Rene Rivera et al.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// Every VX_OS_* macro is either undefined (not detected) or defined to a
// version number (VX_VERSION_NUMBER_AVAILABLE when no version can be
// determined). Use `#if defined(VX_OS_X)` or `#if VX_OS_X`.
//
// Specific OS tags are mutually exclusive. Family tags are set alongside them:
//   VX_OS_BSD    any BSD (set together with VX_OS_BSD_FREE, _NET, _OPEN, ...)
//   VX_OS_APPLE  any Apple OS (iOS, tvOS, watchOS, visionOS, macOS, and
//                classic Mac OS, which is reported as VX_OS_MACOS 9.0.0)
//   VX_OS_UNIX   anything that looks Unix-like. This can be set while
//                VX_OS_UNKNOWN is also set (e.g. Solaris, AIX, QNX, Emscripten).
//
// Platform notes:
//   - Android defines __linux__, so it is reported as VX_OS_LINUX.
//   - Mac Catalyst sets TARGET_OS_IPHONE, so it is reported as VX_OS_IOS.
//   - DriverKit falls into the macOS branch and is reported as VX_OS_MACOS.
//   - VX_OS_UNKNOWN is defined when no specific OS was detected.
//
// Define VX_OS_DISABLE_DETECTION before including this header to skip all
// detection (VX_OS_UNKNOWN is then defined).
//
// Requires from version_number.hpp:
//   VX_VERSION_NUMBER, VX_VERSION_NUMBER_AVAILABLE,
//   VX_PRIV_MAKE_VERSION_NUMBER_10_VRPPPP, _VVRRPPP, _VRP000, _VRR000,
//   _VVRR00PP00, _VVRR

#include "vertex/config/version_number.hpp"

#if !defined(VX_OS_DISABLE_DETECTION)

//=============================================================================
// cygwin
//=============================================================================

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__CYGWIN__)

        #include <cygwin/version.h>
        #define VX_OS_CYGWIN VX_VERSION_NUMBER(CYGWIN_VERSION_API_MAJOR, CYGWIN_VERSION_API_MINOR, 0)
        #define VX_PRIV_OS_DETECTED

    #endif // cygwin

//=============================================================================
// linux (also covers Android, which defines __linux__)
//=============================================================================

    #if !defined(VX_PRIV_OS_DETECTED) && (defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__))

        #define VX_OS_LINUX VX_VERSION_NUMBER_AVAILABLE
        #define VX_PRIV_OS_DETECTED

    #endif // linux

//=============================================================================
// apple (ios, tvos, watchos, visionos, macos)
//=============================================================================

// Uses TargetConditionals.h rather than the iPhone version macro, because
// tvOS/watchOS/visionOS define __APPLE__ && __MACH__ too. The non-macOS
// variants must be checked first: they also set TARGET_OS_IPHONE.
//
// __ENVIRONMENT_*_VERSION_MIN_REQUIRED__ is VVRRPP (e.g. 13.4.0 -> 130400),
// so multiplying by 1000 yields VX_VERSION_NUMBER(13, 4, 0).

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__APPLE__) && defined(__MACH__)

        #include <TargetConditionals.h>

        #if defined(TARGET_OS_TV) && TARGET_OS_TV

            #if defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
                #define VX_OS_TVOS (__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__ * 1000)
            #else
                #define VX_OS_TVOS VX_VERSION_NUMBER_AVAILABLE
            #endif

        #elif defined(TARGET_OS_WATCH) && TARGET_OS_WATCH

            #if defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
                #define VX_OS_WATCHOS (__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__ * 1000)
            #else
                #define VX_OS_WATCHOS VX_VERSION_NUMBER_AVAILABLE
            #endif

        #elif defined(TARGET_OS_VISION) && TARGET_OS_VISION

            #define VX_OS_VISIONOS VX_VERSION_NUMBER_AVAILABLE

        #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE

            #if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
                #define VX_OS_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ * 1000)
            #else
                #define VX_OS_IOS VX_VERSION_NUMBER_AVAILABLE
            #endif

        #else // macOS

            // Mixed formats: 10.9 -> 1090 (VVRP), 10.15 -> 101500 and 11.0 -> 110000 (VVRRPP)
            #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)

                #if (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 100000)
                    #define VX_OS_MACOS VX_VERSION_NUMBER( \
                        __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ / 10000, \
                        (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ / 100) % 100, \
                        __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ % 100)
                #else
                    #define VX_OS_MACOS VX_VERSION_NUMBER( \
                        __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ / 100, \
                        (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ / 10) % 10, \
                        __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ % 10)
                #endif

            #else

                #define VX_OS_MACOS VX_VERSION_NUMBER(10, 0, 0)

            #endif

        #endif

        #define VX_PRIV_OS_DETECTED

    #endif // apple (mach)

// Classic Mac OS

    #if !defined(VX_PRIV_OS_DETECTED) && (defined(macintosh) || defined(Macintosh))

        #define VX_OS_MACOS VX_VERSION_NUMBER(9, 0, 0)
        #define VX_PRIV_OS_DETECTED

    #endif // classic mac

// Umbrella tag for any Apple OS

    #if defined(VX_OS_IOS) || defined(VX_OS_TVOS) || defined(VX_OS_WATCHOS) || \
        defined(VX_OS_VISIONOS) || defined(VX_OS_MACOS)

        #define VX_OS_APPLE VX_VERSION_NUMBER_AVAILABLE

    #endif // apple umbrella

//=============================================================================
// bsd
//=============================================================================

// Apple must be detected before BSD: macOS defines BSD macros once
// <sys/param.h> is included.

//=====================================
// bsdi
//=====================================

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__bsdi__)

        #define VX_OS_BSD      VX_VERSION_NUMBER_AVAILABLE
        #define VX_OS_BSD_BSDI VX_VERSION_NUMBER_AVAILABLE
        #define VX_PRIV_OS_DETECTED

    #endif // bsdi

//=====================================
// dragonfly
//=====================================

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__DragonFly__)

        #define VX_OS_BSD           VX_VERSION_NUMBER_AVAILABLE
        #define VX_OS_DRAGONFLY_BSD VX_VERSION_NUMBER_AVAILABLE
        #define VX_PRIV_OS_DETECTED

    #endif // dragonfly

//=====================================
// free
//=====================================

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__FreeBSD__)

        #define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
        #include <sys/param.h>

        #if defined(__FreeBSD_version)

            #if (__FreeBSD_version == 491000)
                #define VX_OS_BSD_FREE VX_VERSION_NUMBER(4, 10, 0)
            #elif (__FreeBSD_version == 492000)
                #define VX_OS_BSD_FREE VX_VERSION_NUMBER(4, 11, 0)
            #elif (__FreeBSD_version < 500000)
                #define VX_OS_BSD_FREE VX_PRIV_MAKE_VERSION_NUMBER_10_VRPPPP(__FreeBSD_version)
            #else
                #define VX_OS_BSD_FREE VX_PRIV_MAKE_VERSION_NUMBER_10_VVRRPPP(__FreeBSD_version)
            #endif

        #else

            #define VX_OS_BSD_FREE VX_VERSION_NUMBER_AVAILABLE

        #endif

        #define VX_PRIV_OS_DETECTED

    #endif // free

//=====================================
// net
//=====================================

    #if !defined(VX_PRIV_OS_DETECTED) && (defined(__NETBSD__) || defined(__NetBSD__))

        #define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
        #include <sys/param.h> // NetBSD version macros live here

        #if defined(__NETBSD__)

            #if defined(__NETBSD_version)
                #if (__NETBSD_version < 500000)
                    #define VX_OS_BSD_NET VX_PRIV_MAKE_VERSION_NUMBER_10_VRP000(__NETBSD_version)
                #else
                    #define VX_OS_BSD_NET VX_PRIV_MAKE_VERSION_NUMBER_10_VRR000(__NETBSD_version)
                #endif
            #else
                #define VX_OS_BSD_NET VX_VERSION_NUMBER_AVAILABLE
            #endif

        #elif defined(__NetBSD__)

            #if defined(NetBSD0_8)
                #define VX_OS_BSD_NET VX_VERSION_NUMBER(0, 8, 0)
            #elif defined(NetBSD0_9)
                #define VX_OS_BSD_NET VX_VERSION_NUMBER(0, 9, 0)
            #elif defined(NetBSD1_0)
                #define VX_OS_BSD_NET VX_VERSION_NUMBER(1, 0, 0)
            #elif defined(__NetBSD_Version__)
                #define VX_OS_BSD_NET VX_PRIV_MAKE_VERSION_NUMBER_10_VVRR00PP00(__NetBSD_Version__)
            #else
                #define VX_OS_BSD_NET VX_VERSION_NUMBER_AVAILABLE
            #endif

        #endif

        #define VX_PRIV_OS_DETECTED

    #endif // net

//=====================================
// open
//=====================================

// <sys/param.h> defines every OpenBSDx_y macro up to the current release, so
// the chain must be checked from the newest version down. Add a new branch at
// the top for each release.

    #if !defined(VX_PRIV_OS_DETECTED) && defined(__OpenBSD__)

        #define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
        #include <sys/param.h>

        #if defined(OpenBSD7_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 9, 0)
        #elif defined(OpenBSD7_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 8, 0)
        #elif defined(OpenBSD7_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 7, 0)
        #elif defined(OpenBSD7_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 6, 0)
        #elif defined(OpenBSD7_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 5, 0)
        #elif defined(OpenBSD7_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 4, 0)
        #elif defined(OpenBSD7_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 3, 0)
        #elif defined(OpenBSD7_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 2, 0)
        #elif defined(OpenBSD7_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 1, 0)
        #elif defined(OpenBSD7_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(7, 0, 0)
        #elif defined(OpenBSD6_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 9, 0)
        #elif defined(OpenBSD6_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 8, 0)
        #elif defined(OpenBSD6_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 7, 0)
        #elif defined(OpenBSD6_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 6, 0)
        #elif defined(OpenBSD6_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 5, 0)
        #elif defined(OpenBSD6_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 4, 0)
        #elif defined(OpenBSD6_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 3, 0)
        #elif defined(OpenBSD6_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 2, 0)
        #elif defined(OpenBSD6_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 1, 0)
        #elif defined(OpenBSD6_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(6, 0, 0)
        #elif defined(OpenBSD5_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 9, 0)
        #elif defined(OpenBSD5_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 8, 0)
        #elif defined(OpenBSD5_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 7, 0)
        #elif defined(OpenBSD5_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 6, 0)
        #elif defined(OpenBSD5_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 5, 0)
        #elif defined(OpenBSD5_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 4, 0)
        #elif defined(OpenBSD5_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 3, 0)
        #elif defined(OpenBSD5_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 2, 0)
        #elif defined(OpenBSD5_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 1, 0)
        #elif defined(OpenBSD5_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(5, 0, 0)
        #elif defined(OpenBSD4_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 9, 0)
        #elif defined(OpenBSD4_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 8, 0)
        #elif defined(OpenBSD4_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 7, 0)
        #elif defined(OpenBSD4_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 6, 0)
        #elif defined(OpenBSD4_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 5, 0)
        #elif defined(OpenBSD4_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 4, 0)
        #elif defined(OpenBSD4_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 3, 0)
        #elif defined(OpenBSD4_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 2, 0)
        #elif defined(OpenBSD4_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 1, 0)
        #elif defined(OpenBSD4_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(4, 0, 0)
        #elif defined(OpenBSD3_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 9, 0)
        #elif defined(OpenBSD3_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 8, 0)
        #elif defined(OpenBSD3_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 7, 0)
        #elif defined(OpenBSD3_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 6, 0)
        #elif defined(OpenBSD3_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 5, 0)
        #elif defined(OpenBSD3_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 4, 0)
        #elif defined(OpenBSD3_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 3, 0)
        #elif defined(OpenBSD3_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 2, 0)
        #elif defined(OpenBSD3_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 1, 0)
        #elif defined(OpenBSD3_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(3, 0, 0)
        #elif defined(OpenBSD2_9)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 9, 0)
        #elif defined(OpenBSD2_8)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 8, 0)
        #elif defined(OpenBSD2_7)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 7, 0)
        #elif defined(OpenBSD2_6)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 6, 0)
        #elif defined(OpenBSD2_5)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 5, 0)
        #elif defined(OpenBSD2_4)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 4, 0)
        #elif defined(OpenBSD2_3)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 3, 0)
        #elif defined(OpenBSD2_2)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 2, 0)
        #elif defined(OpenBSD2_1)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 1, 0)
        #elif defined(OpenBSD2_0)
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER(2, 0, 0)
        #else
            #define VX_OS_BSD_OPEN VX_VERSION_NUMBER_AVAILABLE
        #endif

        #define VX_PRIV_OS_DETECTED

    #endif // open

//=====================================
// generic bsd fallback
//=====================================

    #if !defined(VX_PRIV_OS_DETECTED) && (defined(BSD) || defined(_SYSTYPE_BSD))

        #include <sys/param.h>

        #if defined(BSD4_4)
            #define VX_OS_BSD VX_VERSION_NUMBER(4, 4, 0)
        #elif defined(BSD4_3)
            #define VX_OS_BSD VX_VERSION_NUMBER(4, 3, 0)
        #elif defined(BSD4_2)
            #define VX_OS_BSD VX_VERSION_NUMBER(4, 2, 0)
        #elif defined(BSD)
            #define VX_OS_BSD VX_PRIV_MAKE_VERSION_NUMBER_10_VVRR(BSD)
        #else
            #define VX_OS_BSD VX_VERSION_NUMBER_AVAILABLE
        #endif

        #define VX_PRIV_OS_DETECTED

    #endif // bsd

//=============================================================================
// windows
//=============================================================================

    #if !defined(VX_PRIV_OS_DETECTED) && (defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__))

        #define VX_OS_WINDOWS VX_VERSION_NUMBER_AVAILABLE
        #define VX_PRIV_OS_DETECTED

    #endif // windows

//=============================================================================
// unix (a family tag: set alongside, not instead of, a specific OS)
//=============================================================================

// Feature-test macros such as _XOPEN_SOURCE and _POSIX_SOURCE are
// deliberately not used: build flags or libraries can set them anywhere.

    #if defined(VX_OS_LINUX) || defined(VX_OS_BSD) || defined(VX_OS_APPLE) || defined(VX_OS_CYGWIN) || \
        defined(unix) || defined(__unix) || defined(__unix__)

        #define VX_OS_UNIX VX_VERSION_NUMBER_AVAILABLE

    #endif // unix

#endif // !VX_OS_DISABLE_DETECTION

//=============================================================================
// unknown
//=============================================================================

#if !defined(VX_PRIV_OS_DETECTED)
    #define VX_OS_UNKNOWN VX_VERSION_NUMBER_AVAILABLE
#endif

//=============================================================================
// cleanup
//=============================================================================

#undef VX_PRIV_OS_DETECTED
