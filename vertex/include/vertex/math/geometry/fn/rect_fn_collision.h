#pragma once

#include "../type/base/base_types.h"
#include "../../math/fn/fn_comparison.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if one rectangle is completely contained within another
/// rectangle.
///
/// @tparam T1 The type of the first rectangle's components.
/// @tparam T2 The type of the second rectangle's components.
/// @param r1 The first rectangle to check.
/// @param r2 The second rectangle to check against.
/// 
/// @return True if r2 completely contains r1, otherwise false.
///////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
VX_FORCE_INLINE constexpr bool contains(const rect_t<T1>& r1, const rect_t<T2>& r2)
{
    return r1.position.x <= r2.position.x
        && r1.position.x + r1.size.x > r2.position.x + r2.size.x
        && r1.position.y <= r2.position.y
        && r1.position.y + r1.size.y > r2.position.y + r2.size.y;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if a rectangle contains a point.
///
/// @tparam T1 The type of the rectangle's components.
/// @tparam T2 The type of the point's components.
/// @param r The rectangle to check against.
/// @param p The point to check for containment.
/// 
/// @return True if the point is within the rectangle, otherwise false.
///////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
VX_FORCE_INLINE constexpr bool contains(const rect_t<T1>& r, const vec<2, T2>& p)
{
    return r.position.x <= p.x
        && r.position.x + r.size.x > p.x
        && r.position.y <= p.y
        && r.position.y + r.size.y > p.y;
}

///////////////////////////////////////////////////////////////////////////////
// overlaps
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if two rectangles overlap.
///
/// @tparam T1 The type of the first rectangle's components.
/// @tparam T2 The type of the second rectangle's components.
/// @param r1 The first rectangle to check.
/// @param r2 The second rectangle to check against.
/// 
/// @return True if the rectangles overlap, otherwise false.
///////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
VX_FORCE_INLINE constexpr bool overlaps(const rect_t<T1>& r1, const rect_t<T2>& r2)
{
    return r1.position.x <= r2.position.x + r2.size.x
        && r1.position.x + r1.size.x > r2.position.x
        && r1.position.y <= r2.position.y + r2.size.y
        && r1.position.y + r1.size.y > r2.position.y;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if a rectangle overlaps a point.
///
/// @tparam T1 The type of the rectangle's components.
/// @tparam T2 The type of the point's components.
/// @param r The rectangle to check against.
/// @param p The point to check for overlap.
/// 
/// @return True if the point overlaps with the rectangle, otherwise false.
///////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
VX_FORCE_INLINE constexpr bool overlaps(const rect_t<T1>& r, const vec<2, T2>& p)
{
    return contains(r, p);
}

}
}