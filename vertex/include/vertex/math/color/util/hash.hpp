#pragma once

#include "vertex/config/hash.hpp"
#include "../types.hpp"

namespace std {

///////////////////////////////////////////////////////////////////////////////
// color
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct hash<vx::math::color_t<T>>
{
    size_t operator()(const vx::math::color_t<T>& c) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, c.r);
        hash_combine(seed, c.g);
        hash_combine(seed, c.b);
        hash_combine(seed, c.a);

        return seed;
    }
};

} // namespace std