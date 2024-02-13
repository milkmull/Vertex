#pragma once

namespace vx {
namespace math {

// =============== srgb ===============

// https://entropymine.com/imageworsener/srgbformula/

/**
 * @brief Convert the color to sRGB color space.
 *
 * This function performs the conversion of the color to the sRGB color space.
 *
 * @return A new color representing the sRGB color space.
 */
template <typename T>
inline constexpr detail::colf<T> color_to_srgb(const detail::colf<T>& c)
{
    const detail::colf<T> cn = c.clamp();

    const T nr = (cn.r < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * cn.r)
        : (static_cast<T>(1.055)) * math::pow(cn.r, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
    const T ng = (cn.g < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * cn.g)
        : (static_cast<T>(1.055)) * math::pow(cn.g, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
    const T nb = (cn.b < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * cn.b)
        : (static_cast<T>(1.055)) * math::pow(cn.b, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);

    return detail::colf<T>(nr, ng, nb, c.a);
}

template <typename T>
inline constexpr detail::coli<T> color_to_srgb(const detail::coli<T>& c)
{
    using float_type = typename decltype(c)::float_type;
    return detail::coli<T>(color_to_srgb(float_type(c)));
}

// =============== linear ===============

// https://entropymine.com/imageworsener/srgbformula/

/**
 * @brief Convert the color to linear color space.
 *
 * This function performs the conversion of the color to the linear color space.
 *
 * @return A new color representing the linear color space.
 */
template <typename T>
inline constexpr detail::colf<T> color_to_linear(const detail::colf<T>& c)
{
    const detail::colf<T> cn = c.clamp();

    const T nr = (cn.r < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * cn.r)
        : math::pow((cn.r + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
    const T ng = (cn.g < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * cn.g)
        : math::pow((cn.g + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
    const T nb = (cn.b < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * cn.b)
        : math::pow((cn.b + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));

    return detail::colf<T>(nr, ng, nb, c.a);
}

template <typename T>
inline constexpr detail::coli<T> color_to_linear(const detail::coli<T>& c)
{
    using float_type = typename decltype(c)::float_type;
    return detail::coli<T>(color_to_linear(float_type(c)));
}

}
}