#pragma once

#include "type/color_type.h"

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
 * @brief Blends two colors using the specified blend modes and operation.
 *
 * This function blends two colors using the specified blend modes and operation. It supports various blend modes
 * and blending operations. You can also provide a constant color for certain blend modes.
 *
 * @param src The source color.
 * @param src_blend The blend mode for the source color.
 * @param dst The destination color.
 * @param dst_blend The blend mode for the destination color.
 * @param constant The constant color used in some blend modes. Defaults to color(0).
 * @param op The blending operation to apply. Defaults to blend_operator::ADD.
 * @return The blended color.
 */
inline constexpr color blend(
    const color& src, blend_mode src_blend,
    const color& dst, blend_mode dst_blend,
    const color& constant = color(0),
    blend_operator op = blend_operator::ADD
)
{
    using T = typename color::value_type;

    struct blend_pair
    {
        color color;
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
        case blend_operator::MIN:				return math::min(res[0].color, res[1].color);
        case blend_operator::MAX:				return math::max(res[0].color, res[1].color);
        default:								break;
    }

    return res[0].color + res[1].color;
}

}
}