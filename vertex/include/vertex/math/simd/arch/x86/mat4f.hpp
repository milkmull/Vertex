#pragma once

#include "./vec4f.hpp"
#include "../mat_default.hpp"

namespace vx {
namespace math {
namespace simd {

template <>
struct mat<4, 4, f32>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = f32;
    static constexpr size_t width = 4;
    static constexpr size_t height = 4;
    static constexpr size_t size = width * height;

    using vec_type = __m128;
    using data_type = __m128[4];

    static constexpr size_t calulate_alignment() noexcept
    {
        constexpr size_t a1 = alignof(scalar_type[width][height]);
        constexpr size_t a2 = alignof(data_type);
        return (a1 > a2) ? a1 : a2;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // add
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void add(const data_type in1, const data_type in2, data_type out) noexcept
    {
        out[0] = _mm_add_ps(in1[0], in2[0]);
        out[1] = _mm_add_ps(in1[1], in2[1]);
        out[2] = _mm_add_ps(in1[2], in2[2]);
        out[3] = _mm_add_ps(in1[3], in2[3]);
    }

    static VX_FORCE_INLINE void add(const data_type in1, scalar_type in2, data_type out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in2);

        out[0] = _mm_add_ps(in1[0], scalar);
        out[1] = _mm_add_ps(in1[1], scalar);
        out[2] = _mm_add_ps(in1[2], scalar);
        out[3] = _mm_add_ps(in1[3], scalar);
    }

    static constexpr int HAVE_ADD = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // sub
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void sub(const data_type in1, scalar_type in2, data_type out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in2);

        out[0] = _mm_sub_ps(in1[0], scalar);
        out[1] = _mm_sub_ps(in1[1], scalar);
        out[2] = _mm_sub_ps(in1[2], scalar);
        out[3] = _mm_sub_ps(in1[3], scalar);
    }

    static VX_FORCE_INLINE void sub(scalar_type in1, const data_type in2, data_type out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in1);

        out[0] = _mm_sub_ps(scalar, in2[0]);
        out[1] = _mm_sub_ps(scalar, in2[1]);
        out[2] = _mm_sub_ps(scalar, in2[2]);
        out[3] = _mm_sub_ps(scalar, in2[3]);
    }

    static VX_FORCE_INLINE void sub(const data_type in1, const data_type in2, data_type out) noexcept
    {
        out[0] = _mm_sub_ps(in1[0], in2[0]);
        out[1] = _mm_sub_ps(in1[1], in2[1]);
        out[2] = _mm_sub_ps(in1[2], in2[2]);
        out[3] = _mm_sub_ps(in1[3], in2[3]);
    }

    static constexpr int HAVE_SUB = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // mul
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void mul(const vec_type* in1, scalar_type in2, vec_type* out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in2);

