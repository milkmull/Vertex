#pragma once

#include "./config.hpp"

namespace vx {
namespace math {
namespace simd {

template <size_t L, typename T>
struct vec
{
    using scalar_type = T;
    using data_type = scalar_type[L];

    static constexpr size_t calulate_alignment() noexcept
    {
        return alignof(data_type);
    }

    static constexpr int HAVE_LOAD = 0;
    static constexpr int HAVE_STORE = 0;

    static constexpr int HAVE_CMP_EQ = 0;
    static constexpr int HAVE_CMP_NEQ = 0;
    static constexpr int HAVE_CMP_LT = 0;
    static constexpr int HAVE_CMP_LE = 0;
    static constexpr int HAVE_CMP_GT = 0;
    static constexpr int HAVE_CMP_GE = 0;

    static constexpr int HAVE_ADD = 0;
    static constexpr int HAVE_SUB = 0;
    static constexpr int HAVE_MUL = 0;
    static constexpr int HAVE_DIV = 0;
    static constexpr int HAVE_DIV_LOWP = 0;

    static constexpr int HAVE_AND = 0;
    static constexpr int HAVE_OR  = 0;
    static constexpr int HAVE_XOR = 0;

    static constexpr int HAVE_SHUFFLE = 0;

    static constexpr int HAVE_MIN = 0;
    static constexpr int HAVE_MAX = 0;

    static constexpr int HAVE_ABS = 0;
    static constexpr int HAVE_NEG = 0;

    static constexpr int HAVE_ROUND = 0;
    static constexpr int HAVE_TRUNC = 0;
    static constexpr int HAVE_FLOOR = 0;
    static constexpr int HAVE_CEIL = 0;
    static constexpr int HAVE_SIGN = 0;
    static constexpr int HAVE_FRACT = 0;

    static constexpr int HAVE_MOD = 0;
    static constexpr int HAVE_FMA = 0;
    static constexpr int HAVE_CLAMP = 0;
    static constexpr int HAVE_SELECT = 0;

    static constexpr int HAVE_INVERSE = 0;
    static constexpr int HAVE_INVERSE_LOWP = 0;

    static constexpr int HAVE_INVERSE_SQRT = 0;
    static constexpr int HAVE_INVERSE_SQRT_LOWP = 0;

    static constexpr int HAVE_SQRT = 0;
    static constexpr int HAVE_SQRT_LOWP = 0;

    static constexpr int HAVE_DOT = 0;
    static constexpr int HAVE_CROSS = 0;

    static constexpr int HAVE_LENGTH = 0;
    static constexpr int HAVE_DISTANCE = 0;

    static constexpr int HAVE_NORMALIZE = 0;
    static constexpr int HAVE_NORMALIZE_LOWP = 0;

    static constexpr int HAVE_PROJECT = 0;
    static constexpr int HAVE_REFLECT = 0;
    static constexpr int HAVE_REFRACT = 0;
    static constexpr int HAVE_FACEFORWARD = 0;

    static constexpr int HAVE_LERP = 0;
    static constexpr int HAVE_MIX = 0;
    static constexpr int HAVE_STEP = 0;
    static constexpr int HAVE_SMOOTHSTEP = 0;
};

///////////////////////////////////////////////////////////////////////////////
// vector types
///////////////////////////////////////////////////////////////////////////////

using vec4i = vec<4, i32>;
using vec4u = vec<4, u32>;
using vec4f = vec<4, f32>;
using vec4 = vec4f;

} // namespace simd
} // namespace math
} // namespace vx