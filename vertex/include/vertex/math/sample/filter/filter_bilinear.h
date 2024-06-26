#pragma once

#include <cstring>
#include <type_traits>

namespace vx {
namespace math {

// https://stackoverflow.com/questions/13888210/extremely-slow-bilinear-interpolation-compared-to-opencv
// https://www.youtube.com/watch?v=_htjjOdXbmA

///////////////////////////////////////////////////////////////////////////////
/// @brief Apply bilinear filtering to an image.
///
/// This function applies bilinear filtering to an image and
/// maps the result to a new image. The images are assumed to be of
/// the same format and have the same channel count in each pixel.
///
/// @tparam channel_type The channel type of the image (must be an arithmetic
/// type).
/// @tparam float_type The floating-point type to use in floating-point
/// calculations (defaults to float).
///
/// @param src Pointer to the source image data.
/// @param src_width Width of the source image.
/// @param src_height Height of the source image.
/// @param dst Pointer to the destination image data.
/// @param dst_width Width of the destination image.
/// @param dst_height Height of the destination image.
/// @param channels Number of channels in the image pixels.
///////////////////////////////////////////////////////////////////////////////
template <typename channel_type, typename float_type = float>
inline constexpr void filter_bilinear(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t dst_width, size_t dst_height,
    size_t channels
)
{
    static_assert(std::is_arithmetic<channel_type>::value, "channel_type must be arithmatic type");
    static_assert(std::is_floating_point<float_type>::value, "float_type must be floating point type");

    constexpr float_type min = static_cast<float_type>(std::numeric_limits<channel_type>::min());
    constexpr float_type max = static_cast<float_type>(std::numeric_limits<channel_type>::max());

    assert(src != nullptr);
    assert(dst != nullptr);

    const size_t pixel_size = sizeof(channel_type) * channels;

    if (dst_width == 0 || dst_height == 0 || channels == 0)
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

    const size_t srcxmax = src_width - 1;
    const size_t srcymax = src_height - 1;

    // Compute scaling factors for width and height
    const float_type xscale = static_cast<float_type>(src_width) / dst_width;
    const float_type yscale = static_cast<float_type>(src_height) / dst_height;

    // Loop over each row in the destination image
    for (size_t y = 0; y < dst_height; ++y)
    {
        // Map the vertical offset back to the source image to figure out what row
        // should be sampled from
        // We offset by -0.5 to align with the center of the source pixels
        const float_type srcyfrac = y * yscale - static_cast<float_type>(0.5);

        // Calculate pointer to the current row in the source image
        size_t srcy = static_cast<size_t>(srcyfrac);
        const uint8_t* srcrow = &src[src_row_size * srcy];

        // Calculate the offset required to get the source pixel above or below
        const size_t dy = static_cast<size_t>(srcy < srcymax) * src_row_size;

        // Calculate the pointer to the destination pixel (the pixel that will be written to)
        uint8_t* dstpx = &dst[dst_row_size * y];

        // Interpolation weights for y-coordinate
        const float_type fy2 = srcyfrac - srcy;
        const float_type fy = static_cast<float_type>(1) - fy2;

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            // Map the horizontal offset back to the source image to figure out what column
            // should be sampled from
            // We offset by -0.5 to align with the center of the source pixels
            const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);

            // Calculate pointer to the current pixel in the source image to sample from
            const size_t srcx = static_cast<size_t>(srcxfrac);
            const uint8_t* srcpx = &srcrow[srcx * pixel_size];

            // Calculate the offset required to get the source pixel to the left or right
            const size_t dx = static_cast<size_t>(srcx < srcxmax) * pixel_size;

            // Interpolation weights for x-coordinate
            const float_type fx2 = srcxfrac - srcx;
            const float_type fx = static_cast<float_type>(1) - fx2;

            // Array of pointers to the four neighboring pixels used in interpolation
            const channel_type* pixels[4] =
            {
                reinterpret_cast<const channel_type*>(srcpx),
                reinterpret_cast<const channel_type*>(srcpx + dx),
                reinterpret_cast<const channel_type*>(srcpx + dy),
                reinterpret_cast<const channel_type*>(srcpx + dx + dy)
            };

            channel_type* dst_pixel = reinterpret_cast<channel_type*>(dstpx);

            // Array of weights corresponding to each neighboring pixel
            const float_type weights[4] =
            {
                fx * fy,
                fx2 * fy,
                fx * fy2,
                fx2 * fy2
            };

            // Loop over each channel
            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bilinear interpolation using the four neighboring pixels and weights
                const float_type px = (
                    pixels[0][c] * weights[0] +
                    pixels[1][c] * weights[1] +
                    pixels[2][c] * weights[2] +
                    pixels[3][c] * weights[3]
                );

                dst_pixel[c] = static_cast<channel_type>(std::clamp(px, min, max));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Apply bilinear filtering to an image with a packed pixel format.
///
/// This function applies bilinear filtering to an image and
/// maps the result to a new image. The images are assumed to be of
/// the same format and have the same channel count in each pixel.
///
/// @tparam pixel_type The data type used to pack the pixels (must be an
/// arithmetic type).
/// @tparam float_type The floating-point type to use in floating-point
/// calculations (defaults to float).
///
/// @param src Pointer to the source image data.
/// @param src_width Width of the source image.
/// @param src_height Height of the source image.
/// @param dst Pointer to the destination image data.
/// @param dst_width Width of the destination image.
/// @param dst_height Height of the destination image.
/// @param channels Number of channels in the image pixels.
/// @param channel_masks Pointer to an array of masks for each pixel channel.
/// @param channel_shifts Pointer to an array of shifts for each pixel channel.
///////////////////////////////////////////////////////////////////////////////
template <typename pixel_type, typename float_type = float>
inline constexpr void filter_bilinear(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t dst_width, size_t dst_height,
    size_t channels, const pixel_type* channel_masks, const uint8_t* channel_shifts
)
{
    static_assert(std::is_arithmetic<pixel_type>::value, "pixel_type must be arithmatic type");
    static_assert(std::is_floating_point<float_type>::value, "float_type must be floating point type");

    assert(src != nullptr);
    assert(dst != nullptr);

    assert(channel_masks != nullptr);
    assert(channel_shifts != nullptr);

    const size_t pixel_size = sizeof(pixel_type);

    if (dst_width == 0 || dst_height == 0 || channels == 0)
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

    const size_t srcxmax = src_width - 1;
    const size_t srcymax = src_height - 1;

    // Compute scaling factors for width and height
    const float_type xscale = static_cast<float_type>(src_width) / dst_width;
    const float_type yscale = static_cast<float_type>(src_height) / dst_height;

    // Loop over each row in the destination image
    for (size_t y = 0; y < dst_height; ++y)
    {
        // Map the vertical offset back to the source image to figure out what row
        // should be sampled from
        // We offset by -0.5 to align with the center of the source pixels
        const float_type srcyfrac = y * yscale - static_cast<float_type>(0.5);

        // Calculate pointer to the current row in the source image
        size_t srcy = static_cast<size_t>(srcyfrac);
        const uint8_t* srcrow = &src[src_row_size * srcy];

        // Calculate the offset required to get the source pixel above or below
        const size_t dy = static_cast<size_t>(srcy < srcymax) * src_row_size;

        // Calculate the pointer to the destination pixel (the pixel that will be written to)
        uint8_t* dstpx = &dst[dst_row_size * y];

        // Interpolation weights for y-coordinate
        const float_type fy2 = srcyfrac - srcy;
        const float_type fy = static_cast<float_type>(1) - fy2;

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            // Map the horizontal offset back to the source image to figure out what column
            // should be sampled from
            // We offset by -0.5 to align with the center of the source pixels
            const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);

            // Calculate pointer to the current pixel in the source image to sample from
            const size_t srcx = static_cast<size_t>(srcxfrac);
            const uint8_t* srcpx = &srcrow[srcx * pixel_size];

            // Calculate the offset required to get the source pixel to the left or right
            const size_t dx = static_cast<size_t>(srcx < srcxmax) * pixel_size;

            // Interpolation weights for x-coordinate
            const float_type fx2 = srcxfrac - srcx;
            const float_type fx = static_cast<float_type>(1) - fx2;

            // Array of pointers to the four neighboring pixels used in interpolation
            const pixel_type* pixels[4] =
            {
                reinterpret_cast<const pixel_type*>(srcpx),
                reinterpret_cast<const pixel_type*>(srcpx + dx),
                reinterpret_cast<const pixel_type*>(srcpx + dy),
                reinterpret_cast<const pixel_type*>(srcpx + dx + dy)
            };

            pixel_type* dst_pixel = reinterpret_cast<pixel_type*>(dstpx);

            // Array of weights corresponding to each neighboring pixel
            const float_type weights[4] =
            {
                fx * fy,
                fx2 * fy,
                fx * fy2,
                fx2 * fy2
            };

            // Loop over each channel
            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bilinear interpolation using the four neighboring pixels and weights
                const float_type px = (
                    ((*pixels[0] & channel_masks[c]) >> channel_shifts[c]) * weights[0] +
                    ((*pixels[1] & channel_masks[c]) >> channel_shifts[c]) * weights[1] +
                    ((*pixels[2] & channel_masks[c]) >> channel_shifts[c]) * weights[2] +
                    ((*pixels[3] & channel_masks[c]) >> channel_shifts[c]) * weights[3]
                );

                *dst_pixel |= (static_cast<pixel_type>(std::clamp(
                    px,
                    static_cast<float_type>(0),
                    static_cast<float_type>(channel_masks[c] >> channel_shifts[c])
                )) << channel_shifts[c]);
            }
        }
    }
}

}
}