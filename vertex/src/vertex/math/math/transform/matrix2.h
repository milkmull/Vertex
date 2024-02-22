#pragma once

#include "../math.h"

namespace vx {
namespace math {
namespace transform2d {
namespace matrix2 {

// =============== mat2 pure rotation ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<2, 2, T> make_rotation(T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<2, 2, T>(cosa, sina, -sina, cosa);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T get_rotation(const mat<2, 2, T>& m)
{
    return math::atan2(m.columns[0].y, m.columns[0].x);
}

// =============== mat2 pure scale ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<2, 2, T> make_scale(const vec<2, T>& scale)
{
    return mat<2, 2, T>(
        scale.x, static_cast<T>(0),
        static_cast<T>(0), scale.y
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> get_scale(const mat<2, 2, T>& m)
{
    return vec<2, T>(
        math::length(columns[0]),
        math::length(columns[1])
    );
}

}
}
}
}