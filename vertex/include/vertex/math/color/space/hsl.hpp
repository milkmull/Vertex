#pragma once

#include "../types/color.hpp"
#include "../functions/common.hpp"

namespace vx {
namespace math {

// https://www.rapidtables.com/convert/color/rgb-to-hsl.html

template <typename T, VXM_REQ_FLOAT(T)>
inline constexpr vec<3, T> hsl_to_rgb(T h, T s, T l) noexcept
{
    h = clamp(h, static_cast<T>(0), static_cast<T>(360));
    s = clamp(s, static_cast<T>(0), static_cast<T>(1));
    l = clamp(l, static_cast<T>(0), static_cast<T>(1));

    if (s == static_cast<T>(0))
    {
        return vec<3, T>(l);
    }

    const T sector = floor(h * static_cast<T>(0.0166666667));
    const T frac = (h * static_cast<T>(0.0166666667)) - sector;

    const T c = (static_cast<T>(1) - abs(static_cast<T>(2) * l - static_cast<T>(1))) * s;

    const T n = l + c * static_cast<T>(0.5);
    const T o = l - c * static_cast<T>(0.5);
    const T p = l + c * (static_cast<T>(0.5) - frac);
    const T q = l + c * (frac - static_cast<T>(0.5));

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

template <typename T, VXM_REQ_FLOAT(T)>
inline constexpr vec<3, T> rgb_to_hsl(T r, T g, T b) noexcept
{
    r = clamp(r, static_cast<T>(0), static_cast<T>(1));
    g = clamp(g, static_cast<T>(0), static_cast<T>(1));
    b = clamp(b, static_cast<T>(0), static_cast<T>(1));

    const T cmax = max(r, g, b);

    if (cmax == static_cast<T>(0))
    {
        return vec<3, T>(0);
    }

    const T cmin = min(r, g, b);
    const T l = (cmin + cmax) * static_cast<T>(0.5);

    if (cmin == cmax)
    {
        return vec<3, T>(0, 0, l);
    }

    const T dc = cmax - cmin;

    T h, s;

    s = dc / (static_cast<T>(1) - abs(static_cast<T>(2) * l - static_cast<T>(1)));

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

    return vec<3, T>(h, s, l);
}

} // namespace math
} // namespace vx