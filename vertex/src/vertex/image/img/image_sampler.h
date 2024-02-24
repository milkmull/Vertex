#pragma once

#include "image.h"

namespace vx {
namespace img {

class image_sampler
{
public:
    
    image_sampler(image& image)
        : m_image(image), m_area(image.get_rect()) {}

    image_sampler(image& image, const math::rectu& area)
        : m_image(image), m_area(m_image.get_rect().crop(area)) {}

    image_sampler(const image_sampler&) = default;
    image_sampler(image_sampler&&) noexcept = default;
    ~image_sampler() = default;

    image_sampler& operator=(const image_sampler&) = default;
    image_sampler& operator=(image_sampler&&) noexcept = default;

private:

    image& m_image;
    math::rectu m_area;

};

}
}