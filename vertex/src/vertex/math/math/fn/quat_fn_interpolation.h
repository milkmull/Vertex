#pragma once

#include "../detail/base_types.h"
#include "quat_fn_geometric.h"

namespace vx {
namespace math {

// =============== lerp ===============

template <typename T>
inline constexpr quat_t<T> lerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== slerp ===============

// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
// assuming qx and qy are normalized
// theta = arccos(dot(qx, qy))
// qm = { qx * sin((1 - t) * theta) + qy * sin(t * theta) } / sin(theta)

/**
 * @brief Performs spherical linear interpolation (SLERP) between two quaternions.
 *
 * This function performs spherical linear interpolation (SLERP) between two quaternions.
 *
 * @tparam T Type of the quaternion components.
 * @param x The starting quaternion.
 * @param y The ending quaternion.
 * @param t Interpolation parameter. Should be in the range [0, 1].
 * @return The interpolated quaternion.
 * 
 * @ref https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
 */
template <typename T>
inline constexpr quat_t<T> slerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
)
{
    T cos_alpha = math::normalized_dot(x, y);
    T xsign = static_cast<T>(1);

    if (cos_alpha < static_cast<T>(0))
    {
        // Since q(w,x,y,z) and q(-w,-x,-y,-z) represent the same
        // rotation we should make sure the result is not sensitive 
        // to whether the positive or inverted form of the
        // quaternion is used.
        // 
        // acos will return an angle between:
        // 
        // 0-90 degrees for positive inputs
        // 90-180 degrees for negative inputs
        // 
        // To ensure acos returns the smallest angle (or shortest
        // path around the hypersphere), if cos_alpha is negative,
        // we make it positive. To account for this, we also have
        // to negate one of the input quaternions to make the new
        // angle accurate.
        // 
        // It doesn't matter which quaternion we invert as the
        // result will still be the same.

        xsign = static_cast<T>(-1);
        cos_alpha = -cos_alpha;
    }

    if (cos_alpha > static_cast<T>(1) - epsilon<T>)
    {
        // If the angle between the quaternions is super small, we
        // can estimate with linear interpolation. This also helps
        // to avoid dividing by 0 later since acos(1) == sin(0) == 0.

        return lerp(xsign * x, y, t);
    }

    const T alpha = math::acos(cos_alpha);
    const T sin_alpha = math::sin(alpha);
    const T inv_sin_alpha = static_cast<T>(1) / sin_alpha;

    const T t1 = math::sin((static_cast<T>(1) - t) * alpha) * inv_sin_alpha;
    const T t2 = math::sin(t * alpha) * inv_sin_alpha;

    return (xsign * x * t1) + (y * t2);
}

}
}