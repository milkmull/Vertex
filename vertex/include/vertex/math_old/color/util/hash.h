#pragma once

#include <functional>

#include "../../_priv/hash.h"
#include "../type/base/base_types.h"

namespace std {

template <typename T>
struct hash<vx::math::color_t<T>>
{
    VX_FORCE_INLINE constexpr size_t operator()(const vx::math::color_t<T>& c) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::_priv::hash_combine(seed, hasher(c.r));
        vx::math::_priv::hash_combine(seed, hasher(c.g));
        vx::math::_priv::hash_combine(seed, hasher(c.b));
        vx::math::_priv::hash_combine(seed, hasher(c.a));
        return seed;
    }
};

}