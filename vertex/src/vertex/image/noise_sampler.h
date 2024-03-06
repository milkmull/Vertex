#pragma once

#include "image.h"
#include "vertex/math/texture/noise/perlin_noise.h"
#include "vertex/math/texture/noise/simplex_noise.h"

namespace vx {
namespace img {

struct noise_sampler
{
    // =============== data ===============

    int seed = 0;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    size_t octaves = 1;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    // =============== sampling ===============

    float sample(float u, float v) const
    {
        return sample(math::vec2(u, v));
    }

    template <math::size_type L>
    float sample(const math::vec<L, float>& uv) const
    {
        float v_frequency = frequency;
        float v_amplitude = amplitude;
        float value = 0.0f;

        for (size_t i = 0; i < octaves; ++i)
        {
            value += math::simplex_noise(uv * v_frequency) * v_amplitude;

            v_frequency *= lacunarity;
            v_amplitude *= persistence;
        }

        return (value + 1.0f) * 0.5f;
    }

};

}
}