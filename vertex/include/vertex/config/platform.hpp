#pragma once

#include "vertex/config/version_number.hpp"
#include "vertex/config/os.hpp"

// https://github.com/VXorg/predef/tree/e1211a4ca467bb6512e99025772ca25afa8d6159/include/VX/predef/platform

#if !defined(VX_PLATFORM_DUMMY)

///////////////////////////////////////////////////////////////////////////////
// android
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/android.h

#if defined(__ANDROID__)

#   define VX_PLATFORM_ANDROID VX_VERSION_NUMBER_AVAILABLE
#   define _VX_PLATFORM_DETECTED

#endif // android

///////////////////////////////////////////////////////////////////////////////
// ios
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/ios.h

#if defined(VX_OS_IOS)

// Detect version

#   include <TargetConditionals.h>

#   if (defined(TARGET_OS_SIMULATOR) && (TARGET_OS_SIMULATOR == 1) || \
        defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR == 1))

#       define VX_PLATFORM_IOS_SIMULATOR VX_VERSION_NUMBER_AVAILABLE

#   else

#       define VX_PLATFORM_IOS_DEVICE VX_VERSION_NUMBER_AVAILABLE

#   endif

// Cleanup

#   define _VX_PLATFORM_DETECTED

#endif // ios

///////////////////////////////////////////////////////////////////////////////
// mingw
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/mingw.h
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/mingw32.h
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/mingw64.h

#if defined(__MINGW32__) || defined(__MINGW64__)

#   include <_mingw.h>

// MINGW32

#   if defined(__MINGW32__)

#       if defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR)
#           define VX_PLATFORM_MINGW32_VERSION VX_VERSION_NUMBER(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#       else
#           define VX_PLATFORM_MINGW32_VERSION VX_VERSION_NUMBER_AVAILABLE
#       endif

#       if defined(_VX_PLATFORM_DETECTED)
#           define VX_PLATFORM_MINGW32_EMULATED VX_PLATFORM_MINGW32_VERSION
#       else
#           define VX_PLATFORM_MINGW32 VX_PLATFORM_MINGW32_VERSION
#           define _VX_PLATFORM_DETECTED
#       endif

#       undef VX_PLATFORM_MINGW32_VERSION

#   endif // __MINGW32__

// MINGW64

#   if defined(__MINGW64__)

#       if defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR)
#           define VX_PLATFORM_MINGW64_VERSION VX_VERSION_NUMBER(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
#       else
#           define VX_PLATFORM_MINGW64_VERSION VX_VERSION_NUMBER_AVAILABLE
#       endif

#       if defined(_VX_PLATFORM_DETECTED)
#           define VX_PLATFORM_MINGW64_EMULATED VX_PLATFORM_MINGW64_VERSION
#       else
#           define VX_PLATFORM_MINGW64 VX_PLATFORM_MINGW64_VERSION
#           define _VX_PLATFORM_DETECTED
#       endif

#       undef VX_PLATFORM_MINGW64_VERSION

#   endif // __MINGW64__

// General MinGW tag (used for grouping both variants)

#   if defined(_VX_PLATFORM_DETECTED)
#       define VX_PLATFORM_MINGW_EMULATED _VX_COMPILER_MSVC_DETECTION
#   else
#       define VX_PLATFORM_MINGW _VX_COMPILER_MSVC_DETECTION
#       define _VX_PLATFORM_DETECTED
#   endif

#endif // mingw

///////////////////////////////////////////////////////////////////////////////
// windows
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_OS_WINDOWS)

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_uwp.h

// MinGW (32-bit), WinCE, and wineg++ don't have a ntverp.h header
#   if !defined(__MINGW32__) && !defined(_WIN32_WCE) && !defined(__WINE__)

#       include <ntverp.h>
#       define VX_PLATFORM_WINDOWS_SDK_VERSION VX_VERSION_NUMBER(0, 0, VER_PRODUCTBUILD)

#   endif

// 9200 is Windows SDK 8.0 from ntverp.h which introduced family support
#   if ((VX_PLATFORM_WINDOWS_SDK_VERSION >= VX_VERSION_NUMBER(0, 0, 9200)) || \
        (defined(__MINGW64__) && __MINGW64_VERSION_MAJOR >= 3))

#       define VX_PLATFORM_WINDOWS_UWP VX_VERSION_NUMBER_AVAILABLE
#       include <winapifamily.h> // Windows SDK
#       define _VX_PLATFORM_DETECTED

#   endif

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_desktop.h

#   if ((defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) || !VX_PLATFORM_WINDOWS_UWP)

#       define VX_PLATFORM_WINDOWS_DESKTOP
#       define _VX_PLATFORM_DETECTED

#   endif // desktop

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_phone.h

#   if ((defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP))

#       define VX_PLATFORM_WINDOWS_PHONE
#       define _VX_PLATFORM_DETECTED

#   endif // phone

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_system.h

#   if ((defined(WINAPI_FAMILY_SYSTEM) && WINAPI_FAMILY == WINAPI_FAMILY_SYSTEM))

#       define VX_PLATFORM_WINDOWS_SYSTEM
#       define _VX_PLATFORM_DETECTED

#   endif // system

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_server.h

#   if ((defined(WINAPI_FAMILY_SERVER) && WINAPI_FAMILY == WINAPI_FAMILY_SERVER))

#       define VX_PLATFORM_WINDOWS_SERVER
#       define _VX_PLATFORM_DETECTED

#   endif // server

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_store.h

#   if ((defined(WINAPI_FAMILY_PC_APP) && WINAPI_FAMILY == WINAPI_FAMILY_PC_APP) || \
        (defined(WINAPI_FAMILY_APP)    && WINAPI_FAMILY == WINAPI_FAMILY_APP))

#       define VX_PLATFORM_WINDOWS_STORE
#       define _VX_PLATFORM_DETECTED

#   endif // store

////////////////////////////////////////
// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/platform/windows_runtime.h

#if (VX_PLATFORM_WINDOWS_PHONE || VX_PLATFORM_WINDOWS_STORE)

#       define VX_PLATFORM_WINDOWS_RUNTIME
#       define _VX_PLATFORM_DETECTED

#   endif // runtime 

////////////////////////////////////////

#endif // windows

#endif // VX_DUMMY_PLATFORM

///////////////////////////////////////////////////////////////////////////////
// dummy
///////////////////////////////////////////////////////////////////////////////

#if !defined(_VX_OS_DETECTED)
#   define VX_PLATFORM_DUMMY VX_VERSION_NUMBER_AVAILABLE
#   define _VX_PLATFORM_DETECTED
#endif

///////////////////////////////////////////////////////////////////////////////
// cleanup
///////////////////////////////////////////////////////////////////////////////

#if defined(_VX_PLATFORM_DETECTED)
#   undef _VX_PLATFORM_DETECTED
#endif