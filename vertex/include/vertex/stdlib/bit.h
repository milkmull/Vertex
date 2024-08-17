#pragma once

#include "vertex/system/compiler.h"

namespace vx {
namespace bit {

// Swap 16, 32 and 64 bits value for endianness

#if defined(VX_COMPILER_GNU)

#   define VX_BSWAP16(x) __builtin_bswap16(x)
#   define VX_BSWAP32(x) __builtin_bswap32(x)
#   define VX_BSWAP64(x) __builtin_bswap64(x)

#elif defined(VX_COMPILER_MSVC)

#   define VX_BSWAP16(x) _byteswap_ushort(x)
#   define VX_BSWAP32(x) _byteswap_ulong(x)
#   define VX_BSWAP64(x) _byteswap_uint64(x)

#else
    
    VX_FORCE_INLINE constexpr uint16_t _vx_byteswap_16(uint16_t x) noexcept
    {
        return (x >> 8) | (x << 8);
    }

    VX_FORCE_INLINE constexpr uint32_t _vx_byteswap_32(uint32_t x) noexcept
    {
        return ((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
    }

    VX_FORCE_INLINE constexpr uint64_t _vx_byteswap_64(uint64_t x) noexcept
    {
        x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
        x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
        x = (x & 0x00FF00FF00FF00FF) <<  8 | (x & 0xFF00FF00FF00FF00) >>  8;
        return x;
    }

#   define VX_BSWAP16(x) ::vx::bit::_vx_byteswap_16(x)
#   define VX_BSWAP32(x) ::vx::bit::_vx_byteswap_32(x)
#   define VX_BSWAP64(x) ::vx::bit::_vx_byteswap_64(x)

#endif

}
}