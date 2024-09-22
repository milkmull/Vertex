#pragma once

///////////////////////////////////////////////////////////////////////////////
// Platform
///////////////////////////////////////////////////////////////////////////////

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_os_operating_system_macros.html

#if defined(linux) || defined(__linux) || defined(__linux__)

#   define VX_PLATFORM_LINUX

#elif defined(ANDROID) || defined(__ANDROID__)

#   define VX_PLATFORM_ANDROID

#elif defined(__unix__) || defined(__unix) || defined(unix)

#   define VX_PLATFORM_UNIX

#elif defined(__APPLE__)

#   define VX_PLATFORM_APPLE

#   include <AvailabilityMacros.h>
#   ifndef __has_extension
#       define __has_extension(x) 0
#       include <TargetConditionals.h>
#       undef __has_extension
#   else
#       include <TargetConditionals.h>
#   endif

#   ifndef TARGET_OS_MACCATALYST
#       define TARGET_OS_MACCATALYST 0
#   endif
#   ifndef TARGET_OS_IOS
#       define TARGET_OS_IOS 0
#   endif
#   ifndef TARGET_OS_IPHONE
#       define TARGET_OS_IPHONE 0
#   endif
#   ifndef TARGET_OS_VISION
#       define TARGET_OS_VISION 0
#   endif

#   if TARGET_OS_VISION
#       define VX_PLATFORM_VISIONOS
#   elif TARGET_OS_IPHONE
#       define VX_PLATFORM_IOS
#   else
#       define VX_PLATFORM_MACOS
#       if MAC_OS_X_VERSION_MIN_REQUIRED < 1070
#           error Vertex for macOS only supports deploying on 10.7 and above
#       endif
#   endif

#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)

#   define VX_PLATFORM_WINDOWS

// Need to add support for other types of windows platforms

#else

#   error Unsupported Platform

#endif

///////////////////////////////////////////////////////////////////////////////
// Architecture
///////////////////////////////////////////////////////////////////////////////

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_arch_architecture_macros.html

#if defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__)
#   define VX_ARCH_X86
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64)
#   define VX_ARCH_X64
#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB)
#   define VX_ARCH_ARM32
#elif defined(__aarch64__) || defined(_M_ARM64)
#   define VX_ARCH_ARM64
#else
#   error Unsupported Architecture
#endif

///////////////////////////////////////////////////////////////////////////////
// Endianness
///////////////////////////////////////////////////////////////////////////////

#define VX_LIL_ENDIAN 1234
#define VX_BIG_ENDIAN 4321

#if defined(VX_PLATFORM_LINUX)

#   include <endian.h>
#   if __BYTE_ORDER == __LITTLE_ENDIAN
#       define VX_BYTE_ORDER VX_LIL_ENDIAN
#   elif __BYTE_ORDER == __BIG_ENDIAN
#       define VX_BYTE_ORDER VX_BIG_ENDIAN
#   else
#       error Unsupported Endianness
#   endif

#elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)

#   if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#       define VX_BYTE_ORDER VX_LIL_ENDIAN
#   elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#       define VX_BYTE_ORDER VX_BIG_ENDIAN
#   else
#       error Unsupported Endianness
#   endif

#elif defined(VX_ARCH_HPPA) || defined(VX_ARCH_M68K) || defined(VX_ARCH_MIPS) || defined(VX_ARCH_POWERPC) || defined(VX_ARCH_SPARC)

#   define VX_BYTE_ORDER VX_BIG_ENDIAN

#else

#   define VX_BYTE_ORDER VX_LIL_ENDIAN

#endif