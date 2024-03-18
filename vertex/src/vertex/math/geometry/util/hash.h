#pragma once

#include <functional>

#include "../../detail/hash.h"
#include "../type/base/base_types.h"

namespace std {

template <typename T>
struct hash<vx::math::rect_t<T>>
{
    VX_FORCE_INLINE constexpr size_t operator()(const vx::math::rect_t<T>& r) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(r.position.x));
        vx::math::detail::hash_combine(seed, hasher(r.position.y));
        vx::math::detail::hash_combine(seed, hasher(r.size.x));
        vx::math::detail::hash_combine(seed, hasher(r.size.y));
        return seed;
    }
};

}