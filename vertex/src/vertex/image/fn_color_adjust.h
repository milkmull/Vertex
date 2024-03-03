#pragma once

#include "image.h"

namespace vx {
namespace img {

/**
 * @brief Fill the entire image with the specified color.
 *
 * @param img The image to be filled.
 * @param color The color to fill the image with.
 */
inline void fill(image& img, const math::color& color)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, color);
        }
    }
}

/**
 * @brief Invert the colors of the entire image.
 *
 * @param img The image to be inverted.
 */
inline void invert(image& img)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, img.get_pixel(x, y).invert());
        }
    }
}

/**
 * @brief Replace all occurrences of one color in the image with another color within a given threshold.
 *
 * @param img The image to be modified.
 * @param color1 The color to be replaced.
 * @param color2 The color to replace with.
 * @param epsilon The threshold for color comparison.
 */
inline void replace_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            if (math::is_equal_approx(img.get_pixel(x, y), color1, epsilon))
            {
                img.set_pixel(x, y, color2);
            }
        }
    }
}

/**
 * @brief Swap all occurrences of two colors in the image within a given threshold.
 *
 * @param img The image to be modified.
 * @param color1 The first color to be swapped.
 * @param color2 The second color to be swapped.
 * @param epsilon The threshold for color comparison.
 */
inline void swap_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            if (math::is_equal_approx(img.get_pixel(x, y), color1, epsilon))
            {
                img.set_pixel(x, y, color2);
            }
            else if (math::is_equal_approx(img.get_pixel(x, y), color2, epsilon))
            {
                img.set_pixel(x, y, color1);
            }
        }
    }
}

/**
 * @brief Set the alpha channel to zero for all pixels in the image that match the specified color within a given threshold.
 *
 * @param img The image to be modified.
 * @param color The color to be keyed out.
 * @param threshold The threshold for color comparison.
 */
inline void key(image& img, const math::color& color, float threshold = math::epsilon<float>)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            math::color c = img.get_pixel(x, y);
            if (math::is_equal_approx(c, color, threshold))
            {
                c.a = 0.0f;
                img.set_pixel(x, y, c);
            }
        }
    }
}

}
}