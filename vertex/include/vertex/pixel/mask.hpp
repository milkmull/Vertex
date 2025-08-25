#pragma once

#include <vector>

#include "vertex/pixel/surface.hpp"

namespace vx {
namespace pixel {

struct bitmask
{
    size_t width, height;
    std::vector<uint8_t> and_mask;
    std::vector<uint8_t> xor_mask;
};

template <pixel_format F>
inline void create_alpha_bitmask(
    const surface<F>& surf,
    std::vector<uint8_t>& mask,
    size_t& mask_width, size_t& mask_height
)
{
    mask_width = ((surf.width() + 15) & ~15) / 8;
    mask_height = surf.height();
    mask.resize(mask_width * mask_height, 0xFF);

    for (auto it = surf.begin(); it != surf.end(); ++it)
    {
        if (it.color().a != 0)
        {
            const size_t byte_index = it.y() * mask_width + (it.x() / 8);
            const size_t bit_position = 7 - (it.x() % 8);
            mask[byte_index] &= ~(1 << bit_position); // Set bit to 0 (opaque)
        }
    }
}

template <pixel_format F>
inline bitmask create_bitmask(
    size_t w, size_t h,
    const char* field,
    char black = 'X',
    char white = 'O',
    char invert = '*',
    char transparent = '.'
)
{
    mask_pair pair;
    pair.width = w;
    pair.height = h;

    if (w == 0 || h == 0 || !field)
    {
        return pair;
    }

    const size_t total_bits = w * h;
    const size_t total_bytes = (total_bits + 7) / 8;

    pair.and_mask.assign(total_bytes, 0);
    pair.xor_mask.assign(total_bytes, 0);

    size_t bit = 0;

    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            const char c = field[y * w + x];

            uint8_t data_bit;
            uint8_t mask_bit;

            if (c == black)
            {
                data_bit = 1;
                mask_bit = 1;
            }
            else if (c == white)
            {
                data_bit = 0;
                mask_bit = 1;
            }
            else if (c == invert)
            {
                data_bit = 1;
                mask_bit = 0;
            }
            else
            {
                // Unknown character -> treat as transparent
                data_bit = 0;
                mask_bit = 0;
            }

            if (data_bit)
            {
                pair.xor_mask[bit / 8] |= (1 << (bit % 8));
            }
            if (mask_bit)
            {
                pair.and_mask[bit / 8] |= (1 << (bit % 8));
            }

            ++bit;
        }
    }

    return pair;
}

} // namespace pixel
} // namespace vx