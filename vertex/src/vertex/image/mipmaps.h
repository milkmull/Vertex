#pragma once

#include "image.h"
#include "raw/filter/filter_box.h"

namespace vx {
namespace img {

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

        raw::filter_box(
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