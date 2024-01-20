#pragma once

#include <type_traits>
#include <cassert>

namespace vx {
namespace math {

template <typename T, typename F>
inline constexpr void scale_bicubic(
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

    const int srcxmax = static_cast<int>(src_width) - 1;
    const int srcymax = static_cast<int>(src_height) - 1;

    const F xscale = static_cast<F>(src_width) / dst_width;
    const F yscale = static_cast<F>(src_height) / dst_height;

    for (size_t y = 0; y < dst_height; y++)
    {
        const F srcyfrac = y * yscale - static_cast<F>(0.5);
        const int srcy = static_cast<int>(srcyfrac);

        T* dstpx = &dst_data[dst_row_size * y];

        const F ty = srcyfrac - srcy;
        const F tty = ty * ty;
        const F ttty = tty * ty;

        const F qy[4] =
        {
            static_cast<F>(0.5) * (-ttty + static_cast<F>(2) * tty - ty),
            static_cast<F>(0.5) * (static_cast<F>(3) * ttty - static_cast<F>(5) * tty + static_cast<F>(2)),
            static_cast<F>(0.5) * (-static_cast<F>(3) * ttty + static_cast<F>(4) * tty + ty),
            static_cast<F>(0.5) * (ttty - tty)
        };

        for (size_t x = 0; x < dst_width; x++, dstpx += channels)
        {
            const F srcxfrac = x * xscale - static_cast<F>(0.5);
            const int srcx = static_cast<int>(srcxfrac);

            const F tx = srcxfrac - srcx;
            const F ttx = tx * tx;
            const F tttx = ttx * tx;

            const F qx[4] =
            {
                static_cast<F>(0.5) * (-tttx + static_cast<F>(2) * ttx - tx),
                static_cast<F>(0.5) * (static_cast<F>(3) * tttx - static_cast<F>(5) * ttx + static_cast<F>(2)),
                static_cast<F>(0.5) * (-static_cast<F>(3) * tttx + static_cast<F>(4) * ttx + tx),
                static_cast<F>(0.5) * (tttx - ttx)
            };

            const T* pixels[4][4]{};

            for (int i = 0; i < 4; i++)
            {
                int sy = std::clamp(srcy + i - 1, 0, srcymax);
                const T* srcrow = &src_data[src_row_size * sy];

                for (int j = 0; j < 4; j++)
                {
                    int sx = std::clamp(srcx + j - 1, 0, srcxmax);
                    pixels[i][j] = &srcrow[sx * channels];
                }
            }

            for (size_t c = 0; c < channels; c++)
            {
                const F px = (
                    (pixels[0][0][c] * qx[0] + pixels[0][1][c] * qx[1] + pixels[0][2][c] * qx[2] + pixels[0][3][c] * qx[3]) * qy[0] +
                    (pixels[1][0][c] * qx[0] + pixels[1][1][c] * qx[1] + pixels[1][2][c] * qx[2] + pixels[1][3][c] * qx[3]) * qy[1] +
                    (pixels[2][0][c] * qx[0] + pixels[2][1][c] * qx[1] + pixels[2][2][c] * qx[2] + pixels[2][3][c] * qx[3]) * qy[2] +
                    (pixels[3][0][c] * qx[0] + pixels[3][1][c] * qx[1] + pixels[3][2][c] * qx[2] + pixels[3][3][c] * qx[3]) * qy[3]
                );

                dstpx[c] = static_cast<T>(std::clamp(px, min, max));
            }
        }
    }
}

}
}