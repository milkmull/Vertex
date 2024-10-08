#pragma once

#include "surface.h"

namespace vx {
namespace pixel {

inline void create_alpha_bitmask(
    const surface& surf,
    std::vector<uint8_t>& mask,
    size_t& mask_width, size_t& mask_height
)
{
    mask_width = ((surf.width() + 15) & ~15) / 8;
    mask_height = surf.height();

    mask.clear();
    mask.resize(mask_width * mask_height, 0xFF);

    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            if (surf.get_pixel(x, y).a != 0)
            {
                size_t byte_index = y * mask_width + (x / 8);
                size_t bit_position = 7 - (x % 8);
                mask[byte_index] &= ~(1 << bit_position); // Set bit to 0 (opaque)
            }
        }
    }
}

}
}