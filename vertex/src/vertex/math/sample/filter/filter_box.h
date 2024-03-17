#pragma once

#include <cstring>
#include <type_traits>

namespace vx {
namespace math {

// https://www.youtube.com/watch?v=4s30cnqxJ-0
// https://www.youtube.com/watch?v=_htjjOdXbmA

// box filtering uses the same algorithm as bilinear, just with the weights equally applied to the surrounding pixels (0.25)

/**
 * @brief Apply box filtering to an image.
 *
 * This function applies box filtering to an image and
 * mapps the result to a new image. The images are assumed to be of
 * the same format and have the same channel count in each pixel.
 *
 * @tparam T The channel type of the image (must be an arithmetic type).
 * @tparam F The floating-point type to use in floating-point calculations (defaults to float).
 *
 * @param src Pointer to the source image data.
 * @param src_width Width of the source image.
 * @param src_height Height of the source image.
 * @param dst Pointer to the destination image data.
 * @param dst_width Width of the destination image.
 * @param dst_height Height of the destination image.
 * @param channels Number of channels in the images.
 */
template <typename T, typename F = float>
inline constexpr void filter_box(
    const T* src, size_t src_width, size_t src_height,
    T* dst, size_t dst_width, size_t dst_height,
    size_t channels
)
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmatic type");
    static_assert(std::is_floating_point<F>::value, "F must be floating point type");

    constexpr F min = static_cast<F>(std::numeric_limits<T>::min());
    constexpr F max = static_cast<F>(std::numeric_limits<T>::max());

    assert(src != nullptr);
    assert(dst != nullptr);

    if (dst_width == 0 || dst_height == 0)
    {
        return;
    }
    if (src_width == 0 || src_height == 0)
    {
        std::memset(dst, 0, dst_width * dst_height * sizeof(T) * channels);
        return;
    }

    const size_t src_row_size = src_width * channels;
    const size_t dst_row_size = dst_width * channels;

    const size_t srcxmax = src_width - 1;
    const size_t srcymax = src_height - 1;

    // Compute scaling factors for width and height
    const F xscale = static_cast<F>(src_width) / dst_width;
    const F yscale = static_cast<F>(src_height) / dst_height;

    // Interpolation weight
    constexpr F weight = static_cast<F>(0.25);

    // Loop over each row in the destination image
    for (size_t y = 0; y < dst_height; ++y)
    {
        // Map the vertical offset back to the source image to figure out what row
        // should be sampled from
        // We offset by -0.5 to align with the center of the source pixels
        const F srcyfrac = y * yscale - static_cast<F>(0.5);

        // Calculate pointer to the current row in the source image
        size_t srcy = static_cast<size_t>(srcyfrac);
        const T* srcrow = &src[src_row_size * srcy];

        // Calculate the offset required to get the source pixel above or below
        const size_t dy = static_cast<size_t>(srcy < srcymax) * src_row_size;

        // Calculate the pointer to the destination pixel (the pixel that will be written to)
        T* dstpx = &dst[dst_row_size * y];

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += channels)
        {
            // Map the horizontal offset back to the source image to figure out what column
            // should be sampled from
            // We offset by -0.5 to align with the center of the source pixels
            const F srcxfrac = x * xscale - static_cast<F>(0.5);

            // Calculate pointer to the current pixel in the source image to sample from
            const size_t srcx = static_cast<size_t>(srcxfrac);
            const T* srcpx = &srcrow[srcx * channels];

            // Calculate the offset required to get the source pixel to the left or right
            const size_t dx = static_cast<size_t>(srcx < srcxmax) * channels;

            // Array of pointers to the four neighboring pixels used in interpolation
            const T* pixels[4] =
            {
                srcpx,
                srcpx + dx,
                srcpx + dy,
                srcpx + dx + dy
            };

            // Loop over each channel
            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bilinear interpolation using the four neighboring pixels
                const F px = (
                    pixels[0][c] * weight +
                    pixels[1][c] * weight +
                    pixels[2][c] * weight +
                    pixels[3][c] * weight
                );

                dstpx[c] = static_cast<T>(std::clamp(px, min, max));
            }
        }
    }
}

}
}