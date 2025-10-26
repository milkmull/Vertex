#pragma once

#include "vertex/math/procedural/noise/perlin_noise.hpp"
#include "vertex/math/procedural/noise/simplex_noise.hpp"
#include "vertex/math/procedural/noise/cellular_noise.hpp"

namespace vx {
namespace math {

struct noise_sampler
{
    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    f32 frequency = 1.0f;
    f32 amplitude = 1.0f;
    size_t octaves = 1;
    f32 persistence = 0.5f;
    f32 lacunarity = 2.0f;

    vec4 seed = vec4(0.0f);
    bool normalize = true;

    ///////////////////////////////////////////////////////////////////////////////
    // perlin
    ///////////////////////////////////////////////////////////////////////////////

    template <size_t L>
    f32 perlin_noise(const vec<L, f32>& uv) const
    {
        using T = decltype(uv);
        using R = f32;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&::vx::math::perlin_noise));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simplex
    ///////////////////////////////////////////////////////////////////////////////

    template <size_t L>
    f32 simplex_noise(const vec<L, f32>& uv) const
    {
        using T = decltype(uv);
        using R = f32;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&::vx::math::simplex_noise));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // cellular
    ///////////////////////////////////////////////////////////////////////////////

    template <size_t L, VX_MATH_REQ(L == 2 || L == 3)>
    vec2 cellular_noise(const vec<L, f32>& uv) const
    {
        using T = decltype(uv);
        using R = vec2;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&::vx::math::cellular_noise));
    }

private:

    template <typename T, typename R>
    VX_FORCE_INLINE R sample_internal(const T& uv, R(*noise_func)(T)) const noexcept
    {
        f32 v_frequency = frequency;
        f32 v_amplitude = amplitude;
        R value{};

        const T uvs = uv + T(seed);

        for (size_t i = 0; i < octaves; ++i)
        {
            value += noise_func(uvs * v_frequency) * v_amplitude;

            v_frequency *= lacunarity;
            v_amplitude *= persistence;
        }

        if (normalize)
        {
            value = (value + static_cast<R>(1)) * static_cast<R>(0.5f);
        }

        return value;
    }
};

} // namespace math
} // namespace vx