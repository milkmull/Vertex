#include "color.h"

#include "image.h"

namespace vx {
namespace img {

void fill(image& img, const math::color& color)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, color);
        }
    }
}

void invert(image& img)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, img.get_pixel(x, y).invert());
        }
    }
}

void replace_color(image& img, const math::color& color1, const math::color& color2, float epsilon)
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

void swap_color(image& img, const math::color& color1, const math::color& color2, float epsilon)
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

void key(image& img, const math::color& color, float threshold)
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