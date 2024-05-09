#pragma once

#include <cstring>
#include <type_traits>

namespace vx {
namespace math {

// https://www.youtube.com/watch?v=_htjjOdXbmA

///////////////////////////////////////////////////////////////////////////////
/// @brief Apply bicubic filtering to an image.
///
/// This function applies bicubic filtering to an image and
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
inline constexpr void filter_bicubic(
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

    const int srcxmax = static_cast<int>(src_width) - 1;
    const int srcymax = static_cast<int>(src_height) - 1;

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
        const int srcy = static_cast<int>(srcyfrac);

        // Calculate the pointer to the destination pixel (the pixel that will be written to)
        uint8_t* dstpx = &dst[dst_row_size * y];

        // Bicubic interpolation weights for y-coordinate
        const float_type ty = srcyfrac - static_cast<float_type>(srcy);
        const float_type tty = ty * ty;
        const float_type ttty = tty * ty;

        const float_type qy[4] =
        {
            static_cast<float_type>(0.5) * (-ttty + static_cast<float_type>(2) * tty - ty),
            static_cast<float_type>(0.5) * (static_cast<float_type>(3) * ttty - static_cast<float_type>(5) * tty + static_cast<float_type>(2)),
            static_cast<float_type>(0.5)* (static_cast<float_type>(-3) * ttty + static_cast<float_type>(4) * tty + ty),
            static_cast<float_type>(0.5) * (ttty - tty)
        };

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            // Map the horizontal offset back to the source image to figure out what column
            // should be sampled from
            // We offset by -0.5 to align with the center of the source pixels
            const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);
            const int srcx = static_cast<int>(srcxfrac);

            // Bicubic interpolation weights for x-coordinate
            const float_type tx = srcxfrac - static_cast<float_type>(srcx);
            const float_type ttx = tx * tx;
            const float_type tttx = ttx * tx;

            const float_type qx[4] =
            {
                static_cast<float_type>(0.5) * (-tttx + static_cast<float_type>(2) * ttx - tx),
                static_cast<float_type>(0.5) * (static_cast<float_type>(3) * tttx - static_cast<float_type>(5) * ttx + static_cast<float_type>(2)),
                static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * tttx + static_cast<float_type>(4) * ttx + tx),
                static_cast<float_type>(0.5) * (tttx - ttx)
            };

            // Array of pointers to the four neighboring pixels used in bicubic interpolation
            const channel_type* pixels[4][4]{};

            // Populate the pixels array with neighboring pixels for interpolation
            for (int i = 0; i < 4; ++i)
            {
                int sy = std::clamp(srcy + i - 1, 0, srcymax);
                const uint8_t* srcrow = &src[src_row_size * sy];

                for (int j = 0; j < 4; ++j)
                {
                    int sx = std::clamp(srcx + j - 1, 0, srcxmax);
                    pixels[i][j] = reinterpret_cast<const channel_type*>(&srcrow[sx * pixel_size]);
                }
            }

            // Loop over each channel
            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bicubic interpolation using the sixteen neighboring pixels and weights
                const float_type px = (
                    (pixels[0][0][c] * qx[0] + pixels[0][1][c] * qx[1] + pixels[0][2][c] * qx[2] + pixels[0][3][c] * qx[3]) * qy[0] +
                    (pixels[1][0][c] * qx[0] + pixels[1][1][c] * qx[1] + pixels[1][2][c] * qx[2] + pixels[1][3][c] * qx[3]) * qy[1] +
                    (pixels[2][0][c] * qx[0] + pixels[2][1][c] * qx[1] + pixels[2][2][c] * qx[2] + pixels[2][3][c] * qx[3]) * qy[2] +
                    (pixels[3][0][c] * qx[0] + pixels[3][1][c] * qx[1] + pixels[3][2][c] * qx[2] + pixels[3][3][c] * qx[3]) * qy[3]
                );

