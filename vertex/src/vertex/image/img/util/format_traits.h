#pragma once

#include "format.h"

namespace vx {
namespace img {
namespace util {

// =============== format type traits ===============

template <image_format format>
struct format_traits_base
{
    static constexpr image_format format = format;
    static constexpr image_format format_8_bit = to_8_bit(format);

    static constexpr size_type channel_count = get_channel_count(format);
    static constexpr size_type bitdepth = get_bitdepth(format);
    static constexpr size_type pixel_size = get_pixel_size(format);

    static constexpr bool alpha = has_alpha(format);

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