#pragma once

#include "./vec_default.hpp"

namespace vx {
namespace math {
namespace simd {

template <size_t M, size_t N, typename T>
struct mat
{
    using scalar_type = T;
    using data_type = scalar_type[M][M];

    constexpr size_t calulate_alignment() noexcept
    {
        return alignof(scalar_type[M][N]);
    }

    static constexpr int HAVE_LOAD = 0;
    static constexpr int HAVE_STORE = 0;

    static constexpr int HAVE_ADD = 0;
    static constexpr int HAVE_SUB = 0;
    static constexpr int HAVE_MUL = 0;
    static constexpr int HAVE_DIV = 0;

    static constexpr int HAVE_TRANSPOSE = 0;
    static constexpr int HAVE_DETERMINANT = 0;
    static constexpr int HAVE_INVERSE = 0;
    static constexpr int HAVE_COMP_MUL = 0;
    static constexpr int HAVE_OUTER_PRODUCT = 0;
};

///////////////////////////////////////////////////////////////////////////////
// matrix types
///////////////////////////////////////////////////////////////////////////////

// int
using mat2x4i = mat<2, 4, i32>;
using mat3x4i = mat<3, 4, i32>;
using mat4x4i = mat<4, 4, i32>;

using mat4i = mat4x4i;

// uint
using mat2x4u = mat<2, 4, u32>;
using mat3x4u = mat<3, 4, u32>;
using mat4x4u = mat<4, 4, u32>;

using mat4u = mat4x4u;

// float
using mat2x4f = mat<2, 4, f32>;
using mat3x3f = mat<3, 3, f32>;
using mat3x4f = mat<3, 4, f32>;
using mat4x4f = mat<4, 4, f32>;

using mat2x4 = mat2x4f;
using mat3x3 = mat3x3f;
using mat3x4 = mat3x4f;
using mat4x4 = mat4x4f;

using mat3f = mat3x3f;
using mat4f = mat4x4f;

// generic
using mat3 = mat3f;
using mat4 = mat4f;

} // namespace simd
} // namespace math
} // namespace vx