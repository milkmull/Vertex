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
inline bool copy_raw(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image horizontally in place.
///
/// @param data Pointer to the image data to be flipped.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline bool flip_x_raw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
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
            std::swap_ranges(srcpx, srcpx + pixel_size, dstpx);
        }
    }

    return true;
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
inline bool flip_x_raw(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip an image vertically in place.
///
/// @param data Pointer to the image data to be flipped.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline bool flip_y_raw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    const size_t rowsz = width * pixel_size;

    for (size_t y = 0; y < height / 2; ++y)
    {
        uint8_t* row1 = &data[width * y * pixel_size];
        uint8_t* row2 = &data[width * (height - y - 1) * pixel_size];
        std::swap_ranges(row1, &row1[rowsz], row2);
    }

    return true;
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
inline bool flip_y_raw(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees clockwise and copy it to a new buffer.
///
/// @param src Pointer to the source image data to be rotated.
/// @param src_width The width of the source image.
/// @param src_height The height of the source image.
/// @param dst Pointer to the destination buffer to store the rotated image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline bool rotate_90_cw_raw(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees clockwise in place.
///
/// @param data Pointer to the image data to be rotated.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline bool rotate_90_cw_raw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    std::vector<uint8_t> dst(width * height * pixel_size);
    rotate_90_cw_raw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);

    return true;
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
inline bool rotate_90_ccw_raw(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate an image 90 degrees counterclockwise in place.
///
/// @param data Pointer to the image data to be rotated.
/// @param width The width of the image.
/// @param height The height of the image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline bool rotate_90_ccw_raw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
{
    if (!data)
    {
        return false;
    }

    std::vector<uint8_t> dst(width * height * pixel_size);
    rotate_90_ccw_raw(data, width, height, dst.data(), pixel_size);
    std::move(dst.begin(), dst.end(), data);

    return true;
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
inline bool rotate_180_raw(uint8_t* data, size_t width, size_t height, size_t pixel_size)
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

    return true;
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
inline bool rotate_180_raw(
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
/// @param area_x The x coordinate of the crop area within the image.
/// @param area_y The y coordinate of the crop area within the image.
/// @param area_width The width of the crop area within the image.
/// @param area_height The height of the crop area within the image.
/// 
/// @return True if the crop operation was successful, false otherwise.
///////////////////////////////////////////////////////////////////////////////
template <typename U>
inline bool crop_raw(
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