#pragma once

#include "detail/base_type_defs.h"
#include "fn_exponential.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// =============== quaternion ===============

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_(in_3-2-1_sequence)_to_quaternion_conversion
// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf
// https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/euler_angles.inl

// quat_from_euler

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_XYZ(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_XZY(T t1, T t3, T t2)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1),
        -(st1 * st2 * ct3) + (st1 * ct1 * ct2),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_XYX(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        +ct2 * math::sin(t1p3),
        +st2 * math::cos(t1m3),
        +st2 * math::sin(t1m3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_XZX(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        +ct2 * math::sin(t1p3),
        -st2 * math::sin(t1m3),
        +st2 * math::cos(t1m3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_YXZ(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1),
        -(st1 * st2 * ct3) + (st3 * ct1 * ct2)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_YZX(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct2)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_YXY(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::cos(t1m3),
        +ct2 * math::sin(t1p3),
        -st2 * math::sin(t1m3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_YZY(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::sin(t1m3),
        +ct2 * math::sin(t1p3),
        +st2 * math::cos(t1m3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_ZXY(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_ZYX(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::quat<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        -(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::cos(t1m3),
        +st2 * math::sin(t1m3),
        +ct2 * math::sin(t1p3)
    );
}

template <typename T>
inline constexpr detail::quat<T> quat_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return detail::quat<T>(
        +ct2 * math::cos(t1p3),
        -st2 * math::sin(t1m3),
        +st2 * math::cos(t1m3),
        +ct2 * math::sin(t1p3)
    );
}

// quat_to_euler

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_XYZ(const detail::quat<T>& q)
{
    return mat3_to_euler_XYZ(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_XZY(const detail::quat<T>& q)
{
    return mat3_to_euler_XZY(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_XYX(const detail::quat<T>& q)
{
    return mat3_to_euler_XYX(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_XZX(const detail::quat<T>& q)
{
    return mat3_to_euler_XZX(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_YXZ(const detail::quat<T>& q)
{
    return mat3_to_euler_YXZ(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_YZX(const detail::quat<T>& q)
{
    return mat3_to_euler_YZX(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_YXY(const detail::quat<T>& q)
{
    return mat3_to_euler_YXY(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_YZY(const detail::quat<T>& q)
{
    return mat3_to_euler_YZY(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_ZXY(const detail::quat<T>& q)
{
    return mat3_to_euler_ZXY(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_ZYX(const detail::quat<T>& q)
{
    return mat3_to_euler_ZYX(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_ZXZ(const detail::quat<T>& q)
{
    return mat3_to_euler_ZXZ(detail::mat3x<T>(q));
}

template <typename T>
inline constexpr detail::vecx<3, T> quat_to_euler_ZYZ(const detail::quat<T>& q)
{
    return mat3_to_euler_ZYZ(detail::mat3x<T>(q));
}

// =============== mat3 ===============

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

// mat3_from_euler

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_XYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),

        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),

        +(st2),
        -(st1 * ct2),
        +(ct1 * ct2)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_XZY(T t1, T t3, T t2)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),

        -(st2),
        +(ct1 * ct2),
        +(st1 * ct2),

        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_XYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct2),
        +(st1 * st2),
        -(ct1 * st2),

        +(st2 * st3),
        +(ct1 * ct3) - (st1 * ct2 * st3),
        +(st1 * ct3) + (ct1 * ct2 * st3),

        +(st2 * ct3),
        -(ct1 * st3) - (st1 * ct2 * ct3),
        -(st1 * st3) + (ct1 * ct2 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_XZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct2),
        +(ct1 * st2),
        +(st1 * st2),

        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),

        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_YXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),

        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),

        +(st1 * ct2),
        -(st2),
        +(ct1 * ct2)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_YZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct1 * ct2),
        +(st2),
        -(st1 * ct2),

        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),

        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_YXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),

        +(st1 * st2),
        +(ct2),
        +(ct1 * st2),

        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_YZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        -(st1 * ct2 * ct3) - (ct1 * st3),

        -(ct1 * st2),
        +(ct2),
        +(st1 * st2),

        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        -(st1 * ct2 * st3) + (ct1 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_ZXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),

        -(st1 * ct2),
        +(ct1 * ct2),
        +(st2),

        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_ZYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct1 * ct2),
        +(st1 * ct2),
        -(st2),

        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),

        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),

        -(st1 * ct2 * ct3) - (ct1 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),

        +(st1 * st2),
        -(ct1 * st2),
        +(ct2)
    );
}

template <typename T>
inline constexpr detail::mat3x<T> mat3_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat3x<T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),

        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),

        +(ct1 * st2),
        +(st1 * st2),
        +(ct2)
    );
}

// mat3_to_euler

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_XYZ(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[1].x * m.columns[1].x);
    const T t2 = math::atan2(-m.columns[2].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].z - c1 * m.columns[0].y, c1 * m.columns[1].y - s1 * m.columns[1].z);

    return detail::vecx<3, T>(
        -t1,
        -t2,
        -t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_XZY(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(-m.columns[1].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].y - c1 * m.columns[0].z, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_XYX(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, -m.columns[0].z);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[2].y - s1 * m.columns[2].z, c1 * m.columns[1].y + s1 * m.columns[1].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_XZX(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].z, m.columns[0].y);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[1].z - s1 * m.columns[1].y, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_YXZ(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[1].y * m.columns[1].y);
    const T t2 = math::atan2(-m.columns[2].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].z - c1 * m.columns[1].x, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_YZX(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(-m.columns[0].z, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].y * m.columns[1].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(m.columns[0].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].x + c1 * m.columns[1].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_YXY(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].x, m.columns[1].z);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x - s1 * m.columns[2].z, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_YZY(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, -m.columns[1].x);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-s1 * m.columns[0].x - c1 * m.columns[0].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_ZXY(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(-m.columns[1].x, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(m.columns[1].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x + s1 * m.columns[2].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_ZYX(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].z * m.columns[1].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(-m.columns[0].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[2].x - c1 * m.columns[2].y, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_ZXZ(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, -m.columns[2].y);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[1].x - s1 * m.columns[1].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat3_to_euler_ZYZ(const detail::mat3x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].x);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[0].y - s1 * m.columns[0].x, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

// =============== mat4 ===============

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

// mat4_from_euler

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_XYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        static_cast<T>(0),

        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        static_cast<T>(0),

        +(st2),
        -(st1 * ct2),
        +(ct1 * ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_XZY(T t1, T t3, T t2)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        static_cast<T>(0),

        -(st2),
        +(ct1 * ct2),
        +(st1 * ct2),
        static_cast<T>(0),

        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_XYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct2),
        +(st1 * st2),
        -(ct1 * st2),
        static_cast<T>(0),

        +(st2 * st3),
        +(ct1 * ct3) - (st1 * ct2 * st3),
        +(st1 * ct3) + (ct1 * ct2 * st3),
        static_cast<T>(0),

        +(st2 * ct3),
        -(ct1 * st3) - (st1 * ct2 * ct3),
        -(st1 * st3) + (ct1 * ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_XZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct2),
        +(ct1 * st2),
        +(st1 * st2),
        static_cast<T>(0),

        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        static_cast<T>(0),

        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_YXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        static_cast<T>(0),

        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        static_cast<T>(0),

        +(st1 * ct2),
        -(st2),
        +(ct1 * ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_YZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct1 * ct2),
        +(st2),
        -(st1 * ct2),
        static_cast<T>(0),

        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        static_cast<T>(0),

        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_YXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        static_cast<T>(0),

        +(st1 * st2),
        +(ct2),
        +(ct1 * st2),
        static_cast<T>(0),

        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_YZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        -(st1 * ct2 * ct3) - (ct1 * st3),
        static_cast<T>(0),

        -(ct1 * st2),
        +(ct2),
        +(st1 * st2),
        static_cast<T>(0),

        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_ZXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        static_cast<T>(0),

        -(st1 * ct2),
        +(ct1 * ct2),
        +(st2),
        static_cast<T>(0),

        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_ZYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct1 * ct2),
        +(st1 * ct2),
        -(st2),
        static_cast<T>(0),

        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        static_cast<T>(0),

        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        static_cast<T>(0),

        -(st1 * ct2 * ct3) - (ct1 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        static_cast<T>(0),

        +(st1 * st2),
        -(ct1 * st2),
        +(ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

template <typename T>
inline constexpr detail::mat4x<T> mat4_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return detail::mat4x<T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        static_cast<T>(0),

        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        static_cast<T>(0),

        +(ct1 * st2),
        +(st1 * st2),
        +(ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

// mat4_to_euler

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_XYZ(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[1].x * m.columns[1].x);
    const T t2 = math::atan2(-m.columns[2].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].z - c1 * m.columns[0].y, c1 * m.columns[1].y - s1 * m.columns[1].z);

    return detail::vecx<3, T>(
        -t1,
        -t2,
        -t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_XZY(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(-m.columns[1].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].y - c1 * m.columns[0].z, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_XYX(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, -m.columns[0].z);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[2].y - s1 * m.columns[2].z, c1 * m.columns[1].y + s1 * m.columns[1].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_XZX(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].z, m.columns[0].y);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[1].z - s1 * m.columns[1].y, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_YXZ(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[1].y * m.columns[1].y);
    const T t2 = math::atan2(-m.columns[2].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].z - c1 * m.columns[1].x, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_YZX(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(-m.columns[0].z, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].y * m.columns[1].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(m.columns[0].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].x + c1 * m.columns[1].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_YXY(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].x, m.columns[1].z);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x - s1 * m.columns[2].z, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_YZY(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, -m.columns[1].x);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-s1 * m.columns[0].x - c1 * m.columns[0].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_ZXY(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(-m.columns[1].x, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(m.columns[1].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x + s1 * m.columns[2].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_ZYX(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].z * m.columns[1].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(-m.columns[0].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[2].x - c1 * m.columns[2].y, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_ZXZ(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, -m.columns[2].y);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[1].x - s1 * m.columns[1].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

template <typename T>
inline constexpr detail::vecx<3, T> mat4_to_euler_ZYZ(const detail::mat4x<T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].x);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[0].y - s1 * m.columns[0].x, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return detail::vecx<3, T>(
        t1,
        t2,
        t3
    );
}

}
}