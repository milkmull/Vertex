#pragma once

#include "image.h"
#include "vertex/math/math/type/vec4_type.h"
#include "vertex/math/texture/noise/perlin_noise.h"
#include "vertex/math/texture/noise/simplex_noise.h"
#include "vertex/math/texture/noise/cellular_noise.h"

namespace vx {
namespace img {

struct noise_sampler
{
    // =============== data ===============

    float frequency = 1.0f;
    float amplitude = 1.0f;
    size_t octaves = 1;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    math::vec4 seed = math::vec4(0.0f);

    // =============== perlin ===============

    /**
     * @brief Compute Perlin noise at the given UV coordinates.
     * @tparam L Dimensionality of the UV coordinates.
     * @param uv UV coordinates at which to sample the noise.
     * @return The Perlin noise value at the specified UV coordinates.
     */
    template <math::size_type L>
    float perlin_noise(const math::vec<L, float>& uv) const
    {
        using T = decltype(uv);
        using R = float;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&math::perlin_noise));
    }

    // =============== simplex ===============

    /**
     * @brief Compute simplex noise at the given UV coordinates.
     * @tparam L Dimensionality of the UV coordinates.
     * @param uv UV coordinates at which to sample the noise.
     * @return The simplex noise value at the specified UV coordinates.
     */
    template <math::size_type L>
    float simplex_noise(const math::vec<L, float>& uv) const
    {
        using T = decltype(uv);
        using R = float;

        return sample_internal<T, R>(uv, static_cast<R(*)(T)>(&math::simplex_noise));
    }

    // =============== cellular ===============

    /**
     * @brief Compute cellular noise at the given UV coordinates.
     * @tparam L Dimensionality of the UV coordinates (2 or 3).
     * @param uv UV coordinates at which to sample the noise.
     * @return The cellular noise values at the specified UV coordinates.
     */
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

        T uvs = uv + T(seed);

        for (size_t i = 0; i < octaves; ++i)
        {
            value += noise_func(uvs * v_frequency) * v_amplitude;

            v_frequency *= lacunarity;
            v_amplitude *= persistence;
        }

        return (value + 1.0f) * 0.5f;
    }

};

}
}