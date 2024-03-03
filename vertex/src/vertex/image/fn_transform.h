#pragma once

#include "image.h"
#include "enum_image_filter.h"
#include "raw/transform.h"
#include "raw/filter/filter_nearest.h"
#include "raw/filter/filter_bilinear.h"

namespace vx {
namespace img {

/**
 * @brief Flip the image horizontally (along the x-axis).
 *
 * @param img The input image to be flipped.
 * @return An image object representing the horizontally flipped version of the input image.
 */
inline image flip_x(const image& img)
{
    image out = img;
    raw::flip_x(out.data(), out.width(), out.height(), out.channels());
    return out;
}

/**
 * @brief Flip the image vertically (along the y-axis).
 *
 * @param img The input image to be flipped.
 * @return An image object representing the vertically flipped version of the input image.
 */
inline image flip_y(const image& img)
{
    image out = img;
    raw::flip_y(out.data(), out.width(), out.height(), out.channels());
    return out;
}

/**
 * @brief Rotate the image 90 degrees clockwise.
 *
 * @param img The input image to be rotated.
 * @return An image object representing the rotated version of the input image.
 */
inline image rotate_90_cw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    raw::rotate_90_cw(img.data(), img.width(), img.height(), out.data(), img.channels());
    return out;
}

/**
 * @brief Rotate the image 90 degrees counter-clockwise.
 *
 * @param img The input image to be rotated.
 * @return An image object representing the rotated version of the input image.
 */
inline image rotate_90_ccw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    raw::rotate_90_ccw(img.data(), img.width(), img.height(), out.data(), img.channels());
    return out;
}

/**
 * @brief Rotate the image 180 degrees.
 *
 * @param img The input image to be rotated.
 * @return An image object representing the rotated version of the input image.
 */
inline image rotate_180(const image& img)
{
    image out = img;
    raw::rotate_180(out.data(), out.width(), out.height(), out.channels());
    return out;
}

/**
 * @brief Crop a region of interest from the image.
 *
 * @param img The input image from which to crop.
 * @param area The rectangular area to be cropped, specified by a rectangle.
 * @return An image object representing the cropped region of interest.
 */
inline image crop(const image& img, const math::recti& area)
{
    const math::recti cropped = img.get_rect().crop(area);
    image out(cropped.size.x, cropped.size.y, img.format());

    const std::array<int32_t, 4> r{ cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y };
    raw::crop(img.data(), img.width(), img.height(), out.data(), img.channels(), r);

    return out;
}

/**
 * @brief Resize the image to a specified size using a specified filter.
 *
 * @param img The input image to be resized.
 * @param size The desired width and height of the resized image.
 * @param filter The interpolation filter to use for resizing.
 * @return An image object representing the resized version of the input image.
 */
inline image resize(const image& img, const math::vec2i& size, image_filter filter)
{
    image out(size.x, size.y, img.format());

    switch (filter)
    {
        case image_filter::NEAREST:
        {
            raw::filter_nearest(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        case image_filter::LINEAR:
        {
            raw::filter_bilinear(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        default:
        {
            break;
        }
    }

    return out;
}

/**
 * @brief Resize the image to a specified size, automatically selecting the interpolation filter based on pixel area change.
 *
 * If the resized image has fewer pixels than the original image, nearest neighbor interpolation is used.
 * If the resized image has more pixels than the original image, bilinear interpolation is used.
 *
 * @param img The input image to be resized.
 * @param size The desired width and height of the resized image.
 * @return An image object representing the resized version of the input image.
 */
inline image resize(const image& img, const math::vec2i& size)
{
    const float pixel_area = (
        (static_cast<float>(size.x) / static_cast<float>(img.width())) *
        (static_cast<float>(size.y) / static_cast<float>(img.height()))
    );

    const image_filter filter = (pixel_area < 1.0f) ? image_filter::NEAREST : image_filter::LINEAR;
    return resize(img, size, filter);
}

}
}