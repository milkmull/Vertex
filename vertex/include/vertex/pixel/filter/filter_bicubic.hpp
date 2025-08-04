#pragma once

#include <vector>

namespace vx {
namespace pixel {

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
inline void filter_bicubic(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t dst_width, size_t dst_height,
    size_t channels
)
{
    static_assert(std::is_arithmetic<channel_type>::value, "channel_type must be arithmatic type");
    static_assert(std::is_floating_point<float_type>::value, "float_type must be floating point type");

    constexpr float_type min = static_cast<float_type>(std::numeric_limits<channel_type>::min());
    constexpr float_type max = static_cast<float_type>(std::numeric_limits<channel_type>::max());

    if (!src || !dst)
    {
        return;
    }

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

    // Precompute weights
    std::vector<float_type> x_weights(dst_width * 4);
    for (size_t x = 0; x < dst_width; ++x)
    {
        const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);
        const int srcx = static_cast<int>(srcxfrac);

        const float_type tx = srcxfrac - static_cast<float_type>(srcx);
        const float_type ttx = tx * tx;
        const float_type tttx = ttx * tx;

        x_weights[(x * 4) + 0] = static_cast<float_type>(0.5) * (-tttx + static_cast<float_type>(2) * ttx - tx);
        x_weights[(x * 4) + 1] = static_cast<float_type>(0.5) * (static_cast<float_type>(3) * tttx - static_cast<float_type>(5) * ttx + static_cast<float_type>(2));
        x_weights[(x * 4) + 2] = static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * tttx + static_cast<float_type>(4) * ttx + tx);
        x_weights[(x * 4) + 3] = static_cast<float_type>(0.5) * (tttx - ttx);
    }

