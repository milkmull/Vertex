#pragma once

#include "detail/base_type_defs.h"
#include "quat_fn_comparison.h"
#include "fn_exponential.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// https://math.stackexchange.com/questions/939229/unit-quaternion-to-a-scalar-power
// https://en.wikipedia.org/wiki/Quaternion#Functions_of_a_quaternion_variable

// =============== pow ===============

// q = a + bi + cj + dk = a + v 
// angle = arccos(a / |q|)
// pow(q, x) = pow(|q|, x) * { cos(x * angle) + (v / |v|) * sin(x * angle) }

/**
 * @brief Calculates the quaternion raised to the power of a scalar value.
 *
 * This function computes the result of a quaternion 'q' raised to the power of 'y'. The result is a new quaternion.
 * Special cases:
 * - If the input quaternion is a zero quaternion (all components are zero), the result is a quaternion with zero components.
 * - If 'y' is 0, the result is a quaternion with all components set to NaN.
 * 
 * @param q The input quaternion.
 * @param y The scalar exponent.
 * @return The resulting quaternion.
 */
template <typename T>
inline constexpr detail::quat<T> pow(const detail::quat<T>& q, T y)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vecx<3, T>;

    const vec3_type v = q.vec();
    const T vmag = v.magnitude();

    if (vmag < math::epsilon<T>)
    {
        // if q only has a real part, calculate the real part of the result
        if (q.w > static_cast<T>(0))
        {
            return quat_type(math::pow(q.w, y), 0, 0, 0);
        }
        if (q.w < static_cast<T>(0))
        {
            return quat_type(math::pow(-q.w, y) * math::cos(y * math::pi<T>), 0, 0, 0);
        }

        // 0 ^ n
        if (y != static_cast<T>(0))
        {
            return quat_type(0, 0, 0, 0);
        }

        // 0 ^ 0
        return quat_type(
            math::nan<T>,
            math::nan<T>,
            math::nan<T>,
            math::nan<T>
        );
    }

    const T qmag = q.magnitude();

    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T angle = math::acos_clamped(q.w * invqmag);
    const T xangle = y * angle;
    const T qmagx = math::pow(qmag, y);

    const T r = qmagx * math::cos(xangle);
    const vec3_type i = qmagx * (v * invvmag) * math::sin(xangle);

    return quat_type(r, i);
}

// =============== exp ===============

// q = a + bi + cj + dk = a + v 
// exp(q) = exp(a) * { cos|v| + (v / |v|) * sin|q| }

/**
 * @brief Calculates the quaternion exponential of the given quaternion.
 * 
 * This function computes the result of e raised to a quaternion power 'q'. The result is a new quaternion.
 * 
 * @param q The input quaternion.
 * @return The quaternion exponential of the input quaternion.
 */
template <typename T>
inline constexpr detail::quat<T> exp(const detail::quat<T>& q)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vecx<3, T>;

    const vec3_type v = q.vec();
    const T vmag = v.magnitude();

    if (vmag < math::epsilon<T>)
    {
        // if q only has a positive real part, take the exp like a real number
        return quat_type(math::exp(q.w), 0, 0, 0);
    }

    const T ea = math::exp(q.w);

    const T r = ea * math::cos(vmag);
    const vec3_type i = ea * v * (static_cast<T>(1) / vmag) * math::sin(vmag);

    return quat_type(r, i);
}

// =============== log ===============

// q = a + bi + cj + dk = a + v 
// ln(q) = { ln|q| + (v / |v|) * arccos(a / |q|) }

/**
 * @brief Calculates the quaternion natural logarithm of the given quaternion.
 *
 * This function computes the natural logorithm of a quaternion 'q'. The result is a new quaternion.
 * Special cases:
 * - If the input quaternion is a zero quaternion (all components are zero), the operation is undefined.
 *
 * @param q The input quaternion.
 * @return The quaternion natural logarithm of the input quaternion.
 */
template <typename T>
inline constexpr detail::quat<T> log(const detail::quat<T>& q)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vecx<3, T>;

    const vec3_type v = q.vec();
    const T vmag = v.magnitude();

    if (vmag < math::epsilon<T>)
    {
        if (q.w > static_cast<T>(0))
        {
            // if q only has a positive real part, take the log like a real number
            return quat_type(math::log(q.w), 0, 0, 0);
        }
        if (q.w < static_cast<T>(0))
        {
            // taking the log of a negative number is defined as:
            // ln(-a) = ln|a| + pi
            // where ln|a| is the real part and pi is the imaginary part
            return quat_type(math::log(-q.w), math::pi<T>, 0, 0);
        }

        // log(0) is undefined
        return quat_type(
            -math::infinity<T>,
            -math::infinity<T>,
            -math::infinity<T>,
            -math::infinity<T>
        );
    }

    const T qmag = q.magnitude();

    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T r = math::log(qmag);
    const vec3_type i = v * invvmag * math::acos_clamped(q.w * invqmag);

    return quat_type(r, i);
}

// =============== sqrt ===============

/**
 * @brief Calculates the square root of a quaternion.
 *
 * This function computes the square root of a quaternion 'q'. The result is a new quaternion.
 * Special cases:
 * - If the input quaternion is a zero quaternion (all components are zero), the result is a quaternion with zero components.
 *
 * @param q The input quaternion.
 * @return The resulting quaternion.
 */
template <typename T>
inline constexpr detail::quat<T> sqrt(const detail::quat<T>& q)
{
    return math::pow(q, static_cast<T>(0.5));
}

}
}