#pragma once

#include "image.h"

namespace vx {
namespace img {

inline void create_xor_mask(const image& img, std::vector<uint8_t>& mask)
{
    size_t mask_width = ((img.width() + 15) & ~15) / 8;
    size_t mask_height = img.height();

    mask.clear();
    mask.resize(mask_width * mask_height, 0xFF);

    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            if (img.get_pixel(x, y).a != 0)
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