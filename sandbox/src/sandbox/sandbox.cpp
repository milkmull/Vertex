#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/pixel_iterator.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"

#include "vertex/math/math/util/to_string.h"

using namespace vx::math;

// Created by anatole duprat - XT95/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

#define iTime 1.0f

const mat2 m = mat2(0.80f, 0.60f, -0.60f, 0.80f);

float noise(vec2 p)
{
    return sin(p.x) * sin(p.y);
}

float fbm4(vec2 p)
{
    float f = 0.0f;
    f += 0.5000f * noise(p); p = m * p * 2.02f;
    f += 0.2500f * noise(p); p = m * p * 2.03f;
    f += 0.1250f * noise(p); p = m * p * 2.01f;
    f += 0.0625f * noise(p);
    return f / 0.9375f;
}

float fbm6(vec2 p)
{
    float f = 0.0f;
    f += 0.500000f * (0.5f + 0.5f * noise(p)); p = m * p * 2.02f;
    f += 0.250000f * (0.5f + 0.5f * noise(p)); p = m * p * 2.03f;
    f += 0.125000f * (0.5f + 0.5f * noise(p)); p = m * p * 2.01f;
    f += 0.062500f * (0.5f + 0.5f * noise(p)); p = m * p * 2.04f;
    f += 0.031250f * (0.5f + 0.5f * noise(p)); p = m * p * 2.01f;
    f += 0.015625f * (0.5f + 0.5f * noise(p));
    return f / 0.96875f;
}

vec2 fbm4_2(vec2 p)
{
    return vec2(fbm4(p), fbm4(p + vec2(7.8f)));
}

vec2 fbm6_2(vec2 p)
{
    return vec2(fbm6(p + vec2(16.8f)), fbm6(p + vec2(11.5f)));
}

//====================================================================

float func(vec2 q, vec4& ron)
{
    q += 0.03f * sin(vec2(0.27f, 0.23f) * iTime + length(q) * vec2(4.1f, 4.3f));

    vec2 o = fbm4_2(0.9f * q);

    o += 0.04f * sin(vec2(0.12f, 0.14f) * iTime + length(o));

    vec2 n = fbm6_2(3.0f * o);

    ron = vec4(o, n);

    float f = 0.5f + 0.5f * fbm4(1.8f * q + 6.0f * n);

    return mix(f, f * f * f * 3.5f, f * abs(n.x));
}

int main()
{
    using namespace vx;

    img::image i(800, 450, img::image_format::RGBA8);

    for (auto it = img::begin<img::pixel_rgba8>(i); it != img::end<img::pixel_rgba8>(i); ++it)
    {
        vec2 p = (2.0f * it.xy() - it.resolution()) / it.resolution().y;
        float e = 2.0f / it.resolution().y;

        vec4 on = vec4(0.0f);
        float f = func(p, on);

        vec3 col = vec3(0.0f);
        col = mix(vec3(0.2f, 0.1f, 0.4f), vec3(0.3f, 0.05f, 0.05f), f);
        col = mix(col, vec3(0.9f, 0.9f, 0.9f), dot(vec2(on.z, on.w), vec2(on.z, on.w)));
        col = mix(col, vec3(0.4f, 0.3f, 0.3f), 0.2f + 0.5f * on.y * on.y);
        col = mix(col, vec3(0.0f, 0.2f, 0.4f), 0.5f * smoothstep(1.2f, 1.3f, abs(on.z) + abs(on.w)));
        col = clamp(col * f * 2.0f, 0.0f, 1.0f);

        // manual derivatives - better quality, but slower
        vec4 kk;
        vec3 nor = normalize(vec3(func(p + vec2(e, 0.0f), kk) - f,
            2.0f * e,
            func(p + vec2(0.0f, e), kk) - f));

        vec3 lig = normalize(vec3(0.9f, 0.2f, -0.4f));
        float dif = clamp(0.3f + 0.7f * dot(nor, lig), 0.0f, 1.0f);
        vec3 lin = vec3(0.70f, 0.90f, 0.95f) * (nor.y * 0.5f + 0.5f) + vec3(0.15f, 0.10f, 0.05f) * dif;
        col *= 1.2f * lin;
        col = 1.0f - col;
        col = 1.1f * col * col;

        *it = color(col, 1.0f);
    }

    img::error_code err;
    err = img::write_png("../../assets/swirl.png", i);

    std::cout << (int)err;

    return 0;
}