                dstpx[c] = static_cast<channel_type>(std::clamp(px, min, max));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Apply bicubic filtering to an image with a packed pixel format.
///
/// This function applies bicubic filtering to an image and
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
/// @param channel_masks Pointer to an array of masks for each pixel channel.
/// @param channel_shifts Pointer to an array of shifts for each pixel channel.
///////////////////////////////////////////////////////////////////////////////
template <typename pixel_type, typename float_type = float>
inline constexpr void filter_bicubic(
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

    const int srcxmax = static_cast<int>(src_width) - 1;
    const int srcymax = static_cast<int>(src_height) - 1;

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
        const int srcy = static_cast<int>(srcyfrac);

        // Calculate the pointer to the destination pixel (the pixel that will be written to)
        uint8_t* dstpx = &dst[dst_row_size * y];

        // Bicubic interpolation weights for y-coordinate
        const float_type ty = srcyfrac - static_cast<float_type>(srcy);
        const float_type tty = ty * ty;
        const float_type ttty = tty * ty;

        const float_type qy[4] =
        {
            static_cast<float_type>(0.5) * (-ttty + static_cast<float_type>(2) * tty - ty),
            static_cast<float_type>(0.5) * (static_cast<float_type>(3) * ttty - static_cast<float_type>(5) * tty + static_cast<float_type>(2)),
            static_cast<float_type>(0.5)* (static_cast<float_type>(-3) * ttty + static_cast<float_type>(4) * tty + ty),
            static_cast<float_type>(0.5) * (ttty - tty)
        };

        // Loop over each column in the destination image
        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            // Map the horizontal offset back to the source image to figure out what column
            // should be sampled from
            // We offset by -0.5 to align with the center of the source pixels
            const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);
            const int srcx = static_cast<int>(srcxfrac);

            // Bicubic interpolation weights for x-coordinate
            const float_type tx = srcxfrac - static_cast<float_type>(srcx);
            const float_type ttx = tx * tx;
            const float_type tttx = ttx * tx;

            const float_type qx[4] =
            {
                static_cast<float_type>(0.5) * (-tttx + static_cast<float_type>(2) * ttx - tx),
                static_cast<float_type>(0.5) * (static_cast<float_type>(3) * tttx - static_cast<float_type>(5) * ttx + static_cast<float_type>(2)),
                static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * tttx + static_cast<float_type>(4) * ttx + tx),
                static_cast<float_type>(0.5) * (tttx - ttx)
            };

            // Array of pointers to the four neighboring pixels used in bicubic interpolation
            const pixel_type* pixels[4][4]{};

            // Populate the pixels array with neighboring pixels for interpolation
            for (int i = 0; i < 4; ++i)
            {
                int sy = std::clamp(srcy + i - 1, 0, srcymax);
                const uint8_t* srcrow = &src[src_row_size * sy];

                for (int j = 0; j < 4; ++j)
                {
                    int sx = std::clamp(srcx + j - 1, 0, srcxmax);
                    pixels[i][j] = reinterpret_cast<const pixel_type*>(&srcrow[sx * pixel_size]);
                }
            }

            pixel_type* dst_pixel = reinterpret_cast<pixel_type*>(dstpx);

            // Loop over each channel
            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bicubic interpolation using the sixteen neighboring pixels and weights
                const float_type px = (
                    (((*pixels[0][0] & channel_masks[c]) >> channel_shifts[c]) * qx[0] +
                     ((*pixels[0][1] & channel_masks[c]) >> channel_shifts[c]) * qx[1] +
                     ((*pixels[0][2] & channel_masks[c]) >> channel_shifts[c]) * qx[2] +
                     ((*pixels[0][3] & channel_masks[c]) >> channel_shifts[c]) * qx[3]) * qy[0] +

                    (((*pixels[1][0] & channel_masks[c]) >> channel_shifts[c]) * qx[0] +
                     ((*pixels[1][1] & channel_masks[c]) >> channel_shifts[c]) * qx[1] +
                     ((*pixels[1][2] & channel_masks[c]) >> channel_shifts[c]) * qx[2] +
                     ((*pixels[1][3] & channel_masks[c]) >> channel_shifts[c]) * qx[3]) * qy[1] +

                    (((*pixels[2][0] & channel_masks[c]) >> channel_shifts[c]) * qx[0] +
                     ((*pixels[2][1] & channel_masks[c]) >> channel_shifts[c]) * qx[1] +
                     ((*pixels[2][2] & channel_masks[c]) >> channel_shifts[c]) * qx[2] +
                     ((*pixels[2][3] & channel_masks[c]) >> channel_shifts[c]) * qx[3]) * qy[2] +

                    (((*pixels[3][0] & channel_masks[c]) >> channel_shifts[c]) * qx[0] +
                     ((*pixels[3][1] & channel_masks[c]) >> channel_shifts[c]) * qx[1] +
                     ((*pixels[3][2] & channel_masks[c]) >> channel_shifts[c]) * qx[2] +
                     ((*pixels[3][3] & channel_masks[c]) >> channel_shifts[c]) * qx[3]) * qy[3]
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