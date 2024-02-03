#pragma once

#include "../image_format.h"

namespace vx {
namespace img {
namespace detail {

// =============== format type traits ===============

template <image_format f>
struct format_traits_base
{
    static constexpr image_format format = f;
    static constexpr image_format format_8_bit = to_8_bit(f);

    static constexpr size_type channel_count = get_channel_count(f);
    static constexpr size_type bitdepth = get_bitdepth(f);
    static constexpr size_type pixel_size = get_pixel_size(f);

    static constexpr bool alpha = has_alpha(f);

};

template <image_format format>
struct format_traits;

template <>
struct format_traits<image_format::R8> : public format_traits_base<image_format::R8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RG8> : public format_traits_base<image_format::RG8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RGB8> : public format_traits_base<image_format::RGB8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RGBA8> : public format_traits_base<image_format::RGBA8>
{
    using channel_type = uint8_t;
};

}
}
}