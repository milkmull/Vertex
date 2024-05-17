#pragma once

#include "pixel.h"
#include "pixel_type_info.h"

#include "vertex/math/color/type/color.h"

namespace vx {
namespace pixel {

namespace _priv {

template <typename I, typename F = float>
VX_FORCE_INLINE constexpr I float_to_packed(F x, I r)
{
    return static_cast<I>(math::clamp(static_cast<F>(x) * static_cast<F>(r), static_cast<F>(0), static_cast<F>(r)));
}

template <typename I, typename F = float>
VX_FORCE_INLINE constexpr F packed_to_float(I x, I r)
{
    return static_cast<F>(x) / static_cast<F>(r);
}

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////
// pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct pixel;

template <pixel_format f, bool alpha> struct packed_pixel;
template <pixel_format f, size_t channels> struct array_pixel;
template <pixel_format f, size_t channels> struct norm_array_pixel;

///////////////////////////////////////////////////////////////////////////////
// packed pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f>
struct packed_pixel<f, false>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr packed_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr packed_pixel(const math::color& c)
        : data((_priv::float_to_packed<channel_type>(c.r, info.channels.mask.r >> info.channels.shift.r) << info.channels.shift.r) |
               (_priv::float_to_packed<channel_type>(c.g, info.channels.mask.g >> info.channels.shift.g) << info.channels.shift.g) |
               (_priv::float_to_packed<channel_type>(c.b, info.channels.mask.b >> info.channels.shift.b) << info.channels.shift.b)) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            _priv::packed_to_float<channel_type>((data & info.channels.mask.r) >> info.channels.shift.r, info.channels.mask.r >> info.channels.shift.r),
            _priv::packed_to_float<channel_type>((data & info.channels.mask.g) >> info.channels.shift.g, info.channels.mask.g >> info.channels.shift.g),
            _priv::packed_to_float<channel_type>((data & info.channels.mask.b) >> info.channels.shift.b, info.channels.mask.b >> info.channels.shift.b),
            1.0f
        );
    }
};

template <pixel_format f>
struct packed_pixel<f, true>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr packed_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr packed_pixel(const math::color& c)
        : data((_priv::float_to_packed<channel_type>(c.r, info.channels.mask.r >> info.channels.shift.r) << info.channels.shift.r) |
               (_priv::float_to_packed<channel_type>(c.g, info.channels.mask.g >> info.channels.shift.g) << info.channels.shift.g) |
               (_priv::float_to_packed<channel_type>(c.b, info.channels.mask.b >> info.channels.shift.b) << info.channels.shift.b) |
               (_priv::float_to_packed<channel_type>(c.a, info.channels.mask.a >> info.channels.shift.a) << info.channels.shift.a)) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            _priv::packed_to_float<channel_type>((data & info.channels.mask.r) >> info.channels.shift.r, info.channels.mask.r >> info.channels.shift.r),
            _priv::packed_to_float<channel_type>((data & info.channels.mask.g) >> info.channels.shift.g, info.channels.mask.g >> info.channels.shift.g),
            _priv::packed_to_float<channel_type>((data & info.channels.mask.b) >> info.channels.shift.b, info.channels.mask.b >> info.channels.shift.b),
            _priv::packed_to_float<channel_type>((data & info.channels.mask.a) >> info.channels.shift.a, info.channels.mask.a >> info.channels.shift.a)
        );
    }
};

///////////////////////////////////////////////////////////////////////////////
// int array pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f>
struct array_pixel<f, 1>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = _priv::float_to_packed<channel_type, float_type>(c.r, ~static_cast<channel_type>(0));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.r], ~static_cast<channel_type>(0))),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

template <pixel_format f>
struct array_pixel<f, 2>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = _priv::float_to_packed<channel_type, float_type>(c.r, ~static_cast<channel_type>(0));
        data[info.channels.mask.g] = _priv::float_to_packed<channel_type, float_type>(c.g, ~static_cast<channel_type>(0));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.r], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.g], ~static_cast<channel_type>(0))),
            0.0f,
            1.0f
        );
    }
};

