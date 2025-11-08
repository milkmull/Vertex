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

enum class error_code
{
    none = 0,
    unsupported_pixel_format,
    max_size,
    unsupported_channel_count,
    unsupported_bitdepth
};

inline const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::none:                          return "none";
        case error_code::unsupported_pixel_format:      return "unknown pixel format";
        case error_code::max_size:                      return "max image size";
        case error_code::unsupported_channel_count:     return "unsupported image channel count";
        case error_code::unsupported_bitdepth:          return "unsupported image bit depth";

        default:                                        break;
    }

    return nullptr;
}

inline error_code get_image_info_error(const image_info& info) noexcept
{
    if (info.format == pixel::pixel_format::unknown)
    {
        return error_code::unsupported_pixel_format;
    }

    if (info.width > max_dimensions || info.height > max_dimensions)
    {
        return error_code::max_size;
    }

    return error_code::none;
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

enum class scan_stage
{
    load = 0,
    type,
    header
};

} // namespace img
} // namespace vx