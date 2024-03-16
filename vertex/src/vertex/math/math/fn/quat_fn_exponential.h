#pragma once

#include "quat_fn_geometric.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {

// https://math.stackexchange.com/questions/939229/unit-quaternion-to-a-scalar-power
// https://en.wikipedia.org/wiki/Quaternion#Functions_of_a_quaternion_variable

// =============== pow ===============

// q = a + bi + cj + dk = a + v 
// angle = arccos(a / |q|)
// pow(q, x) = pow(|q|, x) * { cos(x * angle) + (v / |v|) * sin(x * angle) }

/**
 * @brief Computes the quaternion raised to the power of a scalar.
 *
 * Special Cases:
 * 
 * - If the quaternion is zero (all components are zero) and the exponent is nonzero, returns a quaternion with all components set to zero.
 * - If the quaternion is zero and the exponent is zero, returns a quaternion with all components set to NaN.
 *
 * @tparam T The data type of the components of the quaternion.
 * @param q The quaternion to be raised to the power.
 * @param x The exponent to raise the quaternion to.
 * @return The quaternion raised to the power of the scalar.
 */
template <typename T>
inline constexpr quat_t<T> pow(const quat_t<T>& q, T x)
{
    const vec<3, T> v = q.vector();
    const T vmag = math::length(v);

    if (vmag < math::epsilon<T>)
    {
        // if q only has a real part, calculate the real part of the result
        if (q.w > static_cast<T>(0))
        {
            return quat_t<T>(math::pow(q.w, x), 0, 0, 0);
        }
        if (q.w < static_cast<T>(0))
        {
            return quat_t<T>(math::pow(-q.w, x) * math::cos(x * math::pi<T>), 0, 0, 0);
        }

        // 0 ^ n
        if (x != static_cast<T>(0))
        {
            return quat_t<T>(0, 0, 0, 0);
        }

        // 0 ^ 0
        return quat_t<T>(
            math::nan<T>,
            math::nan<T>,
            math::nan<T>,
            math::nan<T>
        );
    }

    const T qmag = math::length(q);

    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T angle = math::acos_clamped(q.w * invqmag);
    const T xangle = x * angle;
    const T qmagx = math::pow(qmag, x);

    const T r = qmagx * math::cos(xangle);
    const vec<3, T> i = qmagx * (v * invvmag) * math::sin(xangle);

    return quat_t<T>(r, i);
}

// =============== exp ===============

// q = a + bi + cj + dk = a + v 
// exp(q) = exp(a) * { cos|v| + (v / |v|) * sin|q| }

/**
 * @brief Computes the quaternion exponential function.
 * 
 * @tparam T The data type of the components of the quaternion.
 * @param q The quaternion for which to compute the exponential function.
 * @return The exponential of the quaternion.
 */
template <typename T>
inline constexpr quat_t<T> exp(const quat_t<T>& q)
{
    const vec<3, T> v = q.vector();
    const T vmag = math::length(v);

    if (vmag < math::epsilon<T>)
    {
        // if q only has a positive real part, take the exp like a real number
        return quat_t<T>(math::exp(q.w), 0, 0, 0);
    }

    const T ea = math::exp(q.w);

    const T r = ea * math::cos(vmag);
    const vec<3, T> i = ea * v * (static_cast<T>(1) / vmag) * math::sin(vmag);

    return quat_t<T>(r, i);
}

// =============== log ===============

// q = a + bi + cj + dk = a + v 
// ln(q) = { ln|q| + (v / |v|) * arccos(a / |q|) }

/**
 * @brief Computes the quaternion logarithm function.
 *
 * Special Cases:
 * 
 * - If the quaternion is zero (all components are zero), returns a quaternion with all components set to negative infinity.
 * 
 * @tparam T The data type of the components of the quaternion.
 * @param q The quaternion for which to compute the logarithm function.
 * @return The natural logarithm of the quaternion.
 */
template <typename T>
inline constexpr quat_t<T> log(const quat_t<T>& q)
{
    const vec<3, T> v = q.vector();
    const T vmag = math::length(v);

    if (vmag < math::epsilon<T>)
    {
        if (q.w > static_cast<T>(0))
        {
            // if q only has a positive real part, take the log like a real number
            return quat_t<T>(math::log(q.w), 0, 0, 0);
        }
        if (q.w < static_cast<T>(0))
        {
            // taking the log of a negative number is defined as:
            // ln(-a) = ln|a| + pi
            // where ln|a| is the real part and pi is the imaginary part
            return quat_t<T>(math::log(-q.w), math::pi<T>, 0, 0);
        }

        // log(0) is undefined
        return quat_t<T>(
            -math::infinity<T>,
            -math::infinity<T>,
            -math::infinity<T>,
            -math::infinity<T>
        );
    }

    const T qmag = math::length(q);

    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T r = math::log(qmag);
    const vec<3, T> i = v * invvmag * math::acos_clamped(q.w * invqmag);

    return quat_t<T>(r, i);
}

// =============== sqrt ===============

/**
 * @brief Computes the square root of a quaternion.
 *
 * Special Cases:
 *
 * - If the quaternion is zero (all components are zero) and the exponent is nonzero, returns a quaternion with all components set to zero.
 * - If the quaternion is zero and the exponent is zero, returns a quaternion with all components set to NaN.
 *
 * @tparam T The data type of the components of the quaternion.
 * @param q The quaternion for which to compute the square root.
 * @return The square root of the quaternion.
 */
template <typename T>
inline constexpr quat_t<T> sqrt(const quat_t<T>& q)
{
    return math::pow(q, static_cast<T>(0.5));
}

}
}