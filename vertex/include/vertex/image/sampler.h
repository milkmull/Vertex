#pragma once

#include "image_format.h"
#include "vertex/pixel/sampler.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// types
///////////////////////////////////////////////////////////////////////////////

using image_sampler_r_8 = pixel::sampler<pixel::pixel_format::PIXEL_FORMAT_R_8>;
using image_sampler_rg_8 = pixel::sampler<pixel::pixel_format::PIXEL_FORMAT_RG_8>;
using image_sampler_rgb_8 = pixel::sampler<pixel::pixel_format::PIXEL_FORMAT_RGB_8>;
using image_sampler_rgba_8 = pixel::sampler<pixel::pixel_format::PIXEL_FORMAT_RGBA_8>;

///////////////////////////////////////////////////////////////////////////////
// samplers
///////////////////////////////////////////////////////////////////////////////

class image;

template <image_pixel_format f>
inline auto create_sampler(const image& i)
{
    return pixel::sampler<static_cast<pixel::pixel_format>(f)>(i.data(), i.width(), i.height());
}

}
}