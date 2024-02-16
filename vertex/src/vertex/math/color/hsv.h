#pragma once

#include "type/color_type.h"

namespace vx {
namespace math {
namespace color_util {

// https://www.rapidtables.com/convert/color/hsv-to-rgb.html

/**
 * @brief Create a color from HSV (Hue, Saturation, Value) values.
 *
 * This function creates a color using HSV values, where h is the hue in degrees,
 * s is the saturation, v is the value, and a is the alpha (default is 1). The function
 * clamps input values to their valid ranges and performs the HSV to RGB conversion.
 *
 * @param h The hue value in degrees (range: [0, 360]).
 * @param s The saturation value (range: [0, 1]).
 * @param v The value (brightness) value (range: [0, 1]).
 * @param a The alpha value (default is 1, range: [0, 1]).
 * @return A new color created from the HSV values.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr vec<3, T> hsv_to_rgb(T h, T s, T v)
{
    h = math::clamp(h, static_cast<T>(0), static_cast<T>(360));
    s = math::clamp(s, static_cast<T>(0), static_cast<T>(1));
    v = math::clamp(v, static_cast<T>(0), static_cast<T>(1));

    if (s == static_cast<T>(0))
    {
        return vec<3, T>(v);
    }

    const T sector = math::floor(h * static_cast<T>(0.0166666667));
    const T frac = (h * static_cast<T>(0.0166666667)) - sector;

    const T n = v;
    const T o = v * (static_cast<T>(1) - s);
    const T p = v * (static_cast<T>(1) - s * frac);
    const T q = v * (static_cast<T>(1) - s * (static_cast<T>(1) - frac));

    switch (static_cast<int>(sector))
    {
        case 0:		return vec<3, T>(n, q, o);
        case 1:		return vec<3, T>(p, n, o);
        case 2:		return vec<3, T>(o, n, q);
        case 3:		return vec<3, T>(o, p, n);
        case 4:		return vec<3, T>(q, o, n);
        case 5:		return vec<3, T>(n, o, p);
        default:	break;
    }

    return vec<3, T>(0);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> rgb_to_hsv(T r, T g, T b)
{
    r = math::clamp(r, static_cast<T>(0), static_cast<T>(1));
    g = math::clamp(g, static_cast<T>(0), static_cast<T>(1));
    b = math::clamp(b, static_cast<T>(0), static_cast<T>(1));

    const T cmax = math::max({ r, g, b });

    if (cmax == static_cast<T>(0))
    {
        return vec<3, T>(0);
    }

    const T cmin = math::min({ r, g, b });
    const T v = cmax;

    if (cmin == cmax)
    {
        return vec<3, T>(0, 0, v);
    }

    const T dc = cmax - cmin;
    T h, s;

    s = dc / cmax;

    if (cmax == r)
    {
        h = static_cast<T>(0) + static_cast<T>(60) * (g - b) / dc;
    }
    else if (cmax == g)
    {
        h = static_cast<T>(120) + static_cast<T>(60) * (b - r) / dc;
    }
    else if (cmax == b)
    {
        h = static_cast<T>(240) + static_cast<T>(60) * (r - g) / dc;
    }
    else
    {
        h = static_cast<T>(0);
    }

    if (h < static_cast<T>(0))
    {
        h += static_cast<T>(360);
    }

    return vec<3, T>(h, s, v);
}

}
}
}