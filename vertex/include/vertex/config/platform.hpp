#pragma once

// Platform detection, derived from Boost.Predef
// (https://github.com/boostorg/predef), Copyright Rene Rivera et al.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// Every VX_PLATFORM_* macro is either undefined (not detected) or defined to a
// version number (VX_VERSION_NUMBER_AVAILABLE when no version can be
// determined). Use `#if defined(VX_PLATFORM_X)` or `#if VX_PLATFORM_X`.
//
// A platform is a refinement of an OS (device vs. simulator, UWP vs. desktop,
// ...). The OS macros come from os.hpp, so VX_OS_DISABLE_DETECTION also
// switches off every OS-based platform below.
//
// Tags:
//   VX_PLATFORM_ANDROID
//   VX_PLATFORM_{IOS,TVOS,WATCHOS,VISIONOS}_{DEVICE,SIMULATOR}
//   VX_PLATFORM_MINGW        any MinGW. Set together with one of:
//   VX_PLATFORM_MINGW64      64-bit targets (__MINGW64__)
//   VX_PLATFORM_MINGW32      32-bit targets only. A 32-bit mingw-w64 build
//                            lands here, because it does not define __MINGW64__.
//   VX_PLATFORM_WINDOWS_{DESKTOP,UWP,PHONE,SYSTEM,SERVER,STORE,RUNTIME}
//                            from WINAPI_FAMILY. RUNTIME means phone or store;
//                            UWP and STORE are both set for WINAPI_FAMILY_APP.
//   VX_PLATFORM_WINDOWS_SDK_VERSION
//                            Windows SDK build (VX_VERSION_NUMBER(0, 0, build)),
//                            or VX_VERSION_NUMBER_NOT_AVAILABLE when there is
//                            no ntverp.h (MinGW, WinCE, Wine).
//   VX_PLATFORM_UNKNOWN      no tag above was set. This is normal on plain
//                            Linux or macOS and is not an error.
//
// Notes:
//   - Mac Catalyst is an iOS build (see os.hpp) and is reported as
//     VX_PLATFORM_IOS_DEVICE.
//   - Xbox GDK (WINAPI_FAMILY_GAMES) has no tag.
//
// Define VX_PLATFORM_DISABLE_DETECTION before including this header to skip
// all detection (VX_PLATFORM_UNKNOWN is then defined).
//
// Requires from version_number.hpp:
//   VX_VERSION_NUMBER, VX_VERSION_NUMBER_AVAILABLE,
//   VX_VERSION_NUMBER_NOT_AVAILABLE

#include "vertex/config/os.hpp"
#include "vertex/config/version_number.hpp"

#if !defined(VX_PLATFORM_DISABLE_DETECTION)

//=============================================================================
// android
//=============================================================================

    #if defined(__ANDROID__)

        #define VX_PLATFORM_ANDROID VX_VERSION_NUMBER_AVAILABLE
        #define VX_PRIV_PLATFORM_DETECTED

    #endif // android

//=============================================================================
// ios, tvos, watchos, visionos (simulator vs. device)
//=============================================================================

// The OS macros come from os.hpp. TARGET_OS_SIMULATOR covers every Apple OS;
// TARGET_IPHONE_SIMULATOR is the legacy iOS-only form for old SDKs.

    #if defined(VX_OS_IOS) || defined(VX_OS_TVOS) || defined(VX_OS_WATCHOS) || defined(VX_OS_VISIONOS)

        #include <TargetConditionals.h>

        #if ((defined(TARGET_OS_SIMULATOR) && (TARGET_OS_SIMULATOR == 1)) || \
            (defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR == 1)))

            #define VX_PRIV_APPLE_SIMULATOR

        #endif

        #if defined(VX_OS_IOS)

            #if defined(VX_PRIV_APPLE_SIMULATOR)
                #define VX_PLATFORM_IOS_SIMULATOR VX_VERSION_NUMBER_AVAILABLE
            #else
                #define VX_PLATFORM_IOS_DEVICE VX_VERSION_NUMBER_AVAILABLE
            #endif

        #elif defined(VX_OS_TVOS)

            #if defined(VX_PRIV_APPLE_SIMULATOR)
                #define VX_PLATFORM_TVOS_SIMULATOR VX_VERSION_NUMBER_AVAILABLE
            #else
                #define VX_PLATFORM_TVOS_DEVICE VX_VERSION_NUMBER_AVAILABLE
            #endif

        #elif defined(VX_OS_WATCHOS)

            #if defined(VX_PRIV_APPLE_SIMULATOR)
                #define VX_PLATFORM_WATCHOS_SIMULATOR VX_VERSION_NUMBER_AVAILABLE
            #else
                #define VX_PLATFORM_WATCHOS_DEVICE VX_VERSION_NUMBER_AVAILABLE
            #endif

        #else // visionOS

            #if defined(VX_PRIV_APPLE_SIMULATOR)
                #define VX_PLATFORM_VISIONOS_SIMULATOR VX_VERSION_NUMBER_AVAILABLE
            #else
                #define VX_PLATFORM_VISIONOS_DEVICE VX_VERSION_NUMBER_AVAILABLE
            #endif

        #endif

        #undef VX_PRIV_APPLE_SIMULATOR
        #define VX_PRIV_PLATFORM_DETECTED

    #endif // apple mobile

