#pragma once

namespace vx {
namespace math {

// =============== scaler ===============

template <typename T>
inline constexpr T catmull_rom(
    T p1,
    T p2,
    T p3,
    T p4,
    T s
)
{
    const T s2 = s * s;
    const T s3 = s2 * s;

    const T f1 = -s3 + static_cast<T>(2) * s2 - s;
    const T f2 = static_cast<T>(3) * s3 - static_cast<T>(5) * s2 + static_cast<T>(2);
    const T f3 = static_cast<T>(-3) * s3 + static_cast<T>(4) * s2 + s;
    const T f4 = s3 - s2;

    return (f1 * p1 + f2 * p2 + f3 * p3 + f4 * p4) / static_cast<T>(2);
}

template <typename T>
inline constexpr T hermite(
    T p1,
    T p2,
    T m1,
    T m2,
    T s
)
{
    const T s2 = s * s;
    const T s3 = s2 * s;

    const T f1 = static_cast<T>(2) * s3 - static_cast<T>(3) * s2 + static_cast<T>(1);
    const T f2 = static_cast<T>(-2) * s3 + static_cast<T>(3) * s2;
    const T f3 = s3 - static_cast<T>(2) * s2 + s;
    const T f4 = s3 - s2;

    return (f1 * p1 + f2 * p2 + f3 * m1 + f4 * m2) / static_cast<T>(2);
}

template <typename T>
inline constexpr T cubic(
    T p1,
    T p2,
    T p3,
    T p4,
    T s
)
{
    return ((p1 * s + p2) * s + p3) * s + p4;
}

}
}