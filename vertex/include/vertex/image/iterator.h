#pragma once

#include "image_format.h"
#include "vertex/pixel/pixel_iterator.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// types
///////////////////////////////////////////////////////////////////////////////

using image_iterator_r8 = pixel::pixel_iterator<pixel::pixel_r_8>;
using image_iterator_rg8 = pixel::pixel_iterator<pixel::pixel_rg_8>;
using image_iterator_rgb8 = pixel::pixel_iterator<pixel::pixel_rgb_8>;
using image_iterator_rgba8 = pixel::pixel_iterator<pixel::pixel_rgba_8>;

///////////////////////////////////////////////////////////////////////////////
// iterators
///////////////////////////////////////////////////////////////////////////////

class image;

template <image_pixel_format f>
inline constexpr auto begin(image& i) noexcept
{
    return pixel::iter::begin<static_cast<pixel::pixel_format>(f)>((void*)i.data(), i.width(), i.height());
}

template <image_pixel_format f>
inline auto cbegin(const image& i) noexcept
{
    return pixel::iter::cbegin<static_cast<pixel::pixel_format>(f)>((void*)i.data(), i.width(), i.height());
}

template <image_pixel_format f>
inline auto begin(const image& i) noexcept
{
    return cbegin<f>(i);
}

template <image_pixel_format f>
inline constexpr auto end(image& i) noexcept
{
    return pixel::iter::end<static_cast<pixel::pixel_format>(f)>((void*)i.data(), i.width(), i.height());
}

template <image_pixel_format f>
inline auto cend(const image& i) noexcept
{
    return pixel::iter::cend<static_cast<pixel::pixel_format>(f)>((void*)i.data(), i.width(), i.height());
}

template <image_pixel_format f>
inline auto end(const image& i) noexcept
{
    return cend<f>(i);
}

}
}