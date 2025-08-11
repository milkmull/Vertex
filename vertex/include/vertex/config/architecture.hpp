#pragma once

#include "vertex/config/version_number.hpp"

// https://github.com/VXorg/predef/tree/e1211a4ca467bb6512e99025772ca25afa8d6159/include/VX/predef/architecture
// https://sourceforge.net/p/predef/wiki/OperatingSystems/

///////////////////////////////////////////////////////////////////////////////
// arm
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/architecture/arm.h

#if defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || \
    defined(__TARGET_ARCH_THUMB) || defined(_M_ARM) || \
    defined(__arm__) || defined(__arm64) || defined(__thumb__) || \
    defined(_M_ARM64) || defined(__aarch64__) || defined(__AARCH64EL__) || \
    defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || \
    defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)

// Detect version

#   if defined(__ARM_ARCH)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(__ARM_ARCH, 0, 0)

#   elif defined(__TARGET_ARCH_ARM)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(__TARGET_ARCH_ARM, 0, 0)

#   elif defined(__TARGET_ARCH_THUMB)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(__TARGET_ARCH_THUMB, 0, 0)

#   elif defined(_M_ARM)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(_M_ARM, 0, 0)

#   elif defined(__arm64) || defined(_M_ARM64) || \
        defined(__aarch64__) || defined(__AARCH64EL__)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(8, 0, 0)

#   elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
        defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(7, 0, 0)

#   elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
        defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(6, 0, 0)

#   elif defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(5, 0, 0)

#   elif defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)

#       define VX_ARCH_ARM VX_VERSION_NUMBER(4, 0, 0)

#   else

#       define VX_ARCH_ARM VX_VERSION_AVAILABLE

#   endif

// Word size

#   if VX_ARCH_ARM >= VX_VERSION_NUMBER(8, 0, 0)
#       define VX_ARCH_WORD_BITS_64
#   else
#       define VX_ARCH_WORD_BITS_32
#   endif

#endif // VX_ARCH_ARM

///////////////////////////////////////////////////////////////////////////////
// x86
///////////////////////////////////////////////////////////////////////////////

// https://github.com/VXorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/VX/predef/architecture/x86/32.h
// https://github.com/VXorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/VX/predef/architecture/x86/64.h
// https://github.com/VXorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/VX/predef/architecture/x86.h

///////////////////////////////////////
// x86_32
///////////////////////////////////////

#if defined(i386) || defined(__i386__) || \
    defined(__i486__) || defined(__i586__) || \
    defined(__i686__) || defined(__i386) || \
    defined(_M_IX86) || defined(_X86_) || \
    defined(__THW_INTEL__) || defined(__I86__) || \
    defined(__INTEL__)

// Detect version

#   if defined(__I86__)

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER(__I86__, 0, 0)

#   elif defined(_M_IX86)

#       define VX_ARCH_X86_32 _VX_MAKE_VERSION_NUMBER_10_VV00(_M_IX86)

#   elif defined(__i686__)

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER(6, 0, 0)

#   elif defined(__i586__)

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER(5, 0, 0)

#   elif defined(__i486__)

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER(4, 0, 0)

#   elif defined(__i386__)

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER(3, 0, 0)

#   else

#       define VX_ARCH_X86_32 VX_VERSION_NUMBER_AVAILABLE

#   endif

// Word size

#   define VX_ARCH_WORD_BITS_32

#endif // VX_ARCH_X86_32

///////////////////////////////////////
// x86_64
///////////////////////////////////////

#if defined(__x86_64) || defined(__x86_64__) || \
    defined(__amd64__) || defined(__amd64) || \
    defined(_M_X64)

// Detect Version

#   define VX_ARCH_X86_64 VX_VERSION_NUMBER_AVAILABLE

// Word size

#   define VX_ARCH_WORD_BITS_64

#endif // VX_ARCH_X86_64

///////////////////////////////////////
// x86
///////////////////////////////////////

#if defined(VX_ARCH_X86_32) || defined(VX_ARCH_X86_64)

#   define VX_ARCH_X86 VX_VERSION_NUMBER_AVAILABLE

#endif