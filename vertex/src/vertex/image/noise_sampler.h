#pragma once

#include "image.h"
#include "vertex/math/texture/noise/perlin_noise.h"
#include "vertex/math/texture/noise/simplex_noise.h"
#include "vertex/math/texture/noise/cellular_noise.h"

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

    template <math::size_type L>
    float perlin_noise(const math::vec<L, float>& uv) const
    {
        using T = decltype(uv);
        using R = float;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&math::perlin_noise));
    }

    template <math::size_type L>
    float simplex_noise(const math::vec<L, float>& uv) const
    {
        using T = decltype(uv);
        using R = float;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&math::simplex_noise));
    }

    template <math::size_type L, typename std::enable_if<(L == 2 || L == 3), bool>::type = true>
    math::vec2 cellular_noise(const math::vec<L, float>& uv) const
    {
        using T = decltype(uv);
        using R = math::vec2;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&math::cellular_noise));
    }

private:

    template <typename T, typename R>
    R sample_internal(const T& uv, R(*noise_func)(T)) const
    {
        float v_frequency = frequency;
        float v_amplitude = amplitude;
        R value{};

        for (size_t i = 0; i < octaves; ++i)
        {
            value += noise_func(uv * v_frequency) * v_amplitude;

            v_frequency *= lacunarity;
            v_amplitude *= persistence;
        }

        return (value + 1.0f) * 0.5f;
    }


};

}
}