template <pixel_format f>
struct array_pixel<f, 3>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = _priv::float_to_packed<channel_type, float_type>(c.r, ~static_cast<channel_type>(0));
        data[info.channels.mask.g] = _priv::float_to_packed<channel_type, float_type>(c.g, ~static_cast<channel_type>(0));
        data[info.channels.mask.b] = _priv::float_to_packed<channel_type, float_type>(c.b, ~static_cast<channel_type>(0));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.r], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.g], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.b], ~static_cast<channel_type>(0))),
            1.0f
        );
    }
};

template <pixel_format f>
struct array_pixel<f, 4>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = _priv::float_to_packed<channel_type, float_type>(c.r, ~static_cast<channel_type>(0));
        data[info.channels.mask.g] = _priv::float_to_packed<channel_type, float_type>(c.g, ~static_cast<channel_type>(0));
        data[info.channels.mask.b] = _priv::float_to_packed<channel_type, float_type>(c.b, ~static_cast<channel_type>(0));
        data[info.channels.mask.a] = _priv::float_to_packed<channel_type, float_type>(c.a, ~static_cast<channel_type>(0));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.r], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.g], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.b], ~static_cast<channel_type>(0))),
            static_cast<float>(_priv::packed_to_float<channel_type, float_type>(data[info.channels.mask.a], ~static_cast<channel_type>(0)))
        );
    }
};

///////////////////////////////////////////////////////////////////////////////
// normalized array pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f>
struct norm_array_pixel<f, 1>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr norm_array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr norm_array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = static_cast<channel_type>(c.r);
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(data[info.channels.mask.r]),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

template <pixel_format f>
struct norm_array_pixel<f, 2>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr norm_array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr norm_array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = static_cast<channel_type>(c.r);
        data[info.channels.mask.g] = static_cast<channel_type>(c.g);
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(data[info.channels.mask.r]),
            static_cast<float>(data[info.channels.mask.g]),
            0.0f,
            1.0f
        );
    }
};

template <pixel_format f>
struct norm_array_pixel<f, 3>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr norm_array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr norm_array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = static_cast<channel_type>(c.r);
        data[info.channels.mask.g] = static_cast<channel_type>(c.g);
        data[info.channels.mask.b] = static_cast<channel_type>(c.b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(data[info.channels.mask.r]),
            static_cast<float>(data[info.channels.mask.g]),
            static_cast<float>(data[info.channels.mask.b]),
            1.0f
        );
    }
};

template <pixel_format f>
struct norm_array_pixel<f, 4>
{
    using pixel_type = typename pixel_type_info<f>::pixel_type;
    using channel_type = typename pixel_type_info<f>::channel_type;
    using float_type = typename pixel_type_info<f>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(f);
    pixel_type data;

    VX_FORCE_INLINE constexpr norm_array_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr norm_array_pixel(const math::color& c) : data{ 0 }
    {
        data[info.channels.mask.r] = static_cast<channel_type>(c.r);
        data[info.channels.mask.g] = static_cast<channel_type>(c.g);
        data[info.channels.mask.b] = static_cast<channel_type>(c.b);
        data[info.channels.mask.a] = static_cast<channel_type>(c.a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            static_cast<float>(data[info.channels.mask.r]),
            static_cast<float>(data[info.channels.mask.g]),
            static_cast<float>(data[info.channels.mask.b]),
            static_cast<float>(data[info.channels.mask.a])
        );
    }
};

///////////////////////////////////////////////////////////////////////////////
// unknown
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_UNKNOWN>
{
    using pixel_type = typename pixel_type_info<pixel_format::PIXEL_FORMAT_UNKNOWN>::pixel_type;
    using channel_type = typename pixel_type_info<pixel_format::PIXEL_FORMAT_UNKNOWN>::channel_type;
    using float_type = typename pixel_type_info<pixel_format::PIXEL_FORMAT_UNKNOWN>::float_type;

    static constexpr pixel_format_info info = get_pixel_format_info(pixel_format::PIXEL_FORMAT_UNKNOWN);

    VX_FORCE_INLINE constexpr pixel() {}
    VX_FORCE_INLINE constexpr pixel(const math::color& c) {}
    VX_FORCE_INLINE constexpr operator math::color() const { return math::color(); }
};

