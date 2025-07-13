#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace vx {
namespace pixel {
namespace raw {

///////////////////////////////////////////////////////////////////////////////
// copy
///////////////////////////////////////////////////////////////////////////////

inline bool copy(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    std::memcpy(dst, src, src_width * src_height * pixel_size);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// flip_x
///////////////////////////////////////////////////////////////////////////////

inline bool flip_x(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    for (size_t y = 0; y < height; ++y)
    {
        uint8_t* row = &data[width * y * pixel_size];

        for (size_t x = 0; x < width / 2; ++x)
        {
            uint8_t* srcpx = &row[x * pixel_size];
            uint8_t* dstpx = &row[(width - x - 1) * pixel_size];

            for (size_t z = 0; z < pixel_size; ++z)
            {
                const uint8_t tmp = srcpx[z];
                srcpx[z] = dstpx[z];
                dstpx[z] = tmp;
            }
        }
    }

    return true;
}

inline bool flip_x(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstrow = &dst[src_width * y * pixel_size];

        for (size_t x = 0; x < src_width; ++x)
        {
            const uint8_t* srcpx = &srcrow[x * pixel_size];
            uint8_t* dstpx = &dstrow[(src_width - x - 1) * pixel_size];
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// flip_y
///////////////////////////////////////////////////////////////////////////////

inline bool flip_y(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    const size_t rowsz = width * pixel_size;
    std::vector<uint8_t> tmp_row(rowsz);

    for (size_t y = 0; y < height / 2; ++y)
    {
        uint8_t* row1 = &data[y * rowsz];
        uint8_t* row2 = &data[(height - y - 1) * rowsz];

        std::memcpy(tmp_row.data(), row1, rowsz);
        std::memcpy(row1, row2, rowsz);
        std::memcpy(row2, tmp_row.data(), rowsz);
    }

    return true;
}

inline bool flip_y(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    const size_t rowsz = src_width * pixel_size;

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstrow = &dst[src_width * (src_height - y - 1) * pixel_size];
        std::memcpy(dstrow, srcrow, rowsz);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// rotate_90_cw
///////////////////////////////////////////////////////////////////////////////

inline bool rotate_90_cw(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstcol = &dst[(src_height - y - 1) * pixel_size];

        for (size_t x = 0; x < src_width; ++x)
        {
            const uint8_t* srcpx = &srcrow[x * pixel_size];
            uint8_t* dstpx = &dstcol[x * src_height * pixel_size];
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

inline bool rotate_90_cw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    std::vector<uint8_t> dst(width * height * pixel_size);
    rotate_90_cw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// rotate_90_ccw
///////////////////////////////////////////////////////////////////////////////

inline bool rotate_90_ccw(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstcol = &dst[y * pixel_size];

        for (size_t x = 0; x < src_width; ++x)
        {
            const uint8_t* srcpx = &srcrow[(src_width - x - 1) * pixel_size];
            uint8_t* dstpx = &dstcol[x * src_height * pixel_size];
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

inline bool rotate_90_ccw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    std::vector<uint8_t> dst(width * height * pixel_size);
    rotate_90_ccw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// rotate_180
///////////////////////////////////////////////////////////////////////////////

inline bool rotate_180(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    for (size_t y = 0; y < height / 2; ++y)
    {
        uint8_t* srcrow = &data[width * y * pixel_size];
        uint8_t* dstrow = &data[width * (height - y - 1) * pixel_size];

        for (size_t x = 0; x < width; ++x)
        {
            uint8_t* srcpx = &srcrow[x * pixel_size];
            uint8_t* dstpx = &dstrow[(width - x - 1) * pixel_size];

            for (size_t z = 0; z < pixel_size; ++z)
            {
                const uint8_t tmp = srcpx[z];
                srcpx[z] = dstpx[z];
                dstpx[z] = tmp;
            }
        }
    }

    if (height % 2)
    {
        uint8_t* midrow = &data[width * (height / 2) * pixel_size];

        for (size_t x = 0; x < width / 2; ++x)
        {
            uint8_t* srcpx = &midrow[x * pixel_size];
            uint8_t* dstpx = &midrow[(width - x - 1) * pixel_size];

            for (size_t z = 0; z < pixel_size; ++z)
            {
                const uint8_t tmp = srcpx[z];
                srcpx[z] = dstpx[z];
                dstpx[z] = tmp;
            }
        }
    }

    return true;
}

inline bool rotate_180(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    if (!src || !dst)
    {
        return false;
    }

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstrow = &dst[src_width * (src_height - y - 1) * pixel_size];

        for (size_t x = 0; x < src_width; ++x)
        {
            const uint8_t* srcpx = &srcrow[x * pixel_size];
            uint8_t* dstpx = &dstrow[(src_width - x - 1) * pixel_size];
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

template <typename U>
inline bool crop(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size,
    U area_x, U area_y, U area_width, U area_height
)
{
    if (!src || !dst)
    {
        return false;
    }

    // check crop area is within area of image
    if ((area_x < static_cast<U>(0)) || (static_cast<size_t>(area_x + area_width) > src_width) ||
        (area_y < static_cast<U>(0)) || (static_cast<size_t>(area_y + area_height) > src_height))
    {
        return false;
    }

    for (size_t y = 0; y < static_cast<size_t>(area_height); ++y)
    {
        const uint8_t* srcrow = &src[src_width * (static_cast<size_t>(area_y) + y) * pixel_size];
        uint8_t* dstrow = &dst[static_cast<size_t>(area_width) * y * pixel_size];

        for (size_t x = 0; x < static_cast<size_t>(area_width); ++x)
        {
            const uint8_t* srcpx = &srcrow[(static_cast<size_t>(area_x) + x) * pixel_size];
            uint8_t* dstpx = &dstrow[x * pixel_size];
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

}
}
}