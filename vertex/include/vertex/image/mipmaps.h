#pragma once

#include "image.h"
#include "vertex/pixel/filter/filter_box.h"
#include "vertex/math/math/fn/fn_common.h"
#include "vertex/math/math/fn/fn_exponential.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates a series of mipmaps for the given image.
///
/// @param img The original image for which mipmaps are to be generated.
/// @param depth The maximum depth of mipmaps to generate. Default is -1,
/// which indicates generating mipmaps down to the smallest possible size based
/// on the dimensions of the original image.
///
/// @return A vector containing the generated mipmaps.
///////////////////////////////////////////////////////////////////////////////
inline std::vector<image> generate_mipmaps(const image& img, size_t depth = -1)
{
    if (img.empty())
    {
        return std::vector<image>();
    }

    size_t w = img.width();
    size_t h = img.height();

    size_t count = static_cast<size_t>(math::floor(math::log2(static_cast<float>(math::max(w, h)))));
    count = math::min(depth, count);

    std::vector<image> mipmaps(count);

    for (size_t i = 0; i < count; ++i)
    {
        w /= 2;
        h /= 2;

        image map(w, h, img.format());

        pixel::filter::filter_box<image::channel_type>(
            img.data(), img.width(), img.height(),
            map.data(), map.width(), map.height(),
            img.channels()
        );

        mipmaps[i] = map;
    }

    return mipmaps;
}

}
}