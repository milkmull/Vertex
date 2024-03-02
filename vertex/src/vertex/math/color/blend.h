#pragma once

#include "type/color_type.h"

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

struct blend_func
{
    // =============== constructors ===============

    inline constexpr blend_func(
        blend_mode src_blend, blend_mode dst_blend,
        blend_operator op = blend_operator::ADD,
        const color& constant = color()
    )
        : src_blend(src_blend)
        , dst_blend(dst_blend)
        , op(op)
        , constant(constant) {}

    inline constexpr blend_func() = default;
    inline constexpr blend_func(const blend_func&) = default;
    inline constexpr blend_func(blend_func&&) noexcept = default;

    inline constexpr blend_func& operator=(const blend_func&) = default;
    inline constexpr blend_func& operator=(blend_func&&) noexcept = default;

    ~blend_func() = default;

    // =============== data ===============

    blend_mode src_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_operator op = blend_operator::ADD;
    color constant;

    // =============== blend ===============

    inline constexpr color operator()(const color& src, const color& dst) const
    {
        return blend(src, dst);
    }

    inline constexpr color blend(const color& src, const color& dst) const
    {
        using T = typename color::value_type;

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
                    bp.col *= static_cast<T>(0);
                    break;

                case blend_mode::ONE:
                    bp.col *= static_cast<T>(1);
                    break;

                case blend_mode::SRC_COLOR:
                    bp.col *= src;
                    break;

                case blend_mode::ONE_MINUS_SRC_COLOR:
                    bp.col *= (static_cast<T>(1) - src);
                    break;

                case blend_mode::DST_COLOR:
                    bp.col *= dst;
                    break;

                case blend_mode::ONE_MINUS_DST_COLOR:
                    bp.col *= (static_cast<T>(1) - dst);
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.col *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.col *= (static_cast<T>(1) - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.col *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.col *= (static_cast<T>(1) - dst.a);
                    break;

                case blend_mode::CONST_COLOR:
                    bp.col *= constant;
                    break;

                case blend_mode::ONE_MINUS_CONST_COLOR:
                    bp.col *= (static_cast<T>(1) - constant);
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.col *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.col *= (static_cast<T>(1) - constant.a);
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

struct blend_func_separate
{
    // =============== constructors ===============

    inline constexpr blend_func_separate(
        blend_mode src_color_blend, blend_mode src_alpha_blend,
        blend_mode dst_color_blend, blend_mode dst_alpha_blend,
        blend_operator color_op, blend_operator alpha_op,
        const color& constant = color()
    )
        : src_color_blend(src_color_blend)
        , dst_color_blend(dst_color_blend)
        , src_alpha_blend(src_alpha_blend)
        , dst_alpha_blend(dst_alpha_blend)
        , color_op(color_op)
        , alpha_op(alpha_op)
        , constant(constant) {}

    inline constexpr blend_func_separate() = default;
    inline constexpr blend_func_separate(const blend_func_separate&) = default;
    inline constexpr blend_func_separate(blend_func_separate&&) noexcept = default;

    inline constexpr blend_func_separate& operator=(const blend_func_separate&) = default;
    inline constexpr blend_func_separate& operator=(blend_func_separate&&) noexcept = default;

    ~blend_func_separate() = default;

    // =============== data ===============

    blend_mode src_color_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_color_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_mode src_alpha_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_alpha_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_operator color_op = blend_operator::ADD;
    blend_operator alpha_op = blend_operator::ADD;

    color constant;

    // =============== blend ===============

    inline constexpr color operator()(const color& src, const color& dst) const
    {
        return blend(src, dst);
    }

    inline constexpr color blend(const color& src, const color& dst) const
    {
        using T = typename color::value_type;

        struct blend_pair
        {
            typename color::rgb_type rgb;
            T a;
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
                    bp.rgb *= static_cast<T>(0);
                    break;

                case blend_mode::ONE:
                    bp.rgb *= static_cast<T>(1);
                    break;

                case blend_mode::SRC_COLOR:
                    bp.rgb *= src.rgb();
                    break;

                case blend_mode::ONE_MINUS_SRC_COLOR:
                    bp.rgb *= (static_cast<T>(1) - src.rgb());
                    break;

                case blend_mode::DST_COLOR:
                    bp.rgb *= dst.rgb();
                    break;

                case blend_mode::ONE_MINUS_DST_COLOR:
                    bp.rgb *= (static_cast<T>(1) - dst.rgb());
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.rgb *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.rgb *= (static_cast<T>(1) - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.rgb *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.rgb *= (static_cast<T>(1) - dst.a);
                    break;

                case blend_mode::CONST_COLOR:
                    bp.rgb *= constant.rgb();
                    break;

                case blend_mode::ONE_MINUS_CONST_COLOR:
                    bp.rgb *= (static_cast<T>(1) - constant.rgb());
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.rgb *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.rgb *= (static_cast<T>(1) - constant.a);
                    break;

                default:
                    break;
            }

            switch (bp.alpha_mode)
            {
                case blend_mode::ZERO:
                    bp.a *= static_cast<T>(0);
                    break;

                case blend_mode::ONE:
                    bp.a *= static_cast<T>(1);
                    break;

                case blend_mode::SRC_ALPHA:
                    bp.a *= src.a;
                    break;

                case blend_mode::ONE_MINUS_SRC_ALPHA:
                    bp.a *= (static_cast<T>(1) - src.a);
                    break;

                case blend_mode::DST_ALPHA:
                    bp.a *= dst.a;
                    break;

                case blend_mode::ONE_MINUS_DST_ALPHA:
                    bp.a *= (static_cast<T>(1) - dst.a);
                    break;

                case blend_mode::CONST_ALPHA:
                    bp.a *= constant.a;
                    break;

                case blend_mode::ONE_MINUS_CONST_ALPHA:
                    bp.a *= (static_cast<T>(1) - constant.a);
                    break;

                default:
                    break;
            }
        }

        typename color::rgb_type blend_rgb;
        T blend_a = 0;

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