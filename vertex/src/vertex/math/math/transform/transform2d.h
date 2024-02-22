#pragma once

#include "../math.h"

namespace vx {
namespace math {
namespace transform2d {

// =============== 2d translation ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_translation(const vec<2, T>& translation)
{
    return mat<3, 3, T>(
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        translation.x,     translation.y,     static_cast<T>(1)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> get_translation(const mat<3, 3, T>& m)
{
    return vec<2, T>(m.columns[2].x, m.columns[2].y);
}

// =============== 2d rotation ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_rotation(T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<3, 3, T>(
        cosa,              sina,              static_cast<T>(0),
       -sina,              cosa,              static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T get_rotation(const mat<3, 3, T>& m)
{
    return math::atan2(m.columns[0].y, m.columns[0].x);
}

// =============== 2d scale ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_scale(const vec<2, T>& scale)
{
    return mat<3, 3, T>(
        scale.x,           static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), scale.y,           static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> get_scale(const mat<3, 3, T>& m)
{
    return vec<2, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1])
    );
}

// =============== trs ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_trs(const vec<2, T>& translation, T angle, vec<2, T>& scale)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<3, 3, T>(
        cosa * scale.x, sina * scale.x, static_cast<T>(0),
       -sina * scale.y, cosa * scale.y, static_cast<T>(0),
        translation.x,  translation.y,  static_cast<T>(1)
    );
}

// =============== invert ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> invert(const mat<3, 3, T>& m)
{
    const mat<2, 2, T> ibasis = math::invert(mat<2, 2, T>(m));

    return mat<3, 3, T>(
        vec<3, T>(ibasis.columns[0],                 static_cast<T>(0)),
        vec<3, T>(ibasis.columns[1],                 static_cast<T>(0)),
        vec<3, T>(ibasis * vec<2, T>(-m.columns[2]), static_cast<T>(1))
    );
}

// =============== transform ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> transform(const mat<3, 3, T>& m, const vec<2, T>& v)
{
    return vec<2, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + m.columns[2].x,
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + m.columns[2].y
    );
}

}
}
}