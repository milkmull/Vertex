#pragma once

#include "../math/detail/base_type_defs.h"
#include "../math/vec_fn_common.h"

namespace vx {

// =============== blend ===============

enum class blend_mode
{
    ZERO,
    ONE,

    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,

    DST_COLOR,
    ONE_MINUS_DST_COLOR,

    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,

    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,

    CONST_COLOR,
    ONE_MINUS_CONST_COLOR,

    CONST_ALPHA,
    ONE_MINUS_CONST_ALPHA,
};

enum class blend_operator
{
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX
};

namespace math {

// https://learnopengl.com/Advanced-OpenGL/Blending

/**
 * @brief Blend two colors using specified blending modes and operators.
 *
 * This function performs color blending based on the provided blending modes and operators.
 *
 * @param src The source color.
 * @param src_blend The blending mode for the source color.
 * @param dst The destination color.
 * @param dst_blend The blending mode for the destination color.
 * @param constant The constant color used in blending (default is black).
 * @param op The blending operator.
 * @return The result of blending the source and destination colors.
 */
template <typename T>
inline constexpr detail::colf<T> blend(
    const detail::colf<T>& src, blend_mode src_blend,
    const detail::colf<T>& dst, blend_mode dst_blend,
    const detail::colf<T>& constant = detail::colf<T>(),
    blend_operator op = blend_operator::ADD
)
{
    struct blend_pair
    {
        detail::colf<T> color;
        blend_mode mode;
    };

    blend_pair res[2] = { { src, src_blend }, { dst, dst_blend } };

    for (blend_pair& bp : res)
    {
        switch (bp.mode)
        {
            case blend_mode::ZERO:
                bp.color *= static_cast<T>(0);
                break;

            case blend_mode::ONE:
                break;

            case blend_mode::SRC_COLOR:
                bp.color *= src;
                break;

            case blend_mode::ONE_MINUS_SRC_COLOR:
                bp.color *= (static_cast<T>(1) - src);
                break;

            case blend_mode::DST_COLOR:
                bp.color *= dst;
                break;

            case blend_mode::ONE_MINUS_DST_COLOR:
                bp.color *= (static_cast<T>(1) - dst);
                break;

            case blend_mode::SRC_ALPHA:
                bp.color *= src.a;
                break;

            case blend_mode::ONE_MINUS_SRC_ALPHA:
                bp.color *= (static_cast<T>(1) - src.a);
                break;

            case blend_mode::DST_ALPHA:
                bp.color *= dst.a;
                break;

            case blend_mode::ONE_MINUS_DST_ALPHA:
                bp.color *= (static_cast<T>(1) - dst.a);
                break;

            case blend_mode::CONST_COLOR:
                bp.color *= constant;
                break;

            case blend_mode::ONE_MINUS_CONST_COLOR:
                bp.color *= (static_cast<T>(1) - constant);
                break;

            case blend_mode::CONST_ALPHA:
                bp.color *= constant.a;
                break;

            case blend_mode::ONE_MINUS_CONST_ALPHA:
                bp.color *= (static_cast<T>(1) - constant.a);
                break;

            default:
                break;
        }
    }

    switch (op)
    {
        case blend_operator::ADD:				return res[0].color + res[1].color;
        case blend_operator::SUBTRACT:			return res[0].color - res[1].color;
        case blend_operator::REVERSE_SUBTRACT:	return res[1].color - res[0].color;
        case blend_operator::MIN:				return math::min(res[1].color, res[0].color);
        case blend_operator::MAX:				return math::max(res[1].color, res[0].color);
        default:								break;
    }

    return res[0].color + res[1].color;
}

template <typename T>
inline constexpr detail::coli<T> blend(
    const detail::coli<T>& src, blend_mode src_blend,
    const detail::coli<T>& dst, blend_mode dst_blend,
    const detail::coli<T>& constant = detail::coli<T>(),
    blend_operator op = blend_operator::ADD
)
{
    using float_type = typename detail::coli<T>::float_type;

    return detail::coli<T>(blend(
        float_type(src), src_blend,
        float_type(dst), dst_blend,
        float_type(constant),
        op
    ));
}

// =============== lighten ===============

/**
 * @brief Lighten the color by a specified amount.
 *
 * This function returns a new color by lightening each component based on the provided amount.
 *
 * @param amount The amount by which to lighten the color (in the range [0, 1]).
 * @return A new color with lightened components.
 */
template <typename T>
inline constexpr detail::colf<T> lighten(const detail::colf<T>& c, T amount)
{
    return detail::colf<T>(
        c.r + (static_cast<T>(1) - c.r) * amount,
        c.g + (static_cast<T>(1) - c.g) * amount,
        c.b + (static_cast<T>(1) - c.b) * amount,
        c.a
    );
}

template <typename T>
inline constexpr detail::coli<T> lighten(const detail::coli<T>& c, T amount)
{
    using float_type = typename detail::coli<T>::float_type;
    return detail::coli<T>(lighten(float_type(c), amount));
}

// =============== darken ===============

/**
 * @brief Darken the color by a specified amount.
 *
 * This function returns a new color by darkening each component based on the provided amount.
 *
 * @param amount The amount by which to darken the color (in the range [0, 1]).
 * @return A new color with darkened components.
 */
template <typename T>
inline constexpr detail::colf<T> darken(const detail::colf<T>& c, T amount)
{
    return type(
        c.r * (static_cast<T>(1) - amount),
        c.g * (static_cast<T>(1) - amount),
        c.b * (static_cast<T>(1) - amount),
        c.a
    );
}

template <typename T>
inline constexpr detail::coli<T> darken(const detail::coli<T>& c, T amount)
{
    using float_type = typename detail::coli<T>::float_type;
    return detail::coli<T>(darken(float_type(c), amount));
}

}
}