///////////////////////////////////////////////////////////////////////////////
// 332
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_332> : public packed_pixel<pixel_format::PIXEL_FORMAT_RGB_332, false>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_rgb_332 = pixel<pixel_format::PIXEL_FORMAT_RGB_332>;
static_assert(sizeof(pixel_rgb_332) == sizeof(pixel_rgb_332::data));

///////////////////////////////////////////////////////////////////////////////
// 4444
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_4444> : public packed_pixel<pixel_format::PIXEL_FORMAT_RGBA_4444, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_rgba_4444 = pixel<pixel_format::PIXEL_FORMAT_RGBA_4444>;
static_assert(sizeof(pixel_rgba_4444) == sizeof(pixel_rgba_4444::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGRA_4444> : public packed_pixel<pixel_format::PIXEL_FORMAT_BGRA_4444, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_bgra_4444 = pixel<pixel_format::PIXEL_FORMAT_BGRA_4444>;
static_assert(sizeof(pixel_bgra_4444) == sizeof(pixel_bgra_4444::data));

///////////////////////////////////////////////////////////////////////////////
// 565
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_565> : public packed_pixel<pixel_format::PIXEL_FORMAT_RGB_565, false>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_rgb_565 = pixel<pixel_format::PIXEL_FORMAT_RGB_565>;
static_assert(sizeof(pixel_rgb_565) == sizeof(pixel_rgb_565::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGR_565> : public packed_pixel<pixel_format::PIXEL_FORMAT_BGR_565, false>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_bgr_565 = pixel<pixel_format::PIXEL_FORMAT_BGR_565>;
static_assert(sizeof(pixel_bgr_565) == sizeof(pixel_bgr_565::data));

///////////////////////////////////////////////////////////////////////////////
// 5551 & 1555
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_5551> : public packed_pixel<pixel_format::PIXEL_FORMAT_RGBA_5551, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_rgba_5551 = pixel<pixel_format::PIXEL_FORMAT_RGBA_5551>;
static_assert(sizeof(pixel_rgba_5551) == sizeof(pixel_rgba_5551::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGRA_5551> : public packed_pixel<pixel_format::PIXEL_FORMAT_BGRA_5551, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_bgra_5551 = pixel<pixel_format::PIXEL_FORMAT_BGRA_5551>;
static_assert(sizeof(pixel_bgra_5551) == sizeof(pixel_bgra_5551::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_ARGB_1555> : public packed_pixel<pixel_format::PIXEL_FORMAT_ARGB_1555, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_argb_1555 = pixel<pixel_format::PIXEL_FORMAT_ARGB_1555>;
static_assert(sizeof(pixel_argb_1555) == sizeof(pixel_argb_1555::data));

///////////////////////////////////////////////////////////////////////////////
// packed 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_8888> : public packed_pixel<pixel_format::PIXEL_FORMAT_RGBA_8888, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_rgba_8888 = pixel<pixel_format::PIXEL_FORMAT_RGBA_8888>;
static_assert(sizeof(pixel_rgba_8888) == sizeof(pixel_rgba_8888::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGRA_8888> : public packed_pixel<pixel_format::PIXEL_FORMAT_BGRA_8888, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_bgra_8888 = pixel<pixel_format::PIXEL_FORMAT_BGRA_8888>;
static_assert(sizeof(pixel_bgra_8888) == sizeof(pixel_bgra_8888::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_ABGR_8888> : public packed_pixel<pixel_format::PIXEL_FORMAT_ABGR_8888, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_abgr_8888 = pixel<pixel_format::PIXEL_FORMAT_ABGR_8888>;
static_assert(sizeof(pixel_abgr_8888) == sizeof(pixel_abgr_8888::data));

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_R_8> : public array_pixel<pixel_format::PIXEL_FORMAT_R_8, 1>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_r_8 = pixel<pixel_format::PIXEL_FORMAT_R_8>;
static_assert(sizeof(pixel_r_8) == sizeof(pixel_r_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RG_8> : public array_pixel<pixel_format::PIXEL_FORMAT_RG_8, 2>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rg_8 = pixel<pixel_format::PIXEL_FORMAT_RG_8>;
static_assert(sizeof(pixel_rg_8) == sizeof(pixel_rg_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_8> : public array_pixel<pixel_format::PIXEL_FORMAT_RGB_8, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgb_8 = pixel<pixel_format::PIXEL_FORMAT_RGB_8>;
static_assert(sizeof(pixel_rgb_8) == sizeof(pixel_rgb_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_8> : public array_pixel<pixel_format::PIXEL_FORMAT_RGBA_8, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgba_8 = pixel<pixel_format::PIXEL_FORMAT_RGBA_8>;
static_assert(sizeof(pixel_rgba_8) == sizeof(pixel_rgba_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGR_8> : public array_pixel<pixel_format::PIXEL_FORMAT_BGR_8, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_bgr_8 = pixel<pixel_format::PIXEL_FORMAT_BGR_8>;
static_assert(sizeof(pixel_bgr_8) == sizeof(pixel_bgr_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_BGRA_8> : public array_pixel<pixel_format::PIXEL_FORMAT_BGRA_8, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_bgra_8 = pixel<pixel_format::PIXEL_FORMAT_BGRA_8>;
static_assert(sizeof(pixel_bgra_8) == sizeof(pixel_bgra_8::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_ABGR_8> : public array_pixel<pixel_format::PIXEL_FORMAT_ABGR_8, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_abgr_8 = pixel<pixel_format::PIXEL_FORMAT_ABGR_8>;
static_assert(sizeof(pixel_abgr_8) == sizeof(pixel_abgr_8::data));

///////////////////////////////////////////////////////////////////////////////
// 2101010
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_ARGB_2101010> : public packed_pixel<pixel_format::PIXEL_FORMAT_ARGB_2101010, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_argb_2101010 = pixel<pixel_format::PIXEL_FORMAT_ARGB_2101010>;
static_assert(sizeof(pixel_argb_2101010) == sizeof(pixel_argb_2101010::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_ABGR_2101010> : public packed_pixel<pixel_format::PIXEL_FORMAT_ABGR_2101010, true>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : packed_pixel(c) {}
};

using pixel_abgr_2101010 = pixel<pixel_format::PIXEL_FORMAT_ABGR_2101010>;
static_assert(sizeof(pixel_abgr_2101010) == sizeof(pixel_abgr_2101010::data));

///////////////////////////////////////////////////////////////////////////////
// 16 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_R_16> : public array_pixel<pixel_format::PIXEL_FORMAT_R_16, 1>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_r_16 = pixel<pixel_format::PIXEL_FORMAT_R_16>;
static_assert(sizeof(pixel_r_16) == sizeof(pixel_r_16::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RG_16> : public array_pixel<pixel_format::PIXEL_FORMAT_RG_16, 2>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rg_16 = pixel<pixel_format::PIXEL_FORMAT_RG_16>;
static_assert(sizeof(pixel_rg_16) == sizeof(pixel_rg_16::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_16> : public array_pixel<pixel_format::PIXEL_FORMAT_RGB_16, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgb_16 = pixel<pixel_format::PIXEL_FORMAT_RGB_16>;
static_assert(sizeof(pixel_rgb_16) == sizeof(pixel_rgb_16::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_16> : public array_pixel<pixel_format::PIXEL_FORMAT_RGBA_16, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgba_16 = pixel<pixel_format::PIXEL_FORMAT_RGBA_16>;
static_assert(sizeof(pixel_rgba_16) == sizeof(pixel_rgba_16::data));

///////////////////////////////////////////////////////////////////////////////
// 32 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_R_32> : public array_pixel<pixel_format::PIXEL_FORMAT_R_32, 1>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_r_32 = pixel<pixel_format::PIXEL_FORMAT_R_32>;
static_assert(sizeof(pixel_r_32) == sizeof(pixel_r_32::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RG_32> : public array_pixel<pixel_format::PIXEL_FORMAT_RG_32, 2>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rg_32 = pixel<pixel_format::PIXEL_FORMAT_RG_32>;
static_assert(sizeof(pixel_rg_32) == sizeof(pixel_rg_32::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_32> : public array_pixel<pixel_format::PIXEL_FORMAT_RGB_32, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgb_32 = pixel<pixel_format::PIXEL_FORMAT_RGB_32>;
static_assert(sizeof(pixel_rgb_32) == sizeof(pixel_rgb_32::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_32> : public array_pixel<pixel_format::PIXEL_FORMAT_RGBA_32, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgba_32 = pixel<pixel_format::PIXEL_FORMAT_RGBA_32>;
static_assert(sizeof(pixel_rgba_32) == sizeof(pixel_rgba_32::data));

///////////////////////////////////////////////////////////////////////////////
// 32 bit float
///////////////////////////////////////////////////////////////////////////////

static_assert(sizeof(float) == 4);

template <>
struct pixel<pixel_format::PIXEL_FORMAT_R_32F> : norm_array_pixel<pixel_format::PIXEL_FORMAT_R_32F, 1>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : norm_array_pixel(c) {}
};

using pixel_r_32f = pixel<pixel_format::PIXEL_FORMAT_R_32F>;
static_assert(sizeof(pixel_r_32f) == sizeof(pixel_r_32f::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RG_32F> : norm_array_pixel<pixel_format::PIXEL_FORMAT_RG_32F, 2>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : norm_array_pixel(c) {}
};

using pixel_rg_32f = pixel<pixel_format::PIXEL_FORMAT_RG_32F>;
static_assert(sizeof(pixel_rg_32f) == sizeof(pixel_rg_32f::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_32F> : norm_array_pixel<pixel_format::PIXEL_FORMAT_RGB_32F, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : norm_array_pixel(c) {}
};

using pixel_rgb_32f = pixel<pixel_format::PIXEL_FORMAT_RGB_32F>;
static_assert(sizeof(pixel_rgb_32f) == sizeof(pixel_rgb_32f::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_32F> : norm_array_pixel<pixel_format::PIXEL_FORMAT_RGBA_32F, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : norm_array_pixel(c) {}
};

using pixel_rgba_32f = pixel<pixel_format::PIXEL_FORMAT_RGBA_32F>;
static_assert(sizeof(pixel_rgba_32f) == sizeof(pixel_rgba_32f::data));

///////////////////////////////////////////////////////////////////////////////
// 64 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_R_64> : public array_pixel<pixel_format::PIXEL_FORMAT_R_64, 1>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_r_64 = pixel<pixel_format::PIXEL_FORMAT_R_64>;
static_assert(sizeof(pixel_r_64) == sizeof(pixel_r_64::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RG_64> : public array_pixel<pixel_format::PIXEL_FORMAT_RG_64, 2>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rg_64 = pixel<pixel_format::PIXEL_FORMAT_RG_64>;
static_assert(sizeof(pixel_rg_64) == sizeof(pixel_rg_64::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGB_64> : public array_pixel<pixel_format::PIXEL_FORMAT_RGB_64, 3>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgb_64 = pixel<pixel_format::PIXEL_FORMAT_RGB_64>;
static_assert(sizeof(pixel_rgb_64) == sizeof(pixel_rgb_64::data));

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel<pixel_format::PIXEL_FORMAT_RGBA_64> : public array_pixel<pixel_format::PIXEL_FORMAT_RGBA_64, 4>
{
    VX_FORCE_INLINE constexpr pixel() = default;
    VX_FORCE_INLINE constexpr pixel(const math::color& c) : array_pixel(c) {}
};

using pixel_rgba_64 = pixel<pixel_format::PIXEL_FORMAT_RGBA_64>;
static_assert(sizeof(pixel_rgba_64) == sizeof(pixel_rgba_64::data));

///////////////////////////////////////////////////////////////////////////////

}
}