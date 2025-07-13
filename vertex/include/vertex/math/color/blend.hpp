#pragma once

#include "vertex/math/color/types/color.hpp"

namespace vx {
namespace math {

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

// https://learnopengl.com/Advanced-OpenGL/Blending

using blend_func_t = color(*)(const color&, const color&);

struct blend_func
{
    ////////////////////////////////////////////////////////////////////////////////
    // data
    ////////////////////////////////////////////////////////////////////////////////

    blend_mode src_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_blend = blend_mode::ONE_MINUS_SRC_ALPHA;
    blend_operator op = blend_operator::ADD;
    color constant;

    ////////////////////////////////////////////////////////////////////////////////
    // comparison
    ////////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr bool operator==(const blend_func& other) const noexcept
    {
        return src_blend == other.src_blend
            && dst_blend == other.dst_blend
            && op == other.op
            && constant == other.constant;
    }

    VX_FORCE_INLINE constexpr bool operator!=(const blend_func& other) const noexcept
    {
        return !(*this == other);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // blend
    ////////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr color operator()(const color& src, const color& dst) const noexcept
    {
        return blend(src, dst);
    }

    VX_FORCE_INLINE constexpr color blend(const color& src, const color& dst) const noexcept
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
                {
                    bp.col *= 0.0f;
                    break;
                }
                case blend_mode::ONE:
                {
                    bp.col *= 1.0f;
                    break;
                }
                case blend_mode::SRC_COLOR:
                {
                    bp.col *= src;
                    break;
                }
                case blend_mode::ONE_MINUS_SRC_COLOR:
                {
                    bp.col *= (1.0f - src);
                    break;
                }
                case blend_mode::DST_COLOR:
                {
                    bp.col *= dst;
                    break;
                }
                case blend_mode::ONE_MINUS_DST_COLOR:
                {
                    bp.col *= (1.0f - dst);
                    break;
                }
                case blend_mode::SRC_ALPHA:
                {
                    bp.col *= src.a;
                    break;
                }
                case blend_mode::ONE_MINUS_SRC_ALPHA:
                {
                    bp.col *= (1.0f - src.a);
                    break;
                }
                case blend_mode::DST_ALPHA:
                {
                    bp.col *= dst.a;
                    break;
                }
                case blend_mode::ONE_MINUS_DST_ALPHA:
                {
                    bp.col *= (1.0f - dst.a);
                    break;
                }
                case blend_mode::CONST_COLOR:
                {
                    bp.col *= constant;
                    break;
                }
                case blend_mode::ONE_MINUS_CONST_COLOR:
                {
                    bp.col *= (1.0f - constant);
                    break;
                }
                case blend_mode::CONST_ALPHA:
                {
                    bp.col *= constant.a;
                    break;
                }
                case blend_mode::ONE_MINUS_CONST_ALPHA:
                {
                    bp.col *= (1.0f - constant.a);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        switch (op)
        {
            case blend_operator::ADD:               return (pair[0].col + pair[1].col);
            case blend_operator::SUBTRACT:          return (pair[0].col - pair[1].col);
            case blend_operator::REVERSE_SUBTRACT:  return (pair[1].col - pair[0].col);
            case blend_operator::MIN:               return min(pair[0].col, pair[1].col);
            case blend_operator::MAX:               return max(pair[0].col, pair[1].col);
            default:                                break;
        }

        return (pair[0].col + pair[1].col);
    }

};

struct blend_func_separate
{
    ////////////////////////////////////////////////////////////////////////////////
    // data
    ////////////////////////////////////////////////////////////////////////////////

    blend_mode src_color_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_color_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_mode src_alpha_blend = blend_mode::SRC_ALPHA;
    blend_mode dst_alpha_blend = blend_mode::ONE_MINUS_SRC_ALPHA;

    blend_operator color_op = blend_operator::ADD;
    blend_operator alpha_op = blend_operator::ADD;

    color constant;

    ////////////////////////////////////////////////////////////////////////////////
    // blend
    ////////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr color operator()(const color& src, const color& dst) const noexcept
    {
        return blend(src, dst);
    }