        out[0] = _mm_mul_ps(in1[0], scalar);
        out[1] = _mm_mul_ps(in1[1], scalar);
        out[2] = _mm_mul_ps(in1[2], scalar);
        out[3] = _mm_mul_ps(in1[3], scalar);
    }

    static VX_FORCE_INLINE vec_type mul(const data_type m, vec_type v) noexcept
    {
        const vec_type v0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
        const vec_type v1 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
        const vec_type v2 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
        const vec_type v3 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));

        const vec_type m0 = _mm_mul_ps(m[0], v0);
        const vec_type m1 = _mm_mul_ps(m[1], v1);
        const vec_type m2 = _mm_mul_ps(m[2], v2);
        const vec_type m3 = _mm_mul_ps(m[3], v3);

        const vec_type a0 = _mm_add_ps(m0, m1);
        const vec_type a1 = _mm_add_ps(m2, m3);
        const vec_type a2 = _mm_add_ps(a0, a1);

        return a2;
    }

    static VX_FORCE_INLINE vec_type mul(vec_type v, const data_type m) noexcept
    {
        const vec_type m0 = _mm_mul_ps(v, m[0]);
        const vec_type m1 = _mm_mul_ps(v, m[1]);
        const vec_type m2 = _mm_mul_ps(v, m[2]);
        const vec_type m3 = _mm_mul_ps(v, m[3]);

        const vec_type u0 = _mm_unpacklo_ps(m0, m1);
        const vec_type u1 = _mm_unpackhi_ps(m0, m1);
        const vec_type a0 = _mm_add_ps(u0, u1);

        const vec_type u2 = _mm_unpacklo_ps(m2, m3);
        const vec_type u3 = _mm_unpackhi_ps(m2, m3);
        const vec_type a1 = _mm_add_ps(u2, u3);

        const vec_type f0 = _mm_movelh_ps(a0, a1);
        const vec_type f1 = _mm_movehl_ps(a1, a0);
        const vec_type f2 = _mm_add_ps(f0, f1);

        return f2;
    }

    //static VX_FORCE_INLINE void mul(const data_type in1, const mat2x4f_t in2, mat2x4f_t out) noexcept
    //{
    //    {
    //        const vec_type e0 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(0, 0, 0, 0));
    //        const vec_type e1 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(1, 1, 1, 1));
    //        const vec_type e2 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(2, 2, 2, 2));
    //        const vec_type e3 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(3, 3, 3, 3));
    //
    //        const vec_type m0 = _mm_mul_ps(in1[0], e0);
    //        const vec_type m1 = _mm_mul_ps(in1[1], e1);
    //        const vec_type m2 = _mm_mul_ps(in1[2], e2);
    //        const vec_type m3 = _mm_mul_ps(in1[3], e3);
    //
    //        const vec_type a0 = _mm_add_ps(m0, m1);
    //        const vec_type a1 = _mm_add_ps(m2, m3);
    //        const vec_type a2 = _mm_add_ps(a0, a1);
    //
    //        out[0] = a2;
    //    }
    //
    //    {
    //        const vec_type e0 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(0, 0, 0, 0));
    //        const vec_type e1 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(1, 1, 1, 1));
    //        const vec_type e2 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(2, 2, 2, 2));
    //        const vec_type e3 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(3, 3, 3, 3));
    //
    //        const vec_type m0 = _mm_mul_ps(in1[0], e0);
    //        const vec_type m1 = _mm_mul_ps(in1[1], e1);
    //        const vec_type m2 = _mm_mul_ps(in1[2], e2);
    //        const vec_type m3 = _mm_mul_ps(in1[3], e3);
    //
    //        const vec_type a0 = _mm_add_ps(m0, m1);
    //        const vec_type a1 = _mm_add_ps(m2, m3);
    //        const vec_type a2 = _mm_add_ps(a0, a1);
    //
    //        out[1] = a2;
    //    }
    //}
    //
    //static VX_FORCE_INLINE void mul(const data_type in1, const mat3x4f_t in2, mat3x4f_t out) noexcept
    //{
    //    {
    //        const vec_type e0 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(0, 0, 0, 0));
    //        const vec_type e1 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(1, 1, 1, 1));
    //        const vec_type e2 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(2, 2, 2, 2));
    //        const vec_type e3 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(3, 3, 3, 3));
    //
    //        const vec_type m0 = _mm_mul_ps(in1[0], e0);
    //        const vec_type m1 = _mm_mul_ps(in1[1], e1);
    //        const vec_type m2 = _mm_mul_ps(in1[2], e2);
    //        const vec_type m3 = _mm_mul_ps(in1[3], e3);
    //
    //        const vec_type a0 = _mm_add_ps(m0, m1);
    //        const vec_type a1 = _mm_add_ps(m2, m3);
    //        const vec_type a2 = _mm_add_ps(a0, a1);
    //
    //        out[0] = a2;
    //    }
    //
    //    {
    //        const vec_type e0 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(0, 0, 0, 0));
    //        const vec_type e1 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(1, 1, 1, 1));
    //        const vec_type e2 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(2, 2, 2, 2));
    //        const vec_type e3 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(3, 3, 3, 3));
    //
    //        const vec_type m0 = _mm_mul_ps(in1[0], e0);
    //        const vec_type m1 = _mm_mul_ps(in1[1], e1);
    //        const vec_type m2 = _mm_mul_ps(in1[2], e2);
    //        const vec_type m3 = _mm_mul_ps(in1[3], e3);
    //
    //        const vec_type a0 = _mm_add_ps(m0, m1);
    //        const vec_type a1 = _mm_add_ps(m2, m3);
    //        const vec_type a2 = _mm_add_ps(a0, a1);
    //
    //        out[1] = a2;
    //    }
    //
    //    {
    //        const vec_type e0 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(0, 0, 0, 0));
    //        const vec_type e1 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(1, 1, 1, 1));
    //        const vec_type e2 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(2, 2, 2, 2));
    //        const vec_type e3 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(3, 3, 3, 3));
    //
    //        const vec_type m0 = _mm_mul_ps(in1[0], e0);
    //        const vec_type m1 = _mm_mul_ps(in1[1], e1);
    //        const vec_type m2 = _mm_mul_ps(in1[2], e2);
    //        const vec_type m3 = _mm_mul_ps(in1[3], e3);
    //
    //        const vec_type a0 = _mm_add_ps(m0, m1);
    //        const vec_type a1 = _mm_add_ps(m2, m3);
    //        const vec_type a2 = _mm_add_ps(a0, a1);
    //
    //        out[2] = a2;
    //    }
    //}

    static VX_FORCE_INLINE void mul(const vec_type* in1, const vec_type* in2, vec_type* out) noexcept
    {
        {
            const vec_type e0 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type e1 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type e2 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type e3 = _mm_shuffle_ps(in2[0], in2[0], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type m0 = _mm_mul_ps(in1[0], e0);
            const vec_type m1 = _mm_mul_ps(in1[1], e1);
            const vec_type m2 = _mm_mul_ps(in1[2], e2);
            const vec_type m3 = _mm_mul_ps(in1[3], e3);

            const vec_type a0 = _mm_add_ps(m0, m1);
            const vec_type a1 = _mm_add_ps(m2, m3);
            const vec_type a2 = _mm_add_ps(a0, a1);

            out[0] = a2;
        }

        {
            const vec_type e0 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type e1 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type e2 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type e3 = _mm_shuffle_ps(in2[1], in2[1], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type m0 = _mm_mul_ps(in1[0], e0);
            const vec_type m1 = _mm_mul_ps(in1[1], e1);
            const vec_type m2 = _mm_mul_ps(in1[2], e2);
            const vec_type m3 = _mm_mul_ps(in1[3], e3);

            const vec_type a0 = _mm_add_ps(m0, m1);
            const vec_type a1 = _mm_add_ps(m2, m3);
            const vec_type a2 = _mm_add_ps(a0, a1);

            out[1] = a2;
        }

        {
            const vec_type e0 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type e1 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type e2 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type e3 = _mm_shuffle_ps(in2[2], in2[2], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type m0 = _mm_mul_ps(in1[0], e0);
            const vec_type m1 = _mm_mul_ps(in1[1], e1);
            const vec_type m2 = _mm_mul_ps(in1[2], e2);
            const vec_type m3 = _mm_mul_ps(in1[3], e3);

            const vec_type a0 = _mm_add_ps(m0, m1);
            const vec_type a1 = _mm_add_ps(m2, m3);
            const vec_type a2 = _mm_add_ps(a0, a1);

            out[2] = a2;
        }

        {
            const vec_type e0 = _mm_shuffle_ps(in2[3], in2[3], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type e1 = _mm_shuffle_ps(in2[3], in2[3], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type e2 = _mm_shuffle_ps(in2[3], in2[3], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type e3 = _mm_shuffle_ps(in2[3], in2[3], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type m0 = _mm_mul_ps(in1[0], e0);
            const vec_type m1 = _mm_mul_ps(in1[1], e1);
            const vec_type m2 = _mm_mul_ps(in1[2], e2);
            const vec_type m3 = _mm_mul_ps(in1[3], e3);

            const vec_type a0 = _mm_add_ps(m0, m1);
            const vec_type a1 = _mm_add_ps(m2, m3);
            const vec_type a2 = _mm_add_ps(a0, a1);

            out[3] = a2;
        }
    }

    static constexpr int HAVE_MUL = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // div
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void div(const data_type in1, scalar_type in2, data_type out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in2);

        out[0] = _mm_div_ps(in1[0], scalar);
        out[1] = _mm_div_ps(in1[1], scalar);
        out[2] = _mm_div_ps(in1[2], scalar);
        out[3] = _mm_div_ps(in1[3], scalar);
    }

    static VX_FORCE_INLINE void div(scalar_type in1, const data_type in2, data_type out) noexcept
    {
        const vec_type scalar = _mm_set1_ps(in1);

        out[0] = _mm_div_ps(scalar, in2[0]);
        out[1] = _mm_div_ps(scalar, in2[1]);
        out[2] = _mm_div_ps(scalar, in2[2]);
        out[3] = _mm_div_ps(scalar, in2[3]);
    }

    static constexpr int HAVE_DIV = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // transpose
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void transpose(const data_type in, data_type out) noexcept
    {
        const vec_type tmp0 = _mm_shuffle_ps(in[0], in[1], 0x44);
        const vec_type tmp2 = _mm_shuffle_ps(in[0], in[1], 0xEE);
        const vec_type tmp1 = _mm_shuffle_ps(in[2], in[3], 0x44);
        const vec_type tmp3 = _mm_shuffle_ps(in[2], in[3], 0xEE);

        out[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
        out[1] = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
        out[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
        out[3] = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    }

    static constexpr int HAVE_TRANSPOSE = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // determinant
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/g-truc/glm/blob/master/glm/simd/matrix.h#L458

    static VX_FORCE_INLINE scalar_type determinant(const vec_type* m) noexcept
    {
        //  const T subfac00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        //  const T subfac01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        //  const T subfac02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        //  const T subfac03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        //  const T subfac04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        //  const T subfac05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        // First 2 columns
        const vec_type swp2a = _mm_shuffle_ps(m[2], m[2], _MM_SHUFFLE(0, 1, 1, 2));
        const vec_type swp3a = _mm_shuffle_ps(m[3], m[3], _MM_SHUFFLE(3, 2, 3, 3));
        const vec_type mula = _mm_mul_ps(swp2a, swp3a);

        // Second 2 columns
        const vec_type swp2b = _mm_shuffle_ps(m[2], m[2], _MM_SHUFFLE(3, 2, 3, 3));
        const vec_type swp3b = _mm_shuffle_ps(m[3], m[3], _MM_SHUFFLE(0, 1, 1, 2));
        const vec_type mulb = _mm_mul_ps(swp2b, swp3b);

        // Columns subtraction
        const vec_type sube = _mm_sub_ps(mula, mulb);

        // Last 2 rows
        const vec_type swp2c = _mm_shuffle_ps(m[2], m[2], _MM_SHUFFLE(0, 0, 1, 2));
        const vec_type swp3c = _mm_shuffle_ps(m[3], m[3], _MM_SHUFFLE(1, 2, 0, 0));
        const vec_type mulc = _mm_mul_ps(swp2c, swp3c);
        const vec_type subf = _mm_sub_ps(_mm_movehl_ps(mulc, mulc), mulc);

        // vec<4, T> defcof(
        // + (m[1][1] * subfac00 - m[1][2] * subfac01 + m[1][3] * subfac02),
        // - (m[1][0] * subfac00 - m[1][2] * subfac03 + m[1][3] * subfac04),
        // + (m[1][0] * subfac01 - m[1][1] * subfac03 + m[1][3] * subfac05),
        // - (m[1][0] * subfac02 - m[1][1] * subfac04 + m[1][2] * subfac05)
        // );

        const vec_type subfaca = _mm_shuffle_ps(sube, sube, _MM_SHUFFLE(2, 1, 0, 0));
        const vec_type swpfaca = _mm_shuffle_ps(m[1], m[1], _MM_SHUFFLE(0, 0, 0, 1));
        const vec_type mulfaca = _mm_mul_ps(swpfaca, subfaca);

        const vec_type subtmpb = _mm_shuffle_ps(sube, subf, _MM_SHUFFLE(0, 0, 3, 1));
        const vec_type subfacb = _mm_shuffle_ps(subtmpb, subtmpb, _MM_SHUFFLE(3, 1, 1, 0)); // subf[0], sube[3], sube[3], sube[1];
        const vec_type swpfacb = _mm_shuffle_ps(m[1], m[1], _MM_SHUFFLE(1, 1, 2, 2));
        const vec_type mulfacb = _mm_mul_ps(swpfacb, subfacb);

        const vec_type subres = _mm_sub_ps(mulfaca, mulfacb);

        const vec_type subtmpc = _mm_shuffle_ps(sube, subf, _MM_SHUFFLE(1, 0, 2, 2));
        const vec_type subfacc = _mm_shuffle_ps(subtmpc, subtmpc, _MM_SHUFFLE(3, 3, 2, 0));
        const vec_type swpfacc = _mm_shuffle_ps(m[1], m[1], _MM_SHUFFLE(2, 3, 3, 3));
        const vec_type mulfacc = _mm_mul_ps(swpfacc, subfacc);

        const vec_type addres = _mm_add_ps(subres, mulfacc);
        const vec_type detcof = _mm_mul_ps(addres, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

        // return (
        //   + m[0][0] * detcof[0]
        //  + m[0][1] * detcof[1]
        //  + m[0][2] * detcof[2]
        //  + m[0][3] * detcof[3]
        // );

        const vec_type dot = vec4f::dot(m[0], detcof);
        return _mm_cvtss_f32(dot);
    }

    //static constexpr int HAVE_DETERMINANT = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // inverse
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/g-truc/glm/blob/master/glm/simd/matrix.h#L521

    static VX_FORCE_INLINE void inverse(const vec_type* in, vec_type* out) noexcept
    {
        vec_type fac0;
        {
            // const T subfac00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // const T subfac00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // const T subfac06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
            // const T subfac13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac0 = _mm_sub_ps(mul00, mul01);
        }

        vec_type fac1;
        {
            // const T subfac01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // const T subfac01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // const T subfac07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
            // const T subfac14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac1 = _mm_sub_ps(mul00, mul01);
        }


        vec_type fac2;
        {
            // const T subfac02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            // const T subfac02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            // const T subfac08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
            // const T subfac15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac2 = _mm_sub_ps(mul00, mul01);
        }

        vec_type fac3;
        {
            // const T subfac03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // const T subfac03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // const T subfac09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
            // const T subfac16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(3, 3, 3, 3));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(3, 3, 3, 3));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac3 = _mm_sub_ps(mul00, mul01);
        }

        vec_type fac4;
        {
            // const T subfac04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // const T subfac04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // const T subfac10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
            // const T subfac17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(2, 2, 2, 2));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(2, 2, 2, 2));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac4 = _mm_sub_ps(mul00, mul01);
        }

        vec_type fac5;
        {
            // const T subfac05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
            // const T subfac05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
            // const T subfac12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
            // const T subfac18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

            const vec_type swp0a = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(1, 1, 1, 1));
            const vec_type swp0b = _mm_shuffle_ps(in[3], in[2], _MM_SHUFFLE(0, 0, 0, 0));

            const vec_type swp00 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(0, 0, 0, 0));
            const vec_type swp01 = _mm_shuffle_ps(swp0a, swp0a, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp02 = _mm_shuffle_ps(swp0b, swp0b, _MM_SHUFFLE(2, 0, 0, 0));
            const vec_type swp03 = _mm_shuffle_ps(in[2], in[1], _MM_SHUFFLE(1, 1, 1, 1));

            const vec_type mul00 = _mm_mul_ps(swp00, swp01);
            const vec_type mul01 = _mm_mul_ps(swp02, swp03);
            fac5 = _mm_sub_ps(mul00, mul01);
        }

        const vec_type signa = _mm_set_ps(1.0f, -1.0f, 1.0f, -1.0f);
        const vec_type signb = _mm_set_ps(-1.0f, 1.0f, -1.0f, 1.0f);

        // m[1][0]
        // m[0][0]
        // m[0][0]
        // m[0][0]
        const vec_type temp0 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(0, 0, 0, 0));
        const vec_type vec0 = _mm_shuffle_ps(temp0, temp0, _MM_SHUFFLE(2, 2, 2, 0));

        // m[1][1]
        // m[0][1]
        // m[0][1]
        // m[0][1]
        const vec_type temp1 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(1, 1, 1, 1));
        const vec_type vec1 = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2, 2, 2, 0));

        // m[1][2]
        // m[0][2]
        // m[0][2]
        // m[0][2]
        const vec_type temp2 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(2, 2, 2, 2));
        const vec_type vec2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(2, 2, 2, 0));

        // m[1][3]
        // m[0][3]
        // m[0][3]
        // m[0][3]
        const vec_type temp3 = _mm_shuffle_ps(in[1], in[0], _MM_SHUFFLE(3, 3, 3, 3));
        const vec_type vec3 = _mm_shuffle_ps(temp3, temp3, _MM_SHUFFLE(2, 2, 2, 0));

        // col0
        // + (vec1[0] * fac0[0] - vec2[0] * fac1[0] + vec3[0] * fac2[0]),
        // - (vec1[1] * fac0[1] - vec2[1] * fac1[1] + vec3[1] * fac2[1]),
        // + (vec1[2] * fac0[2] - vec2[2] * fac1[2] + vec3[2] * fac2[2]),
        // - (vec1[3] * fac0[3] - vec2[3] * fac1[3] + vec3[3] * fac2[3]),
        const vec_type mul00 = _mm_mul_ps(vec1, fac0);
        const vec_type mul01 = _mm_mul_ps(vec2, fac1);
        const vec_type mul02 = _mm_mul_ps(vec3, fac2);
        const vec_type sub00 = _mm_sub_ps(mul00, mul01);
        const vec_type add00 = _mm_add_ps(sub00, mul02);
        const vec_type inv0 = _mm_mul_ps(signb, add00);

        // col1
        // - (vec0[0] * fac0[0] - vec2[0] * fac3[0] + vec3[0] * fac4[0]),
        // + (vec0[0] * fac0[1] - vec2[1] * fac3[1] + vec3[1] * fac4[1]),
        // - (vec0[0] * fac0[2] - vec2[2] * fac3[2] + vec3[2] * fac4[2]),
        // + (vec0[0] * fac0[3] - vec2[3] * fac3[3] + vec3[3] * fac4[3]),
        const vec_type mul03 = _mm_mul_ps(vec0, fac0);
        const vec_type mul04 = _mm_mul_ps(vec2, fac3);
        const vec_type mul05 = _mm_mul_ps(vec3, fac4);
        const vec_type sub01 = _mm_sub_ps(mul03, mul04);
        const vec_type add01 = _mm_add_ps(sub01, mul05);
        const vec_type inv1 = _mm_mul_ps(signa, add01);

        // col2
        // + (vec0[0] * fac1[0] - vec1[0] * fac3[0] + vec3[0] * fac5[0]),
        // - (vec0[0] * fac1[1] - vec1[1] * fac3[1] + vec3[1] * fac5[1]),
        // + (vec0[0] * fac1[2] - vec1[2] * fac3[2] + vec3[2] * fac5[2]),
        // - (vec0[0] * fac1[3] - vec1[3] * fac3[3] + vec3[3] * fac5[3]),
        const vec_type mul06 = _mm_mul_ps(vec0, fac1);
        const vec_type mul07 = _mm_mul_ps(vec1, fac3);
        const vec_type mul08 = _mm_mul_ps(vec3, fac5);
        const vec_type sub02 = _mm_sub_ps(mul06, mul07);
        const vec_type add02 = _mm_add_ps(sub02, mul08);
        const vec_type inv2 = _mm_mul_ps(signb, add02);

        // col3
        // - (vec1[0] * fac2[0] - vec1[0] * fac4[0] + vec2[0] * fac5[0]),
        // + (vec1[0] * fac2[1] - vec1[1] * fac4[1] + vec2[1] * fac5[1]),
        // - (vec1[0] * fac2[2] - vec1[2] * fac4[2] + vec2[2] * fac5[2]),
        // + (vec1[0] * fac2[3] - vec1[3] * fac4[3] + vec2[3] * fac5[3]));
        const vec_type mul09 = _mm_mul_ps(vec0, fac2);
        const vec_type mul10 = _mm_mul_ps(vec1, fac4);
        const vec_type mul11 = _mm_mul_ps(vec2, fac5);
        const vec_type sub03 = _mm_sub_ps(mul09, mul10);
        const vec_type add03 = _mm_add_ps(sub03, mul11);
        const vec_type inv3 = _mm_mul_ps(signa, add03);

        const vec_type row0 = _mm_shuffle_ps(inv0, inv1, _MM_SHUFFLE(0, 0, 0, 0));
        const vec_type row1 = _mm_shuffle_ps(inv2, inv3, _MM_SHUFFLE(0, 0, 0, 0));
        const vec_type row2 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(2, 0, 2, 0));

        // const T determinant = (
        //     - m[0][0] * inverse[0][0]
        //     + m[0][1] * inverse[1][0]
        //     + m[0][2] * inverse[2][0]
        //     + m[0][3] * inverse[3][0]
        // );

        const vec_type det0 = vec4f::dot(in[0], row2);
        const vec_type rcp0 = _mm_div_ps(_mm_set1_ps(1.0f), det0);

        // inverse /= determinant;
        out[0] = _mm_mul_ps(inv0, rcp0);
        out[1] = _mm_mul_ps(inv1, rcp0);
        out[2] = _mm_mul_ps(inv2, rcp0);
        out[3] = _mm_mul_ps(inv3, rcp0);
    }

    //static constexpr int HAVE_INVERSE = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // componant multiplication
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void comp_mult(const data_type in1, const data_type in2, data_type out) noexcept
    {
        out[0] = _mm_mul_ps(in1[0], in2[0]);
        out[1] = _mm_mul_ps(in1[1], in2[1]);
        out[2] = _mm_mul_ps(in1[2], in2[2]);
        out[3] = _mm_mul_ps(in1[3], in2[3]);
    }

    static constexpr int HAVE_COMP_MUL = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // outer product
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/g-truc/glm/blob/master/glm/simd/matrix.h#L1032

    static VX_FORCE_INLINE void outer_product(const vec_type c, const vec_type r, data_type out) noexcept
    {
        out[0] = _mm_mul_ps(c, _mm_shuffle_ps(r, r, _MM_SHUFFLE(0, 0, 0, 0)));
        out[1] = _mm_mul_ps(c, _mm_shuffle_ps(r, r, _MM_SHUFFLE(1, 1, 1, 1)));
        out[2] = _mm_mul_ps(c, _mm_shuffle_ps(r, r, _MM_SHUFFLE(2, 2, 2, 2)));
        out[3] = _mm_mul_ps(c, _mm_shuffle_ps(r, r, _MM_SHUFFLE(3, 3, 3, 3)));
    }

    static constexpr int HAVE_OUTER_PRODUCT = 1;

}; // mat4f_op

} // namespace simd
} // namespace math
} // namespace vx