#include "sandbox/sandbox.h"

#include "vertex/math/math.h"
#include "vertex/math/math/sdf.h"

int main()
{
    using namespace vx;

    img::image image(450, 450, img::image_format::RGBA8);
    image.fill(math::color::WHITE());

    using pixel_type = img::pixel_rgba8;

    const math::vec2 size(200);
    const math::vec2 b = size * 0.5f;
    const math::vec4 r(30.0f, 40.0f, 10.0f, 0.0f);
    constexpr float a = math::radians(0.0f);
    constexpr float a2 = math::radians(45.0f);

    for (auto it = image.begin<pixel_type>(); it != image.end<pixel_type>(); ++it)
    {
        auto p = math::rotate(it.local(), a2);

        float dist = math::sdf::sd_ellipse(p, math::vec2(-100, 100));

        if (dist < 0.0f)
        {
            *it = math::color::BLUE();
        }
    }

    img::write_png("../../tools/out.png", image.get_info(), image.raw_data(), true);

    return 0;
}