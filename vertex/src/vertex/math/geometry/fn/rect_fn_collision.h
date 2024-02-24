#pragma once

#include "../detail/base_types.h"
#include "../../math/fn/fn_comparison.h"

namespace vx {
namespace math {

// =============== contains ===============

/**
 * @brief Checks if one rectangle is completely contained within another rectangle.
 *
 * @tparam T The type of the rectangles' components.
 * @param r1 The first rectangle to check.
 * @param r2 The second rectangle to check against.
 * @return True if r2 completely contains r1, otherwise false.
 */
template <typename T>
inline constexpr bool contains(const rect_t<T>& r1, const rect_t<T>& r2)
{
    return r1.position.x <= r2.position.x
        && r1.position.x + r1.size.x >= r2.position.x + r2.size.x
        && r1.position.y <= r2.position.y
        && r1.position.y + r1.size.y >= r2.position.y + r2.size.y;
}

/**
 * @brief Checks if a point is contained within a rectangle.
 *
 * @tparam T The type of the rectangle's components.
 * @param r The rectangle to check against.
 * @param p The point to check for containment.
 * @return True if the point is within the rectangle, otherwise false.
 */
template <typename T>
inline constexpr bool contains(const rect_t<T>& r, const vec<2, T>& p)
{
    return r.position.x <= p.x
        && r.position.x + r.size.x >= p.x
        && r.position.y <= p.y
        && r.position.y + r.size.y >= p.y;
}

// =============== overlaps ===============

/**
 * @brief Checks if two rectangles overlap.
 *
 * @tparam T The type of the rectangles' components.
 * @param r1 The first rectangle to check.
 * @param r2 The second rectangle to check against.
 * @return True if the rectangles overlap, otherwise false.
 */
template <typename T>
inline constexpr bool overlaps(const rect_t<T>& r1, const rect_t<T>& r2)
{
    return r1.position.x <= r2.position.x + r2.size.x
        && r1.position.x + r1.size.x >= r2.position.x
        && r1.position.y <= r2.position.y + r2.size.y
        && r1.position.y + r1.size.y >= r2.position.y;
}

/**
 * @brief Checks if a point overlaps with a rectangle.
 *
 * @tparam T The type of the rectangle's components.
 * @param r The rectangle to check against.
 * @param p The point to check for overlap.
 * @return True if the point overlaps with the rectangle, otherwise false.
 */
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