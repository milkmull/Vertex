#pragma once

#include <cassert>
#include <cstring>
#include <vector>
#include <array>

namespace vx {
namespace img {
namespace raw {

// =============== flip_x ===============

template <typename T>
inline constexpr void flip_x(T* data, size_t width, size_t height, size_t channels)
{
    assert(data);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < height; ++y)
    {
        T* row = &data[width * y * pxsz];

        for (size_t x = 0; x < width / 2; ++x)
        {
            T* srcpx = &row[x * pxsz];
            T* dstpx = &row[(width - x - 1) * pxsz];

            std::swap_ranges(srcpx, &srcpx[channels], dstpx);
        }
    }
}

template <typename T>
inline constexpr void flip_x(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels
)
{
    assert(src);
    assert(dst);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < src_height; ++y)
    {
        const T* srcrow = &src[src_width * y * pxsz];
        T* dstrow = &dst[src_width * y * pxsz];

        for (size_t x = 0; x < src_width; ++x)
        {
            const T* srcpx = &srcrow[x * pxsz];
            T* dstpx = &dstrow[(src_width - x - 1) * pxsz];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }
}

// =============== flip_y ===============

template <typename T>
inline constexpr void flip_y(T* data, size_t width, size_t height, size_t channels)
{
    assert(data);

    const size_t pxsz = sizeof(T) * channels;
    const size_t rowsz = width * pxsz;

    for (size_t y = 0; y < height / 2; ++y)
    {
        T* row1 = &data[width * y * pxsz];
        T* row2 = &data[width * (height - y - 1) * pxsz];

        std::swap_ranges(row1, &row1[rowsz], row2);
    }
}

template <typename T>
inline constexpr void flip_y(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels
)
{
    assert(src);
    assert(dst);

    const size_t pxsz = sizeof(T) * channels;
    const size_t rowsz = src_width * pxsz;

    for (size_t y = 0; y < src_height; ++y)
    {
        const T* srcrow = &src[src_width * y * pxsz];
        T* dstrow = &dst[src_width * (src_height - y - 1) * pxsz];

        std::memcpy(dstrow, srcrow, rowsz);
    }
}

// =============== rotate_90_cw ===============

template <typename T>
inline constexpr void rotate_90_cw(T* data, size_t width, size_t height, size_t channels)
{
    assert(data);

    const size_t pxsz = sizeof(T) * channels;
    std::vector<T> dst(width * height * pxsz);

    rotate_90_cw(data, width, height, dst.data(), channels);
    std::move(dst.begin(), dst.end(), data);
}

template <typename T>
inline constexpr void rotate_90_cw(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels
)
{
    assert(src);
    assert(dst);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < src_height; ++y)
    {
        const T* srcrow = &src[src_width * y * pxsz];
        T* dstcol = &dst[(src_height - y - 1) * pxsz];

        for (size_t x = 0; x < src_width; ++x)
        {
            const T* srcpx = &srcrow[x * pxsz];
            T* dstpx = &dstcol[x * src_height * pxsz];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }
}

// =============== rotate_90_ccw ===============

template <typename T>
inline constexpr void rotate_90_ccw(T* data, size_t width, size_t height, size_t channels)
{
    assert(data);

    const size_t pxsz = sizeof(T) * channels;
    std::vector<T> dst(width * height * pxsz);

    rotate_90_ccw(data, width, height, dst.data(), channels);
    std::move(dst.begin(), dst.end(), data);
}

template <typename T>
inline constexpr void rotate_90_ccw(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels
)
{
    assert(src);
    assert(dst);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < src_height; ++y)
    {
        const T* srcrow = &src[src_width * y * pxsz];
        T* dstcol = &dst[y * pxsz];

        for (size_t x = 0; x < src_width; ++x)
        {
            const T* srcpx = &srcrow[(src_width - x - 1) * pxsz];
            T* dstpx = &dstcol[x * src_height * pxsz];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }
}

// =============== rotate_180 ===============

template <typename T>
inline constexpr void rotate_180(T* data, size_t width, size_t height, size_t channels)
{
    assert(data);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < height / 2; ++y)
    {
        T* srcrow = &data[width * y * pxsz];
        T* dstrow = &data[width * (height - y - 1) * pxsz];

        for (size_t x = 0; x < width; ++x)
        {
            T* srcpx = &srcrow[x * pxsz];
            T* dstpx = &dstrow[(width - x - 1) * pxsz];

            std::swap_ranges(srcpx, &srcpx[channels], dstpx);
        }
    }

    if (height % 2)
    {
        T* midrow = &data[width * (height / 2) * pxsz];

        for (size_t x = 0; x < width / 2; ++x)
        {
            T* srcpx = &midrow[x * pxsz];
            T* dstpx = &midrow[(width - x - 1) * pxsz];

            std::swap_ranges(srcpx, &srcpx[channels], dstpx);
        }
    }
}

template <typename T>
inline constexpr void rotate_180(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels
)
{
    assert(src);
    assert(dst);

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < src_height; ++y)
    {
        const T* srcrow = &src[src_width * y * pxsz];
        T* dstrow = &dst[src_width * (src_height - y - 1) * pxsz];

        for (size_t x = 0; x < src_width; ++x)
        {
            const T* srcpx = &srcrow[x * pxsz];
            T* dstpx = &dstrow[(src_width - x - 1) * pxsz];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }
}

// =============== crop ===============

template <typename T, typename U>
inline constexpr bool crop(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t channels,
    const std::array<U, 4>& area
)
{
    assert(src);
    assert(dst);

    // check crop area is within area of image
    if ((area[0] < static_cast<U>(0)) || (static_cast<size_t>(area[0] + area[2]) > src_width) ||
        (area[1] < static_cast<U>(0)) || (static_cast<size_t>(area[1] + area[3]) > src_height))
    {
        return false;
    }

    const size_t pxsz = sizeof(T) * channels;

    for (size_t y = 0; y < static_cast<size_t>(area[3]); ++y)
    {
        const T* srcrow = &src[src_width * (static_cast<size_t>(area[1]) + y) * pxsz];
        T* dstrow = &dst[static_cast<size_t>(area[2]) * y * pxsz];

        for (size_t x = 0; x < static_cast<size_t>(area[2]); ++x)
        {
            const T* srcpx = &srcrow[(static_cast<size_t>(area[0]) + x) * pxsz];
            T* dstpx = &dstrow[x * pxsz];

            std::memcpy(dstpx, srcpx, pxsz);
        }
    }

    return true;
}

}
}
}