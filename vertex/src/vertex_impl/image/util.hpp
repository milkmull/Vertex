#pragma once

#include "vertex/image/defs.hpp"
#include "vertex/system/error.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {
namespace img {

struct image_info
{
    size_t width, height;
    pixel::pixel_format format;
};

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

enum class error_code : int
{
    NONE = 0,
    UNSUPPORTED_PIXEL_FORMAT,
    MAX_SIZE,
    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH
};

inline const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::NONE:                          return "none";
        case error_code::UNSUPPORTED_PIXEL_FORMAT:      return "unknown pixel format";
        case error_code::MAX_SIZE:                      return "max image size";
        case error_code::UNSUPPORTED_CHANNEL_COUNT:     return "unsupported image channel count";
        case error_code::UNSUPPORTED_BITDEPTH:          return "unsupported image bit depth";

        default:                                        break;
    }

    return nullptr;
}

inline error_code get_image_info_error(const image_info& info) noexcept
{
    if (info.format == pixel::pixel_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_PIXEL_FORMAT;
    }

    if (info.width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }

    return error_code::NONE;
}

///////////////////////////////////////////////////////////////////////////////
// overflow helpers
///////////////////////////////////////////////////////////////////////////////

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE constexpr bool is_safe_add(T a, T b) noexcept
{
    VX_IF_CONSTEXPR(std::is_unsigned<T>::value)
    {
        return a <= std::numeric_limits<T>::max() - b;
    }
    else
    {
        if ((a >= 0) != (b >= 0))
        {
            return true; // opposite signs, always safe
        }

        if (a < 0)
        {
            return a >= std::numeric_limits<T>::min() - b;
        }
        else
        {
            return a <= std::numeric_limits<T>::max() - b;
        }
    }
}

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE constexpr bool is_safe_mul(T a, T b) noexcept
{
    VX_IF_CONSTEXPR(std::is_unsigned<T>::value)
    {
        if (b == 0) return true;
        return a <= std::numeric_limits<T>::max() / b;
    }
    else
    {
        if (a < 0 || b < 0) return false;
        if (b == 0) return true;
        return a <= std::numeric_limits<T>::max() / b;
    }
}

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE constexpr bool is_safe_mad(T a, T b, T add) noexcept
{
    return is_safe_mul(a, b) && is_safe_add(a * b, add);
}

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE constexpr bool is_safe_mad(T a, T b, T c, T add) noexcept
{
    return is_safe_mul(a, b) && is_safe_mul(a * b, c) && is_safe_add(a * b * c, add);
}

///////////////////////////////////////////////////////////////////////////////
// safe allocation
///////////////////////////////////////////////////////////////////////////////

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE void* safe_malloc_mad(T a, T b, T add) noexcept
{
    return is_safe_mad(a, b, add) ? std::malloc(a * b + add) : nullptr;
}

template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
VX_FORCE_INLINE void* safe_malloc_mad(T a, T b, T c, T add) noexcept
{
    return is_safe_mad(a, b, c, add) ? std::malloc(a * b * c + add) : nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// scan stage
///////////////////////////////////////////////////////////////////////////////

enum scan_stage
{
    scan_stage_load = 0,
    scan_stage_type,
    scan_stage_header
};

} // namespace img
} // namespace vx