    std::vector<float_type> y_weights(dst_height * 4);
    for (size_t y = 0; y < dst_height; ++y)
    {
        const float_type srcyfrac = y * yscale - static_cast<float_type>(0.5);
        const int srcy = static_cast<int>(srcyfrac);

        const float_type ty = srcyfrac - static_cast<float_type>(srcy);
        const float_type tty = ty * ty;
        const float_type ttty = tty * ty;

        y_weights[(y * 4) + 0] = static_cast<float_type>(0.5) * (-ttty + static_cast<float_type>(2) * tty - ty);
        y_weights[(y * 4) + 1] = static_cast<float_type>(0.5) * (static_cast<float_type>(3) * ttty - static_cast<float_type>(5) * tty + static_cast<float_type>(2));
        y_weights[(y * 4) + 2] = static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * ttty + static_cast<float_type>(4) * tty + ty);
        y_weights[(y * 4) + 3] = static_cast<float_type>(0.5) * (ttty - tty);
    }

    const channel_type* pixels[4][4]{};
    float_type qx[4]{};
    float_type qy[4]{};

    for (size_t y = 0; y < dst_height; ++y)
    {
        const int srcy = static_cast<int>(y * yscale - static_cast<float_type>(0.5));

        uint8_t* dstpx = &dst[dst_row_size * y];
        qy[0] = y_weights[(y * 4) + 0];
        qy[1] = y_weights[(y * 4) + 1];
        qy[2] = y_weights[(y * 4) + 2];
        qy[3] = y_weights[(y * 4) + 3];

        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            const int srcx = static_cast<int>(x * xscale - static_cast<float_type>(0.5));

            qx[0] = x_weights[(x * 4) + 0];
            qx[1] = x_weights[(x * 4) + 1];
            qx[2] = x_weights[(x * 4) + 2];
            qx[3] = x_weights[(x * 4) + 3];

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

            channel_type* dst_pixel = reinterpret_cast<channel_type*>(dstpx);

            for (size_t c = 0; c < channels; ++c)
            {
                // Perform bicubic interpolation using the sixteen neighboring pixels and weights
                const float_type px = (
                    (pixels[0][0][c] * qx[0] + pixels[0][1][c] * qx[1] + pixels[0][2][c] * qx[2] + pixels[0][3][c] * qx[3]) * qy[0] +
                    (pixels[1][0][c] * qx[0] + pixels[1][1][c] * qx[1] + pixels[1][2][c] * qx[2] + pixels[1][3][c] * qx[3]) * qy[1] +
                    (pixels[2][0][c] * qx[0] + pixels[2][1][c] * qx[1] + pixels[2][2][c] * qx[2] + pixels[2][3][c] * qx[3]) * qy[2] +
                    (pixels[3][0][c] * qx[0] + pixels[3][1][c] * qx[1] + pixels[3][2][c] * qx[2] + pixels[3][3][c] * qx[3]) * qy[3]
                    );

                dst_pixel[c] = static_cast<channel_type>(std::clamp(px, min, max));
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
inline void filter_bicubic(
    const uint8_t* src, size_t src_width, size_t src_height,
    uint8_t* dst, size_t dst_width, size_t dst_height,
    size_t channels, const uint32_t* channel_masks, const uint32_t* channel_shifts
)
{
    static_assert(std::is_arithmetic<pixel_type>::value, "pixel_type must be arithmatic type");
    static_assert(std::is_floating_point<float_type>::value, "float_type must be floating point type");

    if (!src || !dst)
    {
        return;
    }

    if (!channel_masks || !channel_shifts)
    {
        return;
    }

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

    // Precompute weights
    std::vector<float_type> x_weights(dst_width * 4);
    for (size_t x = 0; x < dst_width; ++x)
    {
        const float_type srcxfrac = x * xscale - static_cast<float_type>(0.5);
        const int srcx = static_cast<int>(srcxfrac);

        const float_type tx = srcxfrac - static_cast<float_type>(srcx);
        const float_type ttx = tx * tx;
        const float_type tttx = ttx * tx;

        x_weights[(x * 4) + 0] = static_cast<float_type>(0.5) * (-tttx + static_cast<float_type>(2) * ttx - tx);
        x_weights[(x * 4) + 1] = static_cast<float_type>(0.5) * (static_cast<float_type>(3) * tttx - static_cast<float_type>(5) * ttx + static_cast<float_type>(2));
        x_weights[(x * 4) + 2] = static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * tttx + static_cast<float_type>(4) * ttx + tx);
        x_weights[(x * 4) + 3] = static_cast<float_type>(0.5) * (tttx - ttx);
    }

    std::vector<float_type> y_weights(dst_height * 4);
    for (size_t y = 0; y < dst_height; ++y)
    {
        const float_type srcyfrac = y * yscale - static_cast<float_type>(0.5);
        const int srcy = static_cast<int>(srcyfrac);

        const float_type ty = srcyfrac - static_cast<float_type>(srcy);
        const float_type tty = ty * ty;
        const float_type ttty = tty * ty;

        y_weights[(y * 4) + 0] = static_cast<float_type>(0.5) * (-ttty + static_cast<float_type>(2) * tty - ty);
        y_weights[(y * 4) + 1] = static_cast<float_type>(0.5) * (static_cast<float_type>(3) * ttty - static_cast<float_type>(5) * tty + static_cast<float_type>(2));
        y_weights[(y * 4) + 2] = static_cast<float_type>(0.5) * (static_cast<float_type>(-3) * ttty + static_cast<float_type>(4) * tty + ty);
        y_weights[(y * 4) + 3] = static_cast<float_type>(0.5) * (ttty - tty);
    }

    const pixel_type* pixels[4][4]{};
    float_type qx[4]{};
    float_type qy[4]{};

    for (size_t y = 0; y < dst_height; ++y)
    {
        const int srcy = static_cast<int>(y * yscale - static_cast<float_type>(0.5));

        uint8_t* dstpx = &dst[dst_row_size * y];
        qy[0] = y_weights[(y * 4) + 0];
        qy[1] = y_weights[(y * 4) + 1];
        qy[2] = y_weights[(y * 4) + 2];
        qy[3] = y_weights[(y * 4) + 3];

        for (size_t x = 0; x < dst_width; ++x, dstpx += pixel_size)
        {
            const int srcx = static_cast<int>(x * xscale - static_cast<float_type>(0.5));

            qx[0] = x_weights[(x * 4) + 0];
            qx[1] = x_weights[(x * 4) + 1];
            qx[2] = x_weights[(x * 4) + 2];
            qx[3] = x_weights[(x * 4) + 3];

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

} // namespace pixel
} // namespace vx