#pragma once

///////////////////////////////////////////////////////////////////////////////
// Platform
///////////////////////////////////////////////////////////////////////////////

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_os_operating_system_macros.html
// https://sourceforge.net/p/predef/wiki/OperatingSystems/

#if defined(linux) || defined(__linux) || defined(__linux__)

#   define VX_PLATFORM_LINUX
#   define VX_PLATFORM_DEFINED

#endif // linux

#if defined(ANDROID) || defined(__ANDROID__)

#   define VX_PLATFORM_ANDROID
#   undef VX_PLATFORM_LINUX
#   define VX_PLATFORM_DEFINED

#endif // android

#if defined(__unix__) || defined(__unix) || defined(unix)

#   define VX_PLATFORM_UNIX
#   define VX_PLATFORM_DEFINED

#endif // unix

#if defined(__APPLE__)

#   define VX_PLATFORM_APPLE

#   include <TargetConditionals.h>

#   if TARGET_OS_IOS || TARGET_OS_IPHONE

#       define VX_PLATFORM_IOS

#   elif TARGET_OS_MAC

#       define VX_PLATFORM_MACOS
#       if MAC_OS_X_VERSION_MIN_REQUIRED <070
#           error Vertex for macOS only supports deploying on0.7 and above
#       endif

#   else
#       error Unsupported Apple platform
#   endif

#   define VX_PLATFORM_DEFINED

#endif // apple

#if defined(_WIN32)

#   define VX_PLATFORM_WINDOWS

#   if defined(__CYGWIN__)
#       define VX_PLATFORM_CYGWIN
#   endif

#   define VX_PLATFORM_DEFINED

#endif // windows

#if defined(VX_PLATFORM_LINUX) || defined(VX_PLATFORM_ANDROID) || defined(VX_PLATFORM_UNIX) || defined(VX_PLATFORM_APPLE)

#   define VX_PLATFORM_POSIX

#endif // posix

#if !defined(VX_PLATFORM_DEFINED)
#   error Unsupported Platform
#else
#   undef VX_PLATFORM_DEFINED
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