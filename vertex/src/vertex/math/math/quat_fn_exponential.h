#pragma once

#include "detail/base_type_defs.h"
#include "quat_fn_comparison.h"
#include "fn_exponential.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// https://math.stackexchange.com/questions/939229/unit-quaternion-to-a-scalar-power
// https://en.wikipedia.org/wiki/Quaternion

// =============== pow ===============

// q = a + bi + cj + dk = a + v 
// angle = arccos(a / |q|)
// pow(q, x) = pow(|q|, x) * { cos(x * angle) + (v / |v|) * sin(x * angle) }

template <typename T>
inline constexpr detail::quat<T> pow(const detail::quat<T>& q, T x)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vec<3, T, detail::vec_t::vec>;

    const vec3_type v = q.vec();
    const T vmag = v.magnitude();

    if (vmag < math::epsilon<T>)
    {
        // if q only has a real part, calculate the realpart of the result
        if (q.w > static_cast<T>(0))
        {
            return quat_type(math::pow(q.w, x), 0, 0, 0);
        }
        if (q.w < static_cast<T>(0))
        {
            return quat_type(math::pow(-q.w, x) * math::cos(x * math::pi<T>), 0, 0, 0);
        }

        // 0 ^ n
        if (x != static_cast<T>(0))
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
    const T xangle = x * angle;
    const T qmagx = math::pow(qmag, x);

    const T r = qmagx * math::cos(xangle);
    const vec3_type i = qmagx * (v * invvmag) * math::sin(xangle);

    return quat_type(r, i);
}

// =============== exp ===============

// q = a + bi + cj + dk = a + v 
// exp(q) = exp(a) * { cos|v| + (v / |v|) * sin|q| }

template <typename T>
inline constexpr detail::quat<T> exp(const detail::quat<T>& q)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vec<3, T, detail::vec_t::vec>;

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

template <typename T>
inline constexpr detail::quat<T> log(const detail::quat<T>& q)
{
    using quat_type = detail::quat<T>;
    using vec3_type = detail::vec<3, T, detail::vec_t::vec>;

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

template <typename T>
inline constexpr detail::quat<T> sqrt(const detail::quat<T>& q)
{
    return math::pow(q, static_cast<T>(0.5));
}

}
}