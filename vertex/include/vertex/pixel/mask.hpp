#pragma once

#include <vector>

#include "vertex/pixel/surface.hpp"

namespace vx {
namespace pixel {

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

    for (const auto& px : surf)
    {
        if (px.color().a != 0)
        {
            const size_t byte_index = y * mask_width + (x / 8);
            const size_t bit_position = 7 - (x % 8);
            mask[byte_index] &= ~(1 << bit_position); // Set bit to 0 (opaque)
        }
    }
}

} // namespace pixel
} // namespace vx