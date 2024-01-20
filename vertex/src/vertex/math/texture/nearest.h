#pragma once

#include <type_traits>
#include <cassert>

namespace vx {
namespace math{

template <typename T>
inline constexpr void scale_nearest(
    const T* src_data, size_t src_width, size_t src_height,
          T* dst_data, size_t dst_width, size_t dst_height,
    size_t channels
)
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmatic type");

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

    const size_t pxsz = sizeof(T) * channels;

    const size_t src_row_size = src_width * channels;
    const size_t dst_row_size = dst_width * channels;

    for (size_t y = 0; y < dst_height; y++)
    {
        const size_t srcy = y * src_height / dst_height;

        const T* srcrow = &src_data[src_row_size * srcy];
        T* dstpx = &dst_data[dst_row_size * y];

        for (size_t x = 0; x < dst_width; x++, dstpx += channels)
        {
            const T* srcpx = &srcrow[x * src_width / dst_width * channels];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }
}

}
}
