#pragma once

//=============================================================================
// endianness
//=============================================================================

#define VX_ORDER_BIG_ENDIAN    0
#define VX_ORDER_LITTLE_ENDIAN 1

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__

    #error Vertex does not support platforms with PDP endianness

#elif defined(__LITTLE_ENDIAN__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#elif defined(__BIG_ENDIAN__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

#elif defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#else

    #error Vertex could not determine the endianness of this platform.

#endif
