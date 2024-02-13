#pragma once

namespace vx {
namespace math {

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
template <typename T = float, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr detail::colf<T> color_from_hsv(T h, T s, T v, T a = static_cast<T>(1))
{
    h = math::clamp(h, static_cast<T>(0), static_cast<T>(360));
    s = math::clamp(s, static_cast<T>(0), static_cast<T>(1));
    v = math::clamp(v, static_cast<T>(0), static_cast<T>(1));
    a = math::clamp(a, static_cast<T>(0), static_cast<T>(1));

    if (s == static_cast<T>(0))
    {
        return detail::colf<T>(v, v, v, a);
    }

    const T sector = math::floor(h * static_cast<T>(0.0166666667));
    const T frac = (h * static_cast<T>(0.0166666667)) - sector;

    const T n = v;
    const T o = v * (static_cast<T>(1) - s);
    const T p = v * (static_cast<T>(1) - s * frac);
    const T q = v * (static_cast<T>(1) - s * (static_cast<T>(1) - frac));

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

template <typename T>
inline constexpr auto color_to_hsv(const detail::colf<T>& c)
{
    using vec3_type = typename decltype(c)::vec3_type;

    const detail::colf<T> cn = c.clamp();
    const T cmax = cn.max_color();

    if (cmax == static_cast<T>(0))
    {
        return vec3_type();
    }

    const T cmin = cn.min_color();
    const T v = cmax;

    if (cmin == cmax)
    {
        return vec3_type(static_cast<T>(0), static_cast<T>(0), v);
    }

    const T dc = cmax - cmin;
    T h, s;

    s = dc / cmax;

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

    return vec3_type(h, s, v);
}

template <typename T>
inline constexpr auto color_to_hsv(const detail::coli<T>& c)
{
    using float_type = typename decltype(c)::float_type;
    return color_from_hsv(float_type(c));
}

}
}