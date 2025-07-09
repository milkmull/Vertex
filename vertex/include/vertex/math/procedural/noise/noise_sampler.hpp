#pragma once

#include "./perlin_noise.hpp"
#include "./simplex_noise.hpp"
#include "./cellular_noise.hpp"

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
    VX_FORCE_INLINE f32 perlin_noise(const vec<L, f32>& uv) const noexcept
    {
        return sample_internal(uv, perlin_noise<L, f32>);
    }

    VX_FORCE_INLINE f32 perlin_noise(const vec2& uv) const noexcept { return perlin_noise<2>(uv); }
    VX_FORCE_INLINE f32 perlin_noise(const vec3& uv) const noexcept { return perlin_noise<3>(uv); }

    ///////////////////////////////////////////////////////////////////////////////
    // simplex
    ///////////////////////////////////////////////////////////////////////////////

    template <size_t L>
    VX_FORCE_INLINE f32 simplex_noise(const vec<L, f32>& uv) const noexcept
    {
        return sample_internal(uv, simplex_noise<L, f32>);
    }

    VX_FORCE_INLINE f32 simplex_noise(const vec2& uv) const noexcept { return simplex_noise<2>(uv); }
    VX_FORCE_INLINE f32 simplex_noise(const vec3& uv) const noexcept { return simplex_noise<3>(uv); }

    ///////////////////////////////////////////////////////////////////////////////
    // cellular
    ///////////////////////////////////////////////////////////////////////////////

    template <size_t L>
    VX_FORCE_INLINE vec2 cellular_noise(const vec<L, f32>& uv) const noexcept
    {
        VX_STATIC_ASSERT(L == 2 || L == 3, "cellular_noise only supports 2D or 3D vectors");
        return sample_internal(uv, cellular_noise<L, f32>);
    }

    VX_FORCE_INLINE vec2 cellular_noise(const vec2& uv) const noexcept { return cellular_noise<2>(uv); }
    VX_FORCE_INLINE vec2 cellular_noise(const vec3& uv) const noexcept { return cellular_noise<3>(uv); }

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