//=============================================================================
// mingw
//=============================================================================

// mingw-w64 defines __MINGW32__ on both 32- and 64-bit targets, and also
// __MINGW64__ on 64-bit targets. __MINGW64__ therefore takes priority.
// VX_PLATFORM_MINGW is the general tag covering both variants.

    #if defined(__MINGW32__) || defined(__MINGW64__)

        #include <_mingw.h>

        #define VX_PLATFORM_MINGW VX_VERSION_NUMBER_AVAILABLE

        #if defined(__MINGW64__)

            #if defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR)
                #define VX_PLATFORM_MINGW64 VX_VERSION_NUMBER(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
            #else
                #define VX_PLATFORM_MINGW64 VX_VERSION_NUMBER_AVAILABLE
            #endif

        #else // __MINGW32__ only

            #if defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR)
                #define VX_PLATFORM_MINGW32 VX_VERSION_NUMBER(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
            #else
                #define VX_PLATFORM_MINGW32 VX_VERSION_NUMBER_AVAILABLE
            #endif

        #endif

        #define VX_PRIV_PLATFORM_DETECTED

    #endif // mingw

//=============================================================================
// windows
//=============================================================================

    #if defined(VX_OS_WINDOWS)

    // Windows SDK version

        // MinGW (32-bit), WinCE, and wineg++ don't have a ntverp.h header
        #if !defined(__MINGW32__) && !defined(_WIN32_WCE) && !defined(__WINE__)

            #include <ntverp.h>
            #define VX_PLATFORM_WINDOWS_SDK_VERSION VX_VERSION_NUMBER(0, 0, VER_PRODUCTBUILD)

        #else

            #define VX_PLATFORM_WINDOWS_SDK_VERSION VX_VERSION_NUMBER_NOT_AVAILABLE

        #endif

    // WINAPI_FAMILY support

        // 9200 is Windows SDK 8.0 from ntverp.h, which introduced family support
        #if ((VX_PLATFORM_WINDOWS_SDK_VERSION >= VX_VERSION_NUMBER(0, 0, 9200)) || \
            (defined(__MINGW64__) && defined(__MINGW64_VERSION_MAJOR) && (__MINGW64_VERSION_MAJOR >= 3)))

            #include <winapifamily.h>
            #define VX_PRIV_WINAPI_FAMILY_SUPPORT

        #endif

    // UWP

        #if defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && defined(WINAPI_FAMILY_APP) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)

            #define VX_PLATFORM_WINDOWS_UWP VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // uwp

    // Desktop

        #if (defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && defined(WINAPI_FAMILY_DESKTOP_APP) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)) || \
            !defined(VX_PRIV_WINAPI_FAMILY_SUPPORT)

            #define VX_PLATFORM_WINDOWS_DESKTOP VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // desktop

    // Phone

        #if defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && defined(WINAPI_FAMILY_PHONE_APP) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)

            #define VX_PLATFORM_WINDOWS_PHONE VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // phone

    // System

        #if defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && defined(WINAPI_FAMILY_SYSTEM) && (WINAPI_FAMILY == WINAPI_FAMILY_SYSTEM)

            #define VX_PLATFORM_WINDOWS_SYSTEM VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // system

    // Server

        #if defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && defined(WINAPI_FAMILY_SERVER) && (WINAPI_FAMILY == WINAPI_FAMILY_SERVER)

            #define VX_PLATFORM_WINDOWS_SERVER VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // server

    // Store

        #if defined(VX_PRIV_WINAPI_FAMILY_SUPPORT) && \
            ((defined(WINAPI_FAMILY_PC_APP) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)) || \
                (defined(WINAPI_FAMILY_APP) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)))

            #define VX_PLATFORM_WINDOWS_STORE VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // store

    // Runtime (phone or store)

        #if defined(VX_PLATFORM_WINDOWS_PHONE) || defined(VX_PLATFORM_WINDOWS_STORE)

            #define VX_PLATFORM_WINDOWS_RUNTIME VX_VERSION_NUMBER_AVAILABLE
            #define VX_PRIV_PLATFORM_DETECTED

        #endif // runtime

    // Cleanup

        #undef VX_PRIV_WINAPI_FAMILY_SUPPORT

    #endif // windows

#endif // !VX_PLATFORM_DISABLE_DETECTION

//=============================================================================
// unknown
//=============================================================================

#if !defined(VX_PRIV_PLATFORM_DETECTED)
    #define VX_PLATFORM_UNKNOWN VX_VERSION_NUMBER_AVAILABLE
#endif

//=============================================================================
// cleanup
//=============================================================================

#undef VX_PRIV_PLATFORM_DETECTED
