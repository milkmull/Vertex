#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <vector>

namespace vx {
namespace pixel {
namespace transform {

///////////////////////////////////////////////////////////////////////////////
// copy
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Copy an image to a new buffer.
///
/// @param src Pointer to the source image data to be coppied.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the coppied image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void copy(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

    std::memcpy(dst, src, src_width * src_height * pixel_size);
}

///////////////////////////////////////////////////////////////////////////////
// flip_x
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image horizontally in place.
///
/// @param data Pointer to the image data to be flipped.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void flip_x(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    assert(data);

    for (size_t y = 0; y < height; ++y)
    {
        uint8_t* row = &data[width * y * pixel_size];

        for (size_t x = 0; x < width / 2; ++x)
        {
            uint8_t* srcpx = &row[x * pixel_size];
            uint8_t* dstpx = &row[(width - x - 1) * pixel_size];

            std::swap_ranges(srcpx, srcpx + pixel_size, dstpx);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image horizontally and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be flipped.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the flipped image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void flip_x(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

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
}

///////////////////////////////////////////////////////////////////////////////
// flip_y
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image vertically in place.
///
/// @param data Pointer to the image data to be flipped.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void flip_y(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    assert(data);

    const size_t rowsz = width * pixel_size;

    for (size_t y = 0; y < height / 2; ++y)
    {
        uint8_t* row1 = &data[width * y * pixel_size];
        uint8_t* row2 = &data[width * (height - y - 1) * pixel_size];

        std::swap_ranges(row1, &row1[rowsz], row2);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image vertically and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be flipped.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the flipped image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void flip_y(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

    const size_t rowsz = src_width * pixel_size;

    for (size_t y = 0; y < src_height; ++y)
    {
        const uint8_t* srcrow = &src[src_width * y * pixel_size];
        uint8_t* dstrow = &dst[src_width * (src_height - y - 1) * pixel_size];

        std::memcpy(dstrow, srcrow, rowsz);
    }
}

///////////////////////////////////////////////////////////////////////////////
// rotate_90_cw
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees clockwise and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be rotated.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the rotated image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_90_cw(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

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
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees clockwise in place.
///
/// @param data Pointer to the image data to be rotated.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_90_cw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    assert(data);

    std::vector<uint8_t> dst(width * height * pixel_size);

    rotate_90_cw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);
}

///////////////////////////////////////////////////////////////////////////////
// rotate_90_ccw
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees counterclockwise and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be rotated.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the rotated image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_90_ccw(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

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
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees counterclockwise in place.
///
/// @param data Pointer to the image data to be rotated.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_90_ccw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    assert(data);

    std::vector<uint8_t> dst(width * height * pixel_size);

    rotate_90_ccw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);
}

///////////////////////////////////////////////////////////////////////////////
// rotate_180
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 180 degrees in place.
///
/// @param data Pointer to the image data to be rotated.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_180(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    assert(data);

    for (size_t y = 0; y < height / 2; ++y)
    {
        uint8_t* srcrow = &data[width * y * pixel_size];
        uint8_t* dstrow = &data[width * (height - y - 1) * pixel_size];

        for (size_t x = 0; x < width; ++x)
        {
            uint8_t* srcpx = &srcrow[x * pixel_size];
            uint8_t* dstpx = &dstrow[(width - x - 1) * pixel_size];

            std::swap_ranges(srcpx, &srcpx[pixel_size], dstpx);
        }
    }

    if (height % 2)
    {
        uint8_t* midrow = &data[width * (height / 2) * pixel_size];

        for (size_t x = 0; x < width / 2; ++x)
        {
            uint8_t* srcpx = &midrow[x * pixel_size];
            uint8_t* dstpx = &midrow[(width - x - 1) * pixel_size];

            std::swap_ranges(srcpx, srcpx + pixel_size, dstpx);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 180 degrees and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be rotated.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the rotated image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void rotate_180(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size
)
{
    assert(src);
    assert(dst);

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
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a rectangular region of interest from the source image and
/// copies it to the destination buffer.
///
/// @tparam U The data type of the crop area coordinates.
/// @param src Pointer to the source image data.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer where the cropped image will
/// be copied.
/// @param pixel_size The the size in bytes of the pixels.
/// @param area An array containing the coordinates and size of the area to be
/// cropped. The format is [x, y, width, height].
/// 
/// @return True if the crop operation was successful, false otherwise.
///////////////////////////////////////////////////////////////////////////////
template <typename U>
inline constexpr bool crop(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t pixel_size,
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

    for (size_t y = 0; y < static_cast<size_t>(area[3]); ++y)
    {
        const uint8_t* srcrow = &src[src_width * (static_cast<size_t>(area[1]) + y) * pixel_size];
        uint8_t* dstrow = &dst[static_cast<size_t>(area[2]) * y * pixel_size];

        for (size_t x = 0; x < static_cast<size_t>(area[2]); ++x)
        {
            const uint8_t* srcpx = &srcrow[(static_cast<size_t>(area[0]) + x) * pixel_size];
            uint8_t* dstpx = &dstrow[x * pixel_size];

            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }

    return true;
}

}
}
}