    VX_FORCE_INLINE constexpr color blend(const color& src, const color& dst) const noexcept
    {
        struct blend_pair
        {
            color col;
            blend_mode color_mode;
            blend_mode alpha_mode;
        };

        blend_pair pair[2] = {
            { src, src_color_blend, src_alpha_blend },
            { dst, dst_color_blend, dst_alpha_blend }
        };

        for (blend_pair& bp : pair)
        {
            switch (bp.color_mode)
            {
                case blend_mode::ZERO:
                {
                    bp.col.r = bp.col.g = bp.col.b = 0.0f;
                    break;
                }
                case blend_mode::ONE:
                {
                    break;
                }
                case blend_mode::SRC_COLOR:
                {
                    bp.col.r *= src.r;
                    bp.col.g *= src.g;
                    bp.col.b *= src.b;
                    break;
                }
                case blend_mode::ONE_MINUS_SRC_COLOR:
                {
                    bp.col.r *= (1.0f - src.r);
                    bp.col.g *= (1.0f - src.g);
                    bp.col.b *= (1.0f - src.b);
                    break;
                }
                case blend_mode::DST_COLOR:
                {
                    bp.col.r *= dst.r;
                    bp.col.g *= dst.g;
                    bp.col.b *= dst.b;
                    break;
                }
                case blend_mode::ONE_MINUS_DST_COLOR:
                {
                    bp.col.r *= (1.0f - dst.r);
                    bp.col.g *= (1.0f - dst.g);
                    bp.col.b *= (1.0f - dst.b);
                    break;
                }
                case blend_mode::SRC_ALPHA:
                {
                    bp.col.r *= src.a;
                    bp.col.g *= src.a;
                    bp.col.b *= src.a;
                    break;
                }
                case blend_mode::ONE_MINUS_SRC_ALPHA:
                {
                    bp.col.r *= (1.0f - src.a);
                    bp.col.g *= (1.0f - src.a);
                    bp.col.b *= (1.0f - src.a);
                    break;
                }
                case blend_mode::DST_ALPHA:
                {
                    bp.col.r *= dst.a;
                    bp.col.g *= dst.a;
                    bp.col.b *= dst.a;
                    break;
                }
                case blend_mode::ONE_MINUS_DST_ALPHA:
                {
                    bp.col.r *= (1.0f - dst.a);
                    bp.col.g *= (1.0f - dst.a);
                    bp.col.b *= (1.0f - dst.a);
                    break;
                }
                case blend_mode::CONST_COLOR:
                {
                    bp.col.r *= constant.r;
                    bp.col.g *= constant.g;
                    bp.col.b *= constant.b;
                    break;
                }
                case blend_mode::ONE_MINUS_CONST_COLOR:
                {
                    bp.col.r *= (1.0f - constant.r);
                    bp.col.g *= (1.0f - constant.g);
                    bp.col.b *= (1.0f - constant.b);
                    break;
                }
                case blend_mode::CONST_ALPHA:
                {
                    bp.col.r *= constant.a;
                    bp.col.g *= constant.a;
                    bp.col.b *= constant.a;
                    break;
                }
                case blend_mode::ONE_MINUS_CONST_ALPHA:
                {
                    bp.col.r *= (1.0f - constant.a);
                    bp.col.g *= (1.0f - constant.a);
                    bp.col.b *= (1.0f - constant.a);
                    break;
                }
                default:
                {
                    break;
                }
            }

            switch (bp.alpha_mode)
            {
                case blend_mode::ZERO:
                {
                    bp.col.a *= 0.0f;
                    break;
                }
                case blend_mode::ONE:
                {
                    bp.col.a *= 1.0f;
                    break;
                }
                case blend_mode::SRC_ALPHA:
                {
                    bp.col.a *= src.a;
                    break;
                }
                case blend_mode::ONE_MINUS_SRC_ALPHA:
                {
                    bp.col.a *= (1.0f - src.a);
                    break;
                }
                case blend_mode::DST_ALPHA:
                {
                    bp.col.a *= dst.a;
                    break;
                }
                case blend_mode::ONE_MINUS_DST_ALPHA:
                {
                    bp.col.a *= (1.0f - dst.a);
                    break;
                }
                case blend_mode::CONST_ALPHA:
                {
                    bp.col.a *= constant.a;
                    break;
                }
                case blend_mode::ONE_MINUS_CONST_ALPHA:
                {
                    bp.col.a *= (1.0f - constant.a);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        color blend_color(0.0f, 0.0f, 0.0f, 0.0f);

        switch (color_op)
        {
            case blend_operator::ADD:
            {
                blend_color.r = (pair[0].col.r + pair[1].col.r);
                blend_color.g = (pair[0].col.g + pair[1].col.g);
                blend_color.b = (pair[0].col.b + pair[1].col.b);
                break;
            }
            case blend_operator::SUBTRACT:
            {
                blend_color.r = (pair[0].col.r - pair[1].col.r);
                blend_color.g = (pair[0].col.g - pair[1].col.g);
                blend_color.b = (pair[0].col.b - pair[1].col.b);
                break;
            }
            case blend_operator::REVERSE_SUBTRACT:
            {
                blend_color.r = (pair[1].col.r - pair[0].col.r);
                blend_color.g = (pair[1].col.g - pair[0].col.g);
                blend_color.b = (pair[1].col.b - pair[0].col.b);
                break;
            }
            case blend_operator::MIN:
            {
                blend_color.r = min(pair[0].col.r, pair[1].col.r);
                blend_color.g = min(pair[0].col.g, pair[1].col.g);
                blend_color.b = min(pair[0].col.b, pair[1].col.b);
                break;
            }
            case blend_operator::MAX:
            {
                blend_color.r = max(pair[0].col.r, pair[1].col.r);
                blend_color.g = max(pair[0].col.g, pair[1].col.g);
                blend_color.b = max(pair[0].col.b, pair[1].col.b);
                break;
            }
            default:
            {
                break;
            }
        }

        switch (alpha_op)
        {
            case blend_operator::ADD:
            {
                blend_color.a = (pair[0].col.a + pair[1].col.a);
                break;
            }
            case blend_operator::SUBTRACT:
            {
                blend_color.a = (pair[0].col.a - pair[1].col.a);
                break;
            }
            case blend_operator::REVERSE_SUBTRACT:
            {
                blend_color.a = (pair[1].col.a - pair[0].col.a);
                break;
            }
            case blend_operator::MIN:
            {
                blend_color.a = min(pair[0].col.a, pair[1].col.a);
                break;
            }
            case blend_operator::MAX:
            {
                blend_color.a = max(pair[0].col.a, pair[1].col.a);
                break;
            }
            default:
            {
                break;
            }
        }

        return blend_color;
    }
};

} // namespace math
} // namespace vx