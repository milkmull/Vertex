#include "sandbox/sandbox.h"

#include "vertex/math/math.h"
#include "vertex/math/math/sdf.h"

int main()
{
    using namespace vx;

    img::image image(450, 450, img::image_format::RGBA8);
    image.fill(math::color::WHITE());

    using pixel_type = img::pixel_rgba8;

    /*
float rounded_box_SDF(vec2 localPosition, vec2 halfSize, float radius)
{
    vec2 q = abs(localPosition) - halfSize + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

void main()
{
    vec2 halfSize = (v_size - v_softness) * 0.5;
    float distance = rounded_box_SDF(v_localPosition, halfSize, v_cornerRadius);

    float smoothedAlpha = 1.0 - smoothstep(-v_softness, 0.0, distance);
    float borderAlpha = 1.0 - smoothstep(v_borderThickness - v_softness, v_borderThickness, abs(distance));

    color = mix(vec4(1.0, 1.0, 1.0, 0.0), v_color, borderAlpha * smoothedAlpha);
}
    */

    const math::vec2 size(200);
    const math::vec2 b = size * 0.5f;
    const math::vec4 r(30.0f, 40.0f, 10.0f, 0.0f);

    for (auto it = image.begin<pixel_type>(); it != image.end<pixel_type>(); ++it)
    {
        float box = math::sdf::sd_box(it.local(), b);
        float circ = math::sdf::sd_circle(it.local(), b.x);
        float box2 = math::sdf::sd_orientated_box(it.local(), math::vec2(200, 0), math::vec2(-200, 0), 25.0f);

        float dist = math::sdf::op_smooth_union(box, box2, 20.0f);

        if (dist < 0.0f)
        {
            *it = math::color::RED();
        }
    }

    img::write_png("../../tools/out.png", image.get_info(), image.raw_data());

    return 0;
}