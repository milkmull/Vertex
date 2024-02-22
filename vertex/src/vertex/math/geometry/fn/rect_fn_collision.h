#pragma once

#include "../detail/base_types.h"
#include "../../math/fn/fn_comparison.h"

namespace vx {
namespace math {

// =============== contains ===============

template <typename T>
inline constexpr bool contains(const rect_t<T>& r1, const rect_t<T>& r2)
{
    return r1.position.x <= r2.position.x
        && r1.position.x + r1.size.x >= r2.position.x + r2.size.x
        && r1.position.y <= r2.position.y
        && r1.position.y + r1.size.y >= r2.position.y + r2.size.y;
}

template <typename T>
inline constexpr bool contains(const rect_t<T>& r, const vec<2, T>& p)
{
    return r.position.x <= p.x
        && r.position.x + r.size.x >= p.x
        && r.position.y <= p.y
        && r.position.y + r.size.y >= p.y;
}

// =============== overlaps ===============

template <typename T>
inline constexpr bool overlaps(const rect_t<T>& r1, const rect_t<T>& r2)
{
    return r1.position.x <= r2.position.x + r2.size.x
        && r1.position.x + r1.size.x >= r2.position.x
        && r1.position.y <= r2.position.y + r2.size.y
        && r1.position.y + r1.size.y >= r2.position.y;
}

template <typename T>
inline constexpr bool overlaps(const rect_t<T>& r, const vec<2, T>& p)
{
    return r.position.x <= p.x
        && r.position.x + r.size.x >= p.x
        && r.position.y <= p.y
        && r.position.y + r.size.y >= p.y;
}

}
}