#pragma once

#include "./geometric.hpp"

namespace vx {
namespace math {

// https://math.stackexchange.com/questions/939229/unit-quaternion-to-a-scalar-power
// https://en.wikipedia.org/wiki/Quaternion#Functions_of_a_quaternion_variable

///////////////////////////////////////////////////////////////////////////////
// pow
///////////////////////////////////////////////////////////////////////////////

// q = a + bi + cj + dk = a + v 
// angle = arccos(a / |q|)
// pow(q, x) = pow(|q|, x) * { cos(x * angle) + (v / |v|) * sin(x * angle) }

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> pow(const quat_t<T>& q, T x) noexcept
{
    const vec<3, T> v = q.xyz();
    const T vmag = length(v);

    if (vmag <= constants<T>::epsilon)
    {
        // if q only has a real part, calculate the real part of the result
        if (q.w > static_cast<T>(0))
        {
            return quat_t<T>(
                pow(q.w, x),
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0)
            );
        }
        if (q.w < static_cast<T>(0))
        {
            // result could be imaginary if raised to non integer power

            const T qmag = pow(-q.w, x);
            const T angle = constants<T>::pi;
            const T xangle = x * angle;

            const T r = qmag * cos(xangle);
            const T i = qmag * sin(xangle);

            return quat_t<T>(
                r,
                i,
                static_cast<T>(0),
                static_cast<T>(0)
            );
        }

        // 0 ^ n
        if (x != static_cast<T>(0))
        {
            return quat_t<T>(
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0)
            );
        }

        // 0 ^ 0
        return quat_t<T>(
            constants<T>::nan,
            constants<T>::nan,
            constants<T>::nan,
            constants<T>::nan
        );
    }

    const T qmag = length(q);
    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T angle = acos_clamped(q.w * invqmag);
    const T xangle = x * angle;
    const T qmagx = pow(qmag, x);

    const T r = qmagx * cos(xangle);
    const vec<3, T> i = qmagx * (v * invvmag) * sin(xangle);

    return quat_t<T>(r, i);
}

///////////////////////////////////////////////////////////////////////////////
// exp
///////////////////////////////////////////////////////////////////////////////

// q = a + bi + cj + dk = a + v 
// exp(q) = exp(a) * { cos|v| + (v / |v|) * sin|q| }

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> exp(const quat_t<T>& q) noexcept
{
    const vec<3, T> v = q.xyz();
    const T vmag = length(v);

    if (vmag <= constants<T>::epsilon)
    {
        // if q only has a positive real part, take the exp like a real number
        return quat_t<T>(
            exp(q.w),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0)
        );
    }

    const T ea = exp(q.w);
    const T r = ea * cos(vmag);
    const vec<3, T> i = ea * v * (static_cast<T>(1) / vmag) * sin(vmag);

    return quat_t<T>(r, i);
}

///////////////////////////////////////////////////////////////////////////////
// log
///////////////////////////////////////////////////////////////////////////////

// q = a + bi + cj + dk = a + v 
// ln(q) = { ln|q| + (v / |v|) * arccos(a / |q|) }

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> log(const quat_t<T>& q) noexcept
{
    const T qmag = length(q);

    if (qmag <= constants<T>::epsilon)
    {
        // log(0) is undefined, follow standard
        return quat_t<T>(
            -constants<T>::infinity,
            -constants<T>::infinity,
            -constants<T>::infinity,
            -constants<T>::infinity
        );
    }

    const vec<3, T> v = q.xyz();
    const T vmag = length(v);

    if (vmag <= constants<T>::epsilon)
    {
        if (q.w > 0)
        {
            return quat_t<T>(
                log(q.w),
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0)
            );
        }
        else
        {
            return quat_t<T>(
                log(-q.w),
                constants<T>::pi,
                static_cast<T>(0),
                static_cast<T>(0)
            );
        }
    }

    const T invvmag = static_cast<T>(1) / vmag;
    const T invqmag = static_cast<T>(1) / qmag;

    const T r = log(qmag);
    const vec<3, T> i = v * invvmag * acos_clamped(q.w * invqmag);

    return quat_t<T>(r, i);
}

///////////////////////////////////////////////////////////////////////////////
// sqrt
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> sqrt(const quat_t<T>& q) noexcept
{
    return pow(q, static_cast<T>(0.5));
}

} // namespace math
} // namespace vx