#pragma once

#include "../math/detail/base_type_defs.h"

namespace vx {
namespace math {

// =============== hsl ===============

// https://www.rapidtables.com/convert/color/rgb-to-hsl.html

/**
 * @brief Create a color from HSL (Hue, Saturation, Lightness) values.
 *
 * This function creates a color using HSL values, where h is the hue in degrees,
 * s is the saturation, l is the lightness, and a is the alpha (default is 1). The function
 * clamps input values to their valid ranges and performs the HSL to RGB conversion.
 *
 * @param h The hue value in degrees (range: [0, 360]).
 * @param s The saturation value (range: [0, 1]).
 * @param l The lightness value (range: [0, 1]).
 * @param a The alpha value (default is 1, range: [0, 1]).
 * @return A new color created from the HSL values.
 */
template <typename T = float, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr detail::colf<T> from_hsl(T h, T s, T l, T a = static_cast<T>(1))
{
    h = math::clamp(h, static_cast<T>(0), static_cast<T>(360));
    s = math::clamp(s, static_cast<T>(0), static_cast<T>(1));
    l = math::clamp(l, static_cast<T>(0), static_cast<T>(1));
    a = math::clamp(a, static_cast<T>(0), static_cast<T>(1));

    if (s == static_cast<T>(0))
    {
        return detail::colf<T>(l, l, l, a);
    }

    const T sector = std::floor(h * static_cast<T>(0.0166666667));
    const T frac = (h * static_cast<T>(0.0166666667)) - sector;

    const T c = (static_cast<T>(1) - math::abs(static_cast<T>(2) * l - static_cast<T>(1))) * s;

    const T n = l + c * static_cast<T>(0.5);
    const T o = l - c * static_cast<T>(0.5);
    const T p = l + c * (static_cast<T>(0.5) - frac);
    const T q = l + c * (frac - static_cast<T>(0.5));

    switch (static_cast<int>(sector))
    {
        case 0:		return detail::colf<T>(n, q, o, a);
        case 1:		return detail::colf<T>(p, n, o, a);
        case 2:		return detail::colf<T>(o, n, q, a);
        case 3:		return detail::colf<T>(o, p, n, a);
        case 4:		return detail::colf<T>(q, o, n, a);
        case 5:		return detail::colf<T>(n, o, p, a);
        default:	break;
    }

    return detail::colf<T>();
}

/**
 * @brief Convert the color to HSL (Hue, Saturation, Lightness) values.
 *
 * This function calculates the HSL values from the color's RGB components.
 *
 * @return A 3D vector representing the HSL values (Hue in degrees, Saturation, Lightness).
 */
template <typename T>
inline constexpr auto color_to_hsl(const detail::colf<T>& c)
{
    using vec3_type = typename decltype(c)::vec3_type;

    const detail::colf<T> cn = c.clamp();
    const T cmax = cn.max_color();

    if (cmax == static_cast<T>(0))
    {
        return vec3_type();
    }

    const T cmin = cn.min_color();
    const T l = (cmin + cmax) * static_cast<T>(0.5);

    if (cmin == cmax)
    {
        return vec3_type(static_cast<T>(0), static_cast<T>(0), l);
    }

    const T dc = cmax - cmin;

    T h, s;

    s = (l > static_cast<T>(0.5)) ? dc * (static_cast<T>(0.5) - cmax - cmin) : dc / (cmax + cmin);

    if (cmax == cn.r)
    {
        h = static_cast<T>(0) + static_cast<T>(60) * (cn.g - cn.b) / dc;
    }
    else if (cmax == cn.g)
    {
        h = static_cast<T>(120) + static_cast<T>(60) * (cn.b - cn.r) / dc;
    }
    else if (cmax == cn.b)
    {
        h = static_cast<T>(240) + static_cast<T>(60) * (cn.r - cn.g) / dc;
    }
    else
    {
        h = static_cast<T>(0);
    }

    if (h < static_cast<T>(0))
    {
        h += static_cast<T>(360);
    }

    return vec3_type(h, s, l);
}

template <typename T>
inline constexpr auto color_to_hsl(const detail::coli<T>& c)
{
    using float_type = typename decltype(c)::float_type;
    return color_to_hsl(float_type(c));
}

}
}