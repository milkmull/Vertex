#pragma once

#include <type_traits>
#include <cassert>

namespace vx {
namespace math {

// https://stackoverflow.com/questions/13888210/extremely-slow-bilinear-interpolation-compared-to-opencv

template <typename T, typename F>
inline constexpr void scale_bilinear(
    const T* src_data, size_t src_width, size_t src_height,
          T* dst_data, size_t dst_width, size_t dst_height,
    size_t channels
)
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmatic type");
    static_assert(std::is_floating_point<F>::value, "F must be floating point type");

    constexpr F min = static_cast<F>(std::numeric_limits<T>::min());
    constexpr F max = static_cast<F>(std::numeric_limits<T>::max());

    assert(src_data != nullptr);
    assert(dst_data != nullptr);

    if (dst_width == 0 || dst_height == 0)
    {
        return;
    }
    if (src_width == 0 || src_height == 0)
    {
        std::memset(dst_data, 0, dst_width * dst_height * sizeof(T) * channels);
        return;
    }

    const size_t src_row_size = src_width * channels;
    const size_t dst_row_size = dst_width * channels;

    const size_t srcxmax = src_width - 1;
    const size_t srcymax = src_height - 1;

    const F xscale = static_cast<F>(src_width) / dst_width;
    const F yscale = static_cast<F>(src_height) / dst_height;

    for (size_t y = 0; y < dst_height; y++)
    {
        const F srcyfrac = y * yscale - static_cast<F>(0.5);
        size_t srcy = static_cast<size_t>(srcyfrac);

        const T* srcrow = &src_data[src_row_size * srcy];
        const size_t dy = (srcy < srcymax) * src_row_size;

        T* dstpx = &dst_data[dst_row_size * y];

        const F fy2 = srcyfrac - srcy;
        const F fy = 1 - fy2;

        for (size_t x = 0; x < dst_width; x++, dstpx += channels)
        {
            const F srcxfrac = x * xscale - static_cast<F>(0.5);
            const size_t srcx = static_cast<size_t>(srcxfrac);

            const T* srcpx = &srcrow[srcx * channels];
            const size_t dx = (srcx < srcxmax) * channels;

            const F fx2 = srcxfrac - srcx;
            const F fx = 1 - fx2;

            const T* pixels[4] =
            {
                srcpx,
                srcpx + dx,
                srcpx + dy,
                srcpx + dx + dy
            };

            const F weights[4] =
            {
                fx  * fy,
                fx2 * fy,
                fx  * fy2,
                fx2 * fy2
            };

            for (size_t c = 0; c < channels; c++)
            {
                const F px = (
                    pixels[0][c] * weights[0] +
                    pixels[1][c] * weights[1] +
                    pixels[2][c] * weights[2] +
                    pixels[3][c] * weights[3]
                );

                dstpx[c] = static_cast<T>(std::clamp(px, min, max));
            }
        }
    }
}

}
}