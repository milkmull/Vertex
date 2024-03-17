#pragma once

#include "fn/color_fn_common.h"
#include "../math/type/vec3.h"
#include "../math/fn/vec_fn_common.h"

namespace vx {

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
 * @brief Structure representing a blend function for blending colors.
 *
 * The blend function defines how colors are blended together when rendering.
 */
struct blend_func
{
    // =============== data ===============

    blend_mode src_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_blend = blend_mode::ONE_MINUS_SRC_ALPHA;
    blend_operator op = blend_operator::ADD;
    color constant;

    // =============== comparison ===============

    inline constexpr bool operator==(const blend_func& other) const
    {
        return src_blend == other.src_blend
            && dst_blend == other.dst_blend
            && op == other.op
            && constant == other.constant;
    }

    inline constexpr bool operator!=(const blend_func& other) const
    {
        return !(*this == other);
    }

    // =============== blend ===============

    /**
     * @brief Blends two colors using the specified blend function.
     *
     * @param src The source color.
     * @param dst The destination color.
     * @return The blended color.
     */
    inline constexpr color operator()(const color& src, const color& dst) const
    {
        return blend(src, dst);
    }

    /**
     * @brief Blends two colors using the specified blend function.
     *
     * @param src The source color.
     * @param dst The destination color.
     * @return The blended color.
     */
    inline constexpr color blend(const color& src, const color& dst) const
    {
        struct blend_pair
        {
            color col;
            blend_mode color_mode;
            blend_mode alpha_mode;
        };

        blend_pair pair[2] = { { src, src_blend }, { dst, dst_blend } };

        for (blend_pair& bp : pair)
        {
            switch (bp.color_mode)
            {
                case blend_mode::ZERO:
                    bp.col *= 0.0f;
                    break;

                case blend_mode::ONE:
                    bp.col *= 1.0f;
                    break;

                case blend_mode::SRC_COLOR:
                    bp.col *= src;
                    break;

                case blend_mode::ONE_MINUS_SRC_COLOR:
                    bp.col *= (1.0f - src);
                    break;

                case blend_mode::DST_COLOR:
                    bp.col *= dst;
                    break;

                case blend_mode::ONE_MINUS_DST_COLOR:
                    bp.col *= (1.0f - dst);
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.col *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.col *= (1.0f - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.col *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.col *= (1.0f - dst.a);
                    break;

                case blend_mode::CONST_COLOR:
                    bp.col *= constant;
                    break;

                case blend_mode::ONE_MINUS_CONST_COLOR:
                    bp.col *= (1.0f - constant);
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.col *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.col *= (1.0f - constant.a);
                    break;

                default:
                    break;
            }
        }

        switch (op)
        {
            case blend_operator::ADD:               return (pair[0].col + pair[1].col);
            case blend_operator::SUBTRACT:          return (pair[0].col - pair[1].col);
            case blend_operator::REVERSE_SUBTRACT:  return (pair[1].col - pair[0].col);
            case blend_operator::MIN:               return math::min(pair[0].col, pair[1].col);
            case blend_operator::MAX:               return math::max(pair[0].col, pair[1].col);
            default:                                break;
        }

        return (pair[0].col + pair[1].col);
    }

};

/**
 * @brief Structure representing a separate blend function for blending colors with distinct color and alpha blending modes.
 *
 * The blend_func_separate structure allows for blending colors with separate blend modes for their color and alpha components.
 * This allows finer control over the blending process, as it enables different blending behaviors for the color and alpha channels.
 */
struct blend_func_separate
{
    // =============== data ===============

    blend_mode src_color_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_color_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_mode src_alpha_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_alpha_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_operator color_op = blend_operator::ADD;
    blend_operator alpha_op = blend_operator::ADD;

    color constant;

    // =============== blend ===============

    /**
     * @brief Blends two colors using the specified separate blend function.
     *
     * @param src The source color.
     * @param dst The destination color.
     * @return The blended color.
     */
    inline constexpr color operator()(const color& src, const color& dst) const
    {
        return blend(src, dst);
    }

    /**
     * @brief Blends two colors using the specified separate blend function.
     *
     * @param src The source color.
     * @param dst The destination color.
     * @return The blended color.
     */
    inline constexpr color blend(const color& src, const color& dst) const
    {
        struct blend_pair
        {
            vec3 rgb;
            float a;
            blend_mode color_mode;
            blend_mode alpha_mode;
        };

        blend_pair pair[2] = {
            { src.rgb(), src.a, src_color_blend, src_alpha_blend },
            { dst.rgb(), dst.a, dst_color_blend, dst_alpha_blend }
        };

        for (blend_pair& bp : pair)
        {
            switch (bp.color_mode)
            {
                case blend_mode::ZERO:
                    bp.rgb *= 0.0f;
                    break;

                case blend_mode::ONE:
                    bp.rgb *= 1.0f;
                    break;

                case blend_mode::SRC_COLOR:
                    bp.rgb *= src.rgb();
                    break;

                case blend_mode::ONE_MINUS_SRC_COLOR:
                    bp.rgb *= (1.0f - src.rgb());
                    break;

                case blend_mode::DST_COLOR:
                    bp.rgb *= dst.rgb();
                    break;

                case blend_mode::ONE_MINUS_DST_COLOR:
                    bp.rgb *= (1.0f - dst.rgb());
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.rgb *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.rgb *= (1.0f - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.rgb *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.rgb *= (1.0f - dst.a);
                    break;

                case blend_mode::CONST_COLOR:
                    bp.rgb *= constant.rgb();
                    break;

                case blend_mode::ONE_MINUS_CONST_COLOR:
                    bp.rgb *= (1.0f - constant.rgb());
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.rgb *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.rgb *= (1.0f - constant.a);
                    break;

                default:
                    break;
            }

            switch (bp.alpha_mode)
            {
                case blend_mode::ZERO:
                    bp.a *= 0.0f;
                    break;

                case blend_mode::ONE:
                    bp.a *= 1.0f;
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.a *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.a *= (1.0f - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.a *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.a *= (1.0f - dst.a);
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.a *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.a *= (1.0f - constant.a);
                    break;

                default:
                    break;
            }
        }

        vec3 blend_rgb;
        float blend_a = 0.0f;

        switch (color_op)
        {
            case blend_operator::ADD:
                blend_rgb = (pair[0].rgb + pair[1].rgb);
                break;

            case blend_operator::SUBTRACT:
                blend_rgb = (pair[0].rgb - pair[1].rgb);
                break;

            case blend_operator::REVERSE_SUBTRACT:
                blend_rgb = (pair[1].rgb - pair[0].rgb);
                break;

            case blend_operator::MIN:
                blend_rgb = math::min(pair[0].rgb, pair[1].rgb);
                break;

            case blend_operator::MAX:
                blend_rgb = math::max(pair[0].rgb, pair[1].rgb);
                break;

            default:
                break;
        }

        switch (alpha_op)
        {
            case blend_operator::ADD:
                blend_a = (pair[0].a + pair[1].a);
                break;

            case blend_operator::SUBTRACT:
                blend_a = (pair[0].a - pair[1].a);
                break;

            case blend_operator::REVERSE_SUBTRACT:
                blend_a = (pair[1].a - pair[0].a);
                break;

            case blend_operator::MIN:
                blend_a = math::min(pair[0].a, pair[1].a);
                break;

            case blend_operator::MAX:
                blend_a = math::max(pair[0].a, pair[1].a);
                break;

            default:
                break;
        }

        return color(blend_rgb, blend_a);
    }

};

}
}