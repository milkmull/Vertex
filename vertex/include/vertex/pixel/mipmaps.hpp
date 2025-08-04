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
    using channel_type = typename surf_type::channel_type;

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
        constexpr channel_info info = get_channel_info(format);

        uint32_t masks[4]{};
        uint32_t shifts[4]{};

        masks[info.r.index] = info.r.mask;
        masks[info.g.index] = info.g.mask;
        masks[info.b.index] = info.b.mask;
        masks[info.a.index] = info.a.mask;

        shifts[info.r.index] = info.r.shift;
        shifts[info.g.index] = info.g.shift;
        shifts[info.b.index] = info.b.shift;
        shifts[info.a.index] = info.a.shift;

        for (size_t i = 0; i < count; ++i)
        {
            w /= 2;
            h /= 2;

            surf_type map(w, h);

            filter_box<channel_type>(
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

            filter_box<channel_type>(
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