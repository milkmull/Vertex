#pragma once

#include <vector>

#include "vertex/pixel/surface.hpp"
#include "vertex/pixel/filter/filter_box.hpp"
#include "vertex/math/core/functions/common.hpp"
#include "vertex/math/core/functions/exponential.hpp"

namespace vx {
namespace pixel {

template <pixel_format F>
inline std::vector<surface<F>> generate_mipmaps(const surface<F>& surf, size_t depth = -1)
{
    using surf_type = surface<F>;

    if (surf.empty())
    {
        return std::vector<surf_type>();
    }

    size_t w = surf.width();
    size_t h = surf.height();

    size_t count = static_cast<size_t>(math::floor(math::log2(static_cast<float>(math::max(w, h)))));
    count = math::min(depth, count);

    std::vector<surf_type> mipmaps(count);
    constexpr pixel_format format = surf.format;

    VX_IF_CONSTEXPR(is_packed_format(format))
    {
        constexpr const auto masks = build_mask_array(F);
        constexpr const auto shifts = build_shift_array(F);

        for (size_t i = 0; i < count; ++i)
        {
            w /= 2;
            h /= 2;

            surf_type map(w, h);

            using pixel_type = typename surf_type::pixel_type;
            filter::filter_box<pixel_type>(
                surf.data(), surf.width(), surf.height(),
                map.data(), map.width(), map.height(),
                surf.channels(), masks, shifts
            );

            mipmaps[i] = map;
        }
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            w /= 2;
            h /= 2;

            surf_type map(w, h);

            using channel_type = typename surf_type::channel_type;
            filter::filter_box<channel_type>(
                surf.data(), surf.width(), surf.height(),
                map.data(), map.width(), map.height(),
                surf.channels()
            );

            mipmaps[i] = map;
        }
    }

    return mipmaps;
}

} // namespace pixel
} // namespace vx