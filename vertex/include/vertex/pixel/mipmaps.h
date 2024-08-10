#pragma once

#include "surface.h"
#include "vertex/pixel/filter/filter_box.h"
#include "vertex/math/math/fn/fn_common.h"
#include "vertex/math/math/fn/fn_exponential.h"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
/// @brief Generates a series of mipmaps for the given surface.
///
/// @param surf The original surface for which mipmaps are to be generated.
/// @param depth The maximum depth of mipmaps to generate. Default is -1,
/// which indicates generating mipmaps down to the smallest possible size based
/// on the dimensions of the original surface.
///
/// @return A vector containing the generated mipmaps.
///////////////////////////////////////////////////////////////////////////////
inline std::vector<surface> generate_mipmaps(const surface& surf, size_t depth = -1)
{
    if (surf.empty())
    {
        return std::vector<surface>();
    }

    size_t w = surf.width();
    size_t h = surf.height();

    size_t count = static_cast<size_t>(math::floor(math::log2(static_cast<float>(math::max(w, h)))));
    count = math::min(depth, count);

    std::vector<surface> mipmaps(count);
    pixel_format format = surf.format();

    uint32_t masks[4]{};
    uint32_t shifts[4]{};

    if (is_packed_format(format))
    {
        channel_info info = get_channel_info(format);

        masks[info.r.index] = info.r.mask;
        masks[info.g.index] = info.g.mask;
        masks[info.b.index] = info.b.mask;
        masks[info.a.index] = info.a.mask;

        shifts[info.r.index] = info.r.shift;
        shifts[info.g.index] = info.g.shift;
        shifts[info.b.index] = info.b.shift;
        shifts[info.a.index] = info.a.shift;
    }

    for (size_t i = 0; i < count; ++i)
    {
        w /= 2;
        h /= 2;

        surface map(w, h, format);

        switch (format)
        {
            case pixel_format::RGB_332:
            {
                filter::filter_box<uint8_t>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels(), masks, shifts
                );
                break;
            }
            case pixel_format::RGBA_4444:
            case pixel_format::BGRA_4444:
            case pixel_format::RGB_565:
            case pixel_format::BGR_565:
            case pixel_format::RGBA_5551:
            case pixel_format::BGRA_5551:
            case pixel_format::ARGB_1555:
            {
                filter::filter_box<uint16_t>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels(), masks, shifts
                );
                break;
            }
            case pixel_format::RGBA_8888:
            case pixel_format::BGRA_8888:
            case pixel_format::ABGR_8888:
            case pixel_format::ARGB_2101010:
            case pixel_format::ABGR_2101010:
            {
                filter::filter_box<uint32_t>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels(), masks, shifts
                );
                break;
            }
            case pixel_format::R_8:
            case pixel_format::RG_8:
            case pixel_format::RGB_8:
            case pixel_format::BGR_8:
            case pixel_format::RGBA_8:
            case pixel_format::BGRA_8:
            case pixel_format::ABGR_8:
            {
                filter::filter_box<uint8_t>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels()
                );
                break;
            }
            case pixel_format::R_16:
            case pixel_format::RG_16:
            case pixel_format::RGB_16:
            case pixel_format::RGBA_16:
            {
                filter::filter_box<uint16_t>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels()
                );
                break;
            }
            case pixel_format::R_32F:
            case pixel_format::RG_32F:
            case pixel_format::RGB_32F:
            case pixel_format::RGBA_32F:
            {
                filter::filter_box<float>(
                    surf.data(), surf.width(), surf.height(),
                    map.data(), map.width(), map.height(),
                    surf.channels()
                );
                break;
            }
            default:
            {
                break;
            }
        }

        mipmaps[i] = map;
    }

    return mipmaps;
}

}
}