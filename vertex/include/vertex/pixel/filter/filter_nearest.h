#pragma once

#include <cassert>
#include <cstring>
#include <type_traits>

namespace vx {
namespace pixel {
namespace filter {

// https://www.youtube.com/watch?v=4s30cnqxJ-0

///////////////////////////////////////////////////////////////////////////////
/// @brief Apply nearest-neighbor filtering to an image.
///
/// This function applies nearest-neighbor filtering to an image and
/// maps the result to a new image. The images are assumed to be of
/// the same format and have the same channel count in each pixel.
///
/// @tparam T The channel type of the image (must be an arithmetic type).
///
/// @param src Pointer to the source image data.
/// @param src_width Width of the source image.
/// @param src_height Height of the source image.
/// @param dst Pointer to the destination image data.
/// @param dst_width Width of the destination image.
/// @param dst_height Height of the destination image.
/// @param pixel_size The the size in bytes of the pixels.
///////////////////////////////////////////////////////////////////////////////
inline void filter_nearest(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t dst_width, size_t dst_height,
    size_t pixel_size
)
{
    assert(src != nullptr);
    assert(dst != nullptr);

    if (dst_width == 0 || dst_height == 0 || pixel_size == 0)
    {
        return;
    }
    if (src_width == 0 || src_height == 0)
    {
        std::memset(dst, 0, dst_width * dst_height * pixel_size);
        return;
    }

    const size_t src_row_size = src_width * pixel_size;
    const size_t dst_row_size = dst_width * pixel_size;

    // Loop over each column in the destination image
    for (size_t y = 0; y < dst_height; ++y)
    {
        // Calculate the corresponding y-coordinate in the source image
        const size_t srcy = y * src_height / dst_height;

        // Calculate pointers to the current row in the source and destination images
        const uint8_t* srcrow = &src[src_row_size * srcy];
        uint8_t* dstpx = &dst[dst_row_size * y];

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            // Calculate pointer to the source pixel to sample from
            const uint8_t* srcpx = &srcrow[x * src_width / dst_width * pixel_size];

            // Copy the pixel data from source to destination
            std::memcpy(dstpx, srcpx, pixel_size);
        }
    }
}

}
}
}
