#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"
#include "vertex/std/_priv/min_max.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// sort impl
//=============================================================================

namespace _sort {

enum min_max_mode
{
    mode_min = 1 << 0,
    mode_max = 1 << 1,
    mode_both = mode_min | mode_max,
};

template <typename Base>
struct traits_scalar : Base
{
    static constexpr bool vectorized = false;
    static constexpr size_t tail_mask = 0;
    static constexpr bool has_unsigned_cmp = false;
    using vec_t = void;
};

#if defined(USE_ARM_NEON)

struct traits_neon_base
{
    static constexpr bool vectorized = true;
    static constexpr size_t vec_size = 16;
    static constexpr size_t vec_mask = 0xF;
    static constexpr size_t tail_mask = 0;
    static constexpr bool has_unsigned_cmp = true;

    static void exit_vectorized() noexcept
    {}
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_avx_base
{
    using guard = zeroupper_on_exit;
    static constexpr bool vectorized = true;
    static constexpr size_t vec_size = 32;
    static constexpr size_t vec_mask = 0x1F;
    static constexpr bool has_unsigned_cmp = false;

    static __m256i zero() noexcept
    {
        return _mm256_setzero_si256();
    }

    static __m256i all_ones() noexcept
    {
        return _mm256_set1_epi8(static_cast<char>(0xFF));
    }

    static __m256i blend(const __m256i px1, const __m256i px2, const __m256i msk) noexcept
    {
        return _mm256_blendv_epi8(px1, px2, msk);
    }

    static unsigned long mask(const __m256i val) noexcept
    {
        return _mm256_movemask_epi8(val);
    }

    static void exit_vectorized() noexcept
    {
        _mm256_zeroupper();
    }

    static unsigned long get_first_h_pos(const unsigned long mask) noexcept
    {
        return _tzcnt_u32(mask);
    }

    static unsigned long get_last_h_pos(const unsigned long mask) noexcept
    {
        return 31 - _lzcnt_u32(mask);
    }
};

struct traits_avx_i_base : traits_avx_base
{
    static constexpr size_t tail_mask = 0x1C;
    using vec_t = __m256i;

    static __m256i blendval(const __m256i px1, const __m256i px2, const __m256i msk) noexcept
    {
        return _mm256_blendv_epi8(px1, px2, msk);
    }

    static __m256i load_mask(const void* const src, const __m256i mask) noexcept
    {
        return _mm256_maskload_epi32(reinterpret_cast<const int*>(src), mask);
    }
};

    #elif defined(USE_SSE2)

struct traits_sse_base
{
    using guard = char;
    static constexpr bool vectorized = true;
    static constexpr size_t vec_size = 16;
    static constexpr size_t vec_mask = 0xF;
    static constexpr size_t tail_mask = 0;
    static constexpr bool has_unsigned_cmp = false;
    using vec_t = __m128i;

    static __m128i zero() noexcept
    {
        return _mm_setzero_si128();
    }

    static __m128i all_ones() noexcept
    {
        return _mm_set1_epi8(static_cast<char>(0xFF));
    }

    static __m128i blend(const __m128i px1, const __m128i px2, const __m128i msk) noexcept
    {
        return _mm_blendv_epi8(px1, px2, msk);
    }

    static unsigned long mask(const __m128i val) noexcept
    {
        return _mm_movemask_epi8(val);
    }

    static void exit_vectorized() noexcept
    {}

    static unsigned long get_first_h_pos(const unsigned long mask) noexcept
    {
        unsigned long h_pos;
        // CodeQL [SM02313] h_pos is always initialized: element exists, so mask != 0.
        _BitScanForward(&h_pos, mask);
        return h_pos;
    }

    static unsigned long get_last_h_pos(const unsigned long mask) noexcept
    {
        unsigned long h_pos;
        // CodeQL [SM02313] h_pos is always initialized: element exists, so mask != 0.
        _BitScanReverse(&h_pos, mask);
        return h_pos;
    }
};

    #endif

#endif

struct traits_1_base
{
    static constexpr bool is_floating = false;

    using signed_t = int8_t;
    using unsigned_t = uint8_t;

    static constexpr signed_t init_min_val = static_cast<signed_t>(0x7F);
    static constexpr signed_t init_max_val = static_cast<signed_t>(0x80);

    using minmax_i_t = _priv::min_max_1i;
    using minmax_u_t = _priv::min_max_1u;

    static constexpr bool has_portion_max = true;
    static constexpr size_t portion_max = 256;
};

#if defined(USE_ARM_NEON)

struct traits_1_neon : traits_1_base, traits_neon_base
{
    using vec_t = int8x16_t;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    static vec_t zero() noexcept
    {
        return vdupq_n_s8(0);
    }

    static vec_t all_ones() noexcept
    {
        return vdupq_n_s8(static_cast<int8_t>(0xFF));
    }

    static vec_t blend(const vec_t px1, const vec_t px2, const vec_t msk) noexcept
    {
        return vbslq_s8(vreinterpretq_u8_s8(msk), px2, px1);
    }

    // Compresses a 128-bit Mask of 16 8-bit values into a 64-bit Mask of 16 4-bit values.
    static uint64_t mask(const vec_t val) noexcept
    {
        const uint8x8_t res = vshrn_n_u16(vreinterpretq_u16_s8(val), 4);
        return vget_lane_u64(vreinterpret_u64_u8(res), 0);
    }

    static uint64_t match_mask(const vec_t val_lo, const vec_t val_hi) noexcept
    {
        const uint64x2_t val = vreinterpretq_u64_s8(vorrq_s8(val_lo, val_hi));
        return vgetq_lane_u64(vpaddq_u64(val, val), 0);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return _CountTrailingZeros64(mask) >> 2;
    }

    static unsigned long get_last_h_pos(const uint64_t mask) noexcept
    {
        return 15 - (_CountLeadingZeros64(mask) >> 2);
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_s8(reinterpret_cast<const int8_t*>(src));
    }

    static vec_t inc(const vec_t idx) noexcept
    {
        return vaddq_s8(idx, vdupq_n_s8(1));
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        return vdupq_n_s8(vminvq_s8(cur));
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        return vdupq_n_s8(vmaxvq_s8(cur));
    }

    static vec_t h_min_u(const vec_t cur) noexcept
    {
        const uint8_t mx = vminvq_u8(vreinterpretq_u8_s8(cur));
        return vreinterpretq_s8_u8(vdupq_n_u8(mx));
    }

    static vec_t h_max_u(const vec_t cur) noexcept
    {
        const uint8_t mx = vmaxvq_u8(vreinterpretq_u8_s8(cur));
        return vreinterpretq_s8_u8(vdupq_n_u8(mx));
    }

    static signed_t get_any(const vec_t cur) noexcept
    {
        return static_cast<signed_t>(vgetq_lane_s8(cur, 0));
    }

    static unsigned_t get_v_pos(const vec_t cur) noexcept
    {
        return static_cast<unsigned_t>(get_any(cur));
    }

    static vec_t cmp_eq(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s8_u8(vceqq_s8(first, second));
    }

    static vec_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s8_u8(vcgtq_s8(first, second));
    }

    static vec_t cmp_gt_u(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s8_u8(vcgtq_u8(vreinterpretq_u8_s8(first), vreinterpretq_u8_s8(second)));
    }

    static vec_t cmp_eq_idx(const vec_t first, const vec_t second) noexcept
    {
        return cmp_eq(first, second);
    }

    static vec_t min(const vec_t first, const vec_t second, vec_t = vdupq_n_s8(0)) noexcept
    {
        return vminq_s8(first, second);
    }

    static vec_t max(const vec_t first, const vec_t second, vec_t = vdupq_n_s8(0)) noexcept
    {
        return vmaxq_s8(first, second);
    }

    static vec_t min_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s8(0)) noexcept
    {
        const uint8x16_t Rx = vminq_u8(vreinterpretq_u8_s8(first), vreinterpretq_u8_s8(second));
        return vreinterpretq_s8_u8(Rx);
    }

    static vec_t max_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s8(0)) noexcept
    {
        const uint8x16_t Rx = vmaxq_u8(vreinterpretq_u8_s8(first), vreinterpretq_u8_s8(second));
        return vreinterpretq_s8_u8(Rx);
    }

    static vec_t mask_cast(const vec_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_1_avx : traits_1_base, traits_avx_i_base
{
    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i sign_correction(const __m256i val, const bool sign) noexcept
    {
        alignas(32) static constexpr unsigned_t sign_corrections[2][32] = {
            { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },
            {}
        };
        return _mm256_sub_epi8(
            val,
            _mm256_load_si256(reinterpret_cast<const __m256i*>(sign_corrections[sign])));
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi8(idx, _mm256_set1_epi8(1));
    }

    template <typename Fn>
    static __m256i h_func(const __m256i cur, const Fn funct) noexcept
    {
        const __m128i shuf_b = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
        const __m128i shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m256i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm256_permute4x64_epi64(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi8(h_min_val, _mm256_broadcastsi128_si256(shuf_w)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi8(h_min_val, _mm256_broadcastsi128_si256(shuf_b)));
        return h_min_val;
    }

    static __m256i h_min(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epi8(val1, val2); });
    }

    static __m256i h_max(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epi8(val1, val2); });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epu8(val1, val2); });
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epu8(val1, val2); });
    }

    static signed_t get_any(const __m256i cur) noexcept
    {
        return static_cast<signed_t>(_mm256_cvtsi256_si32(cur));
    }

    static unsigned_t get_v_pos(const __m256i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m256i cmp_eq(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi8(first, second);
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi8(first, second);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi8(first, second);
    }

    static __m256i min(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_min_epi8(first, second);
    }

    static __m256i max(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_max_epi8(first, second);
    }

    static __m256i min_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_min_epu8(first, second);
    }

    static __m256i max_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_max_epu8(first, second);
    }

    static __m256i mask_cast(const __m256i mask) noexcept
    {
        return mask;
    }
};

    #elif defined(USE_SSE2)

struct traits_1_sse : traits_1_base, traits_sse_base
{
    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static __m128i sign_correction(const __m128i val, const bool sign) noexcept
    {
        alignas(16) static constexpr unsigned_t sign_corrections[2][16] = {
            { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },
            {}
        };
        return _mm_sub_epi8(val, _mm_load_si128(reinterpret_cast<const __m128i*>(sign_corrections[sign])));
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi8(idx, _mm_set1_epi8(1));
    }

    template <typename Fn>
    static __m128i h_func(const __m128i cur, const Fn funct) noexcept
    {
        const __m128i shuf_b = _mm_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
        const __m128i shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m128i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        h_min_val = funct(h_min_val, _mm_shuffle_epi8(h_min_val, shuf_w));
        h_min_val = funct(h_min_val, _mm_shuffle_epi8(h_min_val, shuf_b));
        return h_min_val;
    }

    static __m128i h_min(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epi8(val1, val2); });
    }

    static __m128i h_max(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epi8(val1, val2); });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epu8(val1, val2); });
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epu8(val1, val2); });
    }

    static signed_t get_any(const __m128i cur) noexcept
    {
        return static_cast<signed_t>(_mm_cvtsi128_si32(cur));
    }

    static unsigned_t get_v_pos(const __m128i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m128i cmp_eq(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi8(first, second);
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi8(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi8(first, second);
    }

    static __m128i min(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_min_epi8(first, second);
    }

    static __m128i max(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_max_epi8(first, second);
    }

    static __m128i min_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_min_epu8(first, second);
    }

    static __m128i max_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_max_epu8(first, second);
    }

    static __m128i mask_cast(const __m128i mask) noexcept
    {
        return mask;
    }
};

    #endif

#endif

struct traits_2_base
{
    static constexpr bool is_floating = false;

    using signed_t = int16_t;
    using unsigned_t = uint16_t;

    static constexpr signed_t init_min_val = static_cast<signed_t>(0x7FFF);
    static constexpr signed_t init_max_val = static_cast<signed_t>(0x8000);

    using minmax_i_t = _priv::min_max_2i;
    using minmax_u_t = _priv::min_max_2u;

    static constexpr bool has_portion_max = true;
    static constexpr size_t portion_max = 65536;
};

#if defined(USE_ARM_NEON)

struct traits_2_neon : traits_2_base, traits_neon_base
{
    using vec_t = int16x8_t;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    static vec_t zero() noexcept
    {
        return vdupq_n_s16(0);
    }

    static vec_t all_ones() noexcept
    {
        return vreinterpretq_s16_s8(vdupq_n_s8(static_cast<int8_t>(0xFF)));
    }

    static vec_t blend(const vec_t px1, const vec_t px2, const vec_t msk) noexcept
    {
        return vbslq_s16(vreinterpretq_u16_s16(msk), px2, px1);
    }

    // Compresses a 128-bit Mask of 8 16-bit values into a 64-bit Mask of 8 8-bit values.
    static uint64_t mask(const vec_t val) noexcept
    {
        const uint16x4_t res = vshrn_n_u32(vreinterpretq_u32_s16(val), 8);
        return vget_lane_u64(vreinterpret_u64_u16(res), 0);
    }

    static uint64_t match_mask(const vec_t val_lo, const vec_t val_hi) noexcept
    {
        const int8x8_t val = vaddhn_s16(val_lo, val_hi);
        return vget_lane_u64(vreinterpret_u64_s8(val), 0);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return _CountTrailingZeros64(mask) >> 2;
    }

    static unsigned long get_last_h_pos(const uint64_t mask) noexcept
    {
        return 15 - (_CountLeadingZeros64(mask) >> 2);
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_s16(reinterpret_cast<const int16_t*>(src));
    }

    static vec_t inc(const vec_t idx) noexcept
    {
        return vaddq_s16(idx, vdupq_n_s16(1));
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        return vdupq_n_s16(vminvq_s16(cur));
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        return vdupq_n_s16(vmaxvq_s16(cur));
    }

    static vec_t h_min_u(const vec_t cur) noexcept
    {
        const uint16_t mx = vminvq_u16(vreinterpretq_u16_s16(cur));
        return vreinterpretq_s16_u16(vdupq_n_u16(mx));
    }

    static vec_t h_max_u(const vec_t cur) noexcept
    {
        const uint16_t mx = vmaxvq_u16(vreinterpretq_u16_s16(cur));
        return vreinterpretq_s16_u16(vdupq_n_u16(mx));
    }

    static signed_t get_any(const vec_t cur) noexcept
    {
        return static_cast<signed_t>(vgetq_lane_s16(cur, 0));
    }

    static unsigned_t get_v_pos(const vec_t cur) noexcept
    {
        return static_cast<unsigned_t>(get_any(cur));
    }

    static vec_t cmp_eq(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s16_u16(vceqq_s16(first, second));
    }

    static vec_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s16_u16(vcgtq_s16(first, second));
    }

    static vec_t cmp_gt_u(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s16_u16(vcgtq_u16(vreinterpretq_u16_s16(first), vreinterpretq_u16_s16(second)));
    }

    static vec_t cmp_eq_idx(const vec_t first, const vec_t second) noexcept
    {
        return cmp_eq(first, second);
    }

    static vec_t min(const vec_t first, const vec_t second, vec_t = vdupq_n_s16(0)) noexcept
    {
        return vminq_s16(first, second);
    }

    static vec_t max(const vec_t first, const vec_t second, vec_t = vdupq_n_s16(0)) noexcept
    {
        return vmaxq_s16(first, second);
    }

    static vec_t min_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s16(0)) noexcept
    {
        const uint16x8_t Rx = vminq_u16(vreinterpretq_u16_s16(first), vreinterpretq_u16_s16(second));
        return vreinterpretq_s16_u16(Rx);
    }

    static vec_t max_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s16(0)) noexcept
    {
        const uint16x8_t Rx = vmaxq_u16(vreinterpretq_u16_s16(first), vreinterpretq_u16_s16(second));
        return vreinterpretq_s16_u16(Rx);
    }

    static vec_t mask_cast(const vec_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_2_avx : traits_2_base, traits_avx_i_base
{
    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i sign_correction(const __m256i val, const bool sign) noexcept
    {
        alignas(32) static constexpr unsigned_t sign_corrections[2][16] = { 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, {} };
        return _mm256_sub_epi16(
            val,
            _mm256_load_si256(reinterpret_cast<const __m256i*>(sign_corrections[sign])));
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi16(idx, _mm256_set1_epi16(1));
    }

    template <typename Fn>
    static __m256i h_func(const __m256i cur, const Fn funct) noexcept
    {
        const __m128i shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m256i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm256_permute4x64_epi64(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi8(h_min_val, _mm256_broadcastsi128_si256(shuf_w)));
        return h_min_val;
    }

    static __m256i h_min(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epi16(val1, val2); });
    }

    static __m256i h_max(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epi16(val1, val2); });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epu16(val1, val2); });
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epu16(val1, val2); });
    }

    static signed_t get_any(const __m256i cur) noexcept
    {
        return static_cast<signed_t>(_mm256_cvtsi256_si32(cur));
    }

    static unsigned_t get_v_pos(const __m256i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m256i cmp_eq(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi16(first, second);
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi16(first, second);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi16(first, second);
    }

    static __m256i min(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_min_epi16(first, second);
    }

    static __m256i max(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_max_epi16(first, second);
    }

    static __m256i min_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_min_epu16(first, second);
    }

    static __m256i max_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_max_epu16(first, second);
    }

    static __m256i mask_cast(const __m256i mask) noexcept
    {
        return mask;
    }
};

    #elif defined(USE_SSE2)

struct traits_2_sse : traits_2_base, traits_sse_base
{
    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static __m128i sign_correction(const __m128i val, const bool sign) noexcept
    {
        alignas(16) static constexpr unsigned_t sign_corrections[2][8] = {
            0x8000,
            0x8000,
            0x8000,
            0x8000,
            0x8000,
            0x8000,
            0x8000,
            0x8000,
            {}
        };
        return _mm_sub_epi16(val, _mm_load_si128(reinterpret_cast<const __m128i*>(sign_corrections[sign])));
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi16(idx, _mm_set1_epi16(1));
    }

    template <typename Fn>
    static __m128i h_func(const __m128i cur, const Fn funct) noexcept
    {
        const __m128i shuf_w = _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2);

        __m128i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        h_min_val = funct(h_min_val, _mm_shuffle_epi8(h_min_val, shuf_w));
        return h_min_val;
    }

    static __m128i h_min(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epi16(val1, val2); });
    }

    static __m128i h_max(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epi16(val1, val2); });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epu16(val1, val2); });
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epu16(val1, val2); });
    }

    static signed_t get_any(const __m128i cur) noexcept
    {
        return static_cast<signed_t>(_mm_cvtsi128_si32(cur));
    }

    static unsigned_t get_v_pos(const __m128i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m128i cmp_eq(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi16(first, second);
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi16(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi16(first, second);
    }

    static __m128i min(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_min_epi16(first, second);
    }

    static __m128i max(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_max_epi16(first, second);
    }

    static __m128i min_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_min_epu16(first, second);
    }

    static __m128i max_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_max_epu16(first, second);
    }

    static __m128i mask_cast(const __m128i mask) noexcept
    {
        return mask;
    }
};

    #endif

#endif

struct traits_4_base
{
    static constexpr bool is_floating = false;

    using signed_t = int32_t;
    using unsigned_t = uint32_t;

    using minmax_i_t = _priv::min_max_4i;
    using minmax_u_t = _priv::min_max_4u;

    static constexpr signed_t init_min_val = static_cast<signed_t>(0x7FFF'FFFFUL);
    static constexpr signed_t init_max_val = static_cast<signed_t>(0x8000'0000UL);

#if defined(VX_ARCH_WORD_BITS_32)
    static constexpr bool has_portion_max = false;
#else  // ^^^ 32-bit / 64-bit vvv
    static constexpr bool has_portion_max = true;
    static constexpr size_t portion_max = 0x1'0000'0000ULL;
#endif // ^^^ 64-bit ^^^
};

#if defined(USE_ARM_NEON)

struct traits_4_neon : traits_4_base, traits_neon_base
{
    using vec_t = int32x4_t;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    static vec_t zero() noexcept
    {
        return vdupq_n_s32(0);
    }

    static vec_t all_ones() noexcept
    {
        return vreinterpretq_s32_s8(vdupq_n_s8(static_cast<int8_t>(0xFF)));
    }

    static vec_t blend(const vec_t px1, const vec_t px2, const vec_t msk) noexcept
    {
        return vbslq_s32(vreinterpretq_u32_s32(msk), px2, px1);
    }

    // Compresses a 128-bit Mask of 4 32-bit values into a 64-bit Mask of 4 16-bit values.
    static uint64_t mask(const int32x4_t val) noexcept
    {
        const uint32x2_t res = vshrn_n_u64(vreinterpretq_u64_s32(val), 16);
        return vget_lane_u64(vreinterpret_u64_u32(res), 0);
    }

    static uint64_t match_mask(const vec_t val_lo, const vec_t val_hi) noexcept
    {
        const int8x8_t val = vaddhn_s16(vreinterpretq_s16_s32(val_lo), vreinterpretq_s16_s32(val_hi));
        return vget_lane_u64(vreinterpret_u64_s8(val), 0);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return _CountTrailingZeros64(mask) >> 2;
    }

    static unsigned long get_last_h_pos(const uint64_t mask) noexcept
    {
        return 15 - (_CountLeadingZeros64(mask) >> 2);
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_s32(reinterpret_cast<const int32_t*>(src));
    }

    static vec_t inc(const vec_t idx) noexcept
    {
        return vaddq_s32(idx, vdupq_n_s32(1));
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        return vdupq_n_s32(vminvq_s32(cur));
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        return vdupq_n_s32(vmaxvq_s32(cur));
    }

    static vec_t h_min_u(const vec_t cur) noexcept
    {
        const uint32_t mx = vminvq_u32(vreinterpretq_u32_s32(cur));
        return vreinterpretq_s32_u32(vdupq_n_u32(mx));
    }

    static vec_t h_max_u(const vec_t cur) noexcept
    {
        const uint32_t mx = vmaxvq_u32(vreinterpretq_u32_s32(cur));
        return vreinterpretq_s32_u32(vdupq_n_u32(mx));
    }

    static signed_t get_any(const vec_t cur) noexcept
    {
        return static_cast<signed_t>(vgetq_lane_s32(cur, 0));
    }

    static unsigned_t get_v_pos(const vec_t cur) noexcept
    {
        return static_cast<unsigned_t>(get_any(cur));
    }

    static vec_t cmp_eq(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s32_u32(vceqq_s32(first, second));
    }

    static vec_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s32_u32(vcgtq_s32(first, second));
    }

    static vec_t cmp_gt_u(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s32_u32(vcgtq_u32(vreinterpretq_u32_s32(first), vreinterpretq_u32_s32(second)));
    }

    static vec_t cmp_eq_idx(const vec_t first, const vec_t second) noexcept
    {
        return cmp_eq(first, second);
    }

    static vec_t min(const vec_t first, const vec_t second, vec_t = vdupq_n_s32(0)) noexcept
    {
        return vminq_s32(first, second);
    }

    static vec_t max(const vec_t first, const vec_t second, vec_t = vdupq_n_s32(0)) noexcept
    {
        return vmaxq_s32(first, second);
    }

    static vec_t min_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s32(0)) noexcept
    {
        const uint32x4_t Rx = vminq_u32(vreinterpretq_u32_s32(first), vreinterpretq_u32_s32(second));
        return vreinterpretq_s32_u32(Rx);
    }

    static vec_t max_u(const vec_t first, const vec_t second, vec_t = vdupq_n_s32(0)) noexcept
    {
        const uint32x4_t Rx = vmaxq_u32(vreinterpretq_u32_s32(first), vreinterpretq_u32_s32(second));
        return vreinterpretq_s32_u32(Rx);
    }

    static vec_t mask_cast(const vec_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_4_avx : traits_4_base, traits_avx_i_base
{
    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i sign_correction(const __m256i val, const bool sign) noexcept
    {
        alignas(32) static constexpr unsigned_t sign_corrections[2][8] = { 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, 0x8000'0000UL, {} };
        return _mm256_sub_epi32(
            val,
            _mm256_load_si256(reinterpret_cast<const __m256i*>(sign_corrections[sign])));
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi32(idx, _mm256_set1_epi32(1));
    }

    template <typename Fn>
    static __m256i h_func(const __m256i cur, const Fn funct) noexcept
    {
        __m256i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm256_permute4x64_epi64(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm256_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        return h_min_val;
    }

    static __m256i h_min(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epi32(val1, val2); });
    }

    static __m256i h_max(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epi32(val1, val2); });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_min_epu32(val1, val2); });
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return h_func(cur,
            [](const __m256i val1, const __m256i val2) noexcept
            { return _mm256_max_epu32(val1, val2); });
    }

    static signed_t get_any(const __m256i cur) noexcept
    {
        return static_cast<signed_t>(_mm256_cvtsi256_si32(cur));
    }

    static unsigned_t get_v_pos(const __m256i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m256i cmp_eq(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi32(first, second);
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi32(first, second);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi32(first, second);
    }

    static __m256i min(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_min_epi32(first, second);
    }

    static __m256i max(
        const __m256i first,
        const __m256i second,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        return _mm256_max_epi32(first, second);
    }

    static __m256i min_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_min_epu32(first, second);
    }

    static __m256i max_u(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_max_epu32(first, second);
    }

    static __m256i mask_cast(const __m256i mask) noexcept
    {
        return mask;
    }
};

    #elif defined(USE_SSE2)

struct traits_4_sse : traits_4_base, traits_sse_base
{
    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static __m128i sign_correction(const __m128i val, const bool sign) noexcept
    {
        alignas(16) static constexpr unsigned_t sign_corrections[2][4] = {
            0x8000'0000UL,
            0x8000'0000UL,
            0x8000'0000UL,
            0x8000'0000UL,
            {}
        };
        return _mm_sub_epi32(val, _mm_load_si128(reinterpret_cast<const __m128i*>(sign_corrections[sign])));
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi32(idx, _mm_set1_epi32(1));
    }

    template <typename Fn>
    static __m128i h_func(const __m128i cur, const Fn funct) noexcept
    {
        __m128i h_min_val = cur;
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)));
        h_min_val = funct(h_min_val, _mm_shuffle_epi32(h_min_val, _MM_SHUFFLE(2, 3, 0, 1)));
        return h_min_val;
    }

    static __m128i h_min(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epi32(val1, val2); });
    }

    static __m128i h_max(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epi32(val1, val2); });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_min_epu32(val1, val2); });
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return h_func(cur,
            [](const __m128i val1, const __m128i val2) noexcept
            { return _mm_max_epu32(val1, val2); });
    }

    static signed_t get_any(const __m128i cur) noexcept
    {
        return static_cast<signed_t>(_mm_cvtsi128_si32(cur));
    }

    static unsigned_t get_v_pos(const __m128i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m128i cmp_eq(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi32(first, second);
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi32(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi32(first, second);
    }

    static __m128i min(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_min_epi32(first, second);
    }

    static __m128i max(const __m128i first, const __m128i second, __m128i = _mm_undefined_si128()) noexcept
    {
        return _mm_max_epi32(first, second);
    }

    static __m128i min_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_min_epu32(first, second);
    }

    static __m128i max_u(const __m128i first, const __m128i second) noexcept
    {
        return _mm_max_epu32(first, second);
    }

    static __m128i mask_cast(const __m128i mask) noexcept
    {
        return mask;
    }
};

    #endif

#endif

struct traits_8_base
{
    static constexpr bool is_floating = false;

    using signed_t = int64_t;
    using unsigned_t = uint64_t;

    static constexpr signed_t init_min_val = static_cast<signed_t>(0x7FFF'FFFF'FFFF'FFFFULL);
    static constexpr signed_t init_max_val = static_cast<signed_t>(0x8000'0000'0000'0000ULL);

    using minmax_i_t = _priv::min_max_8i;
    using minmax_u_t = _priv::min_max_8u;

    static constexpr bool has_portion_max = false;
};

#if defined(USE_ARM_NEON)

struct traits_8_neon : traits_8_base, traits_neon_base
{
    using vec_t = int64x2_t;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    // Compresses a 128-bit Mask of 2 64-bit values into a 64-bit Mask of 2 32-bit values.
    static uint64_t mask(const vec_t val) noexcept
    {
        const uint32x2_t res = vreinterpret_u32_s32(vmovn_s64(val));
        return vget_lane_u64(vreinterpret_u64_u32(res), 0);
    }

    static uint64_t match_mask(const vec_t val_lo, const vec_t val_hi) noexcept
    {
        const int8x8_t val = vaddhn_s16(vreinterpretq_s16_s64(val_lo), vreinterpretq_s16_s64(val_hi));
        return vget_lane_u64(vreinterpret_u64_s8(val), 0);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return _CountTrailingZeros64(mask) >> 2;
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_s64(reinterpret_cast<const int64_t*>(src));
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        int64x2_t _Swapped = vextq_s64(cur, cur, 1);
        uint64x2_t mask_lt = vcltq_s64(_Swapped, cur);
        return vbslq_s64(mask_lt, _Swapped, cur);
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        int64x2_t _Swapped = vextq_s64(cur, cur, 1);
        uint64x2_t mask_gt = vcgtq_s64(_Swapped, cur);
        return vbslq_s64(mask_gt, _Swapped, cur);
    }

    static vec_t h_min_u(const vec_t cur) noexcept
    {
        const uint64x2_t cur_u = vreinterpretq_u64_s64(cur);
        uint64x2_t _Swapped = vextq_u64(cur_u, cur_u, 1);
        uint64x2_t mask_lt = vcltq_u64(_Swapped, cur_u);
        return vreinterpretq_s64_u64(vbslq_u64(mask_lt, _Swapped, cur_u));
    }

    static vec_t h_max_u(const vec_t cur) noexcept
    {
        const uint64x2_t cur_u = vreinterpretq_u64_s64(cur);
        uint64x2_t _Swapped = vextq_u64(cur_u, cur_u, 1);
        uint64x2_t mask_gt = vcgtq_u64(_Swapped, cur_u);
        return vreinterpretq_s64_u64(vbslq_u64(mask_gt, _Swapped, cur_u));
    }

    static signed_t get_any(const vec_t cur) noexcept
    {
        return static_cast<signed_t>(vgetq_lane_s64(cur, 0));
    }

    static vec_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s64_u64(vcgtq_s64(first, second));
    }

    static vec_t cmp_gt_u(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s64_u64(vcgtq_u64(vreinterpretq_u64_s64(first), vreinterpretq_u64_s64(second)));
    }

    static vec_t min(const vec_t first, const vec_t second, const vec_t mask) noexcept
    {
        return vbslq_s64(vreinterpretq_u64_s64(mask), second, first);
    }

    static vec_t min(const vec_t first, const vec_t second) noexcept
    {
        return min(first, second, cmp_gt(first, second));
    }

    static vec_t min_u(const vec_t first, const vec_t second) noexcept
    {
        return min(first, second, cmp_gt_u(first, second));
    }

    static vec_t max(const vec_t first, const vec_t second, const vec_t mask) noexcept
    {
        return vbslq_s64(vreinterpretq_u64_s64(mask), second, first);
    }

    static vec_t max(const vec_t first, const vec_t second) noexcept
    {
        return max(first, second, cmp_gt(second, first));
    }

    static vec_t max_u(const vec_t first, const vec_t second) noexcept
    {
        return max(first, second, cmp_gt_u(second, first));
    }

    static vec_t mask_cast(const vec_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_8_avx : traits_8_base, traits_avx_i_base
{
    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i sign_correction(const __m256i val, const bool sign) noexcept
    {
        alignas(32) static constexpr unsigned_t sign_corrections[2][4] = { 0x8000'0000'0000'0000ULL,
            0x8000'0000'0000'0000ULL,
            0x8000'0000'0000'0000ULL,
            0x8000'0000'0000'0000ULL,
            {} };
        return _mm256_sub_epi64(
            val,
            _mm256_load_si256(reinterpret_cast<const __m256i*>(sign_corrections[sign])));
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi64(idx, _mm256_set1_epi64x(1));
    }

    template <typename Fn>
    static __m256i h_func(const __m256i cur, const Fn funct) noexcept
    {
        alignas(32) signed_t _Array[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(_Array), cur);

        signed_t h_min_v = _Array[0];

        if (funct(_Array[1], h_min_v))
        {
            h_min_v = _Array[1];
        }

        if (funct(_Array[2], h_min_v))
        {
            h_min_v = _Array[2];
        }

        if (funct(_Array[3], h_min_v))
        {
            h_min_v = _Array[3];
        }

        return _mm256_set1_epi64x(h_min_v);
    }

    static __m256i h_min(const __m256i cur) noexcept
    {
        return h_func(cur, [](const signed_t lhs, const signed_t rhs) noexcept
            { return lhs < rhs; });
    }

    static __m256i h_max(const __m256i cur) noexcept
    {
        return h_func(cur, [](const signed_t lhs, const signed_t rhs) noexcept
            { return lhs > rhs; });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return h_func(
            cur,
            [](const unsigned_t lhs, const unsigned_t rhs) noexcept
            { return lhs < rhs; });
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return h_func(
            cur,
            [](const unsigned_t lhs, const unsigned_t rhs) noexcept
            { return lhs > rhs; });
    }

    static signed_t get_any(const __m256i cur) noexcept
    {
        return traits_8_sse::get_any(_mm256_castsi256_si128(cur));
    }

    static unsigned_t get_v_pos(const __m256i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m256i cmp_eq(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi64(first, second);
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi64(first, second);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi64(first, second);
    }

    static __m256i min(const __m256i first, const __m256i second, const __m256i mask) noexcept
    {
        return _mm256_blendv_epi8(first, second, mask);
    }

    static __m256i max(const __m256i first, const __m256i second, const __m256i mask) noexcept
    {
        return _mm256_blendv_epi8(first, second, mask);
    }

    static __m256i min(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_blendv_epi8(first, second, cmp_gt(first, second));
    }

    static __m256i max(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_blendv_epi8(first, second, cmp_gt(second, first));
    }

    static __m256i mask_cast(const __m256i mask) noexcept
    {
        return mask;
    }
};

    #elif defined(USE_SSE2)

struct traits_8_sse : traits_8_base, traits_sse_base
{
    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static __m128i sign_correction(const __m128i val, const bool sign) noexcept
    {
        alignas(16) static constexpr unsigned_t sign_corrections[2][2] = {
            0x8000'0000'0000'0000ULL,
            0x8000'0000'0000'0000ULL,
            {}
        };
        return _mm_sub_epi64(val, _mm_load_si128(reinterpret_cast<const __m128i*>(sign_corrections[sign])));
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi64(idx, _mm_set1_epi64x(1));
    }

    template <typename Fn>
    static __m128i h_func(const __m128i cur, const Fn funct) noexcept
    {
        signed_t h_min_a = get_any(cur);
        const signed_t h_min_b = get_any(_mm_bsrli_si128(cur, 8));
        if (funct(h_min_b, h_min_a))
        {
            h_min_a = h_min_b;
        }
        return _mm_set1_epi64x(h_min_a);
    }

    static __m128i h_min(const __m128i cur) noexcept
    {
        return h_func(cur, [](const signed_t lhs, const signed_t rhs) noexcept
            { return lhs < rhs; });
    }

    static __m128i h_max(const __m128i cur) noexcept
    {
        return h_func(cur, [](const signed_t lhs, const signed_t rhs) noexcept
            { return lhs > rhs; });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const unsigned_t lhs, const unsigned_t rhs) noexcept
            { return lhs < rhs; });
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return h_func(
            cur,
            [](const unsigned_t lhs, const unsigned_t rhs) noexcept
            { return lhs > rhs; });
    }

    static signed_t get_any(const __m128i cur) noexcept
    {
        // With optimizations enabled, compiles into register movement, rather than an actual stack spill.
        // Works around the absence of _mm_cvtsi128_si64 on 32-bit.
        signed_t _Array[2];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(&_Array), cur);
        return _Array[0];
    }

    static unsigned_t get_v_pos(const __m128i idx) noexcept
    {
        return static_cast<unsigned_t>(get_any(idx));
    }

    static __m128i cmp_eq(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi64(first, second);
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi64(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi64(first, second);
    }

    static __m128i min(const __m128i first, const __m128i second, const __m128i mask) noexcept
    {
        return _mm_blendv_epi8(first, second, mask);
    }

    static __m128i max(const __m128i first, const __m128i second, const __m128i mask) noexcept
    {
        return _mm_blendv_epi8(first, second, mask);
    }

    static __m128i min(const __m128i first, const __m128i second) noexcept
    {
        return _mm_blendv_epi8(first, second, cmp_gt(first, second));
    }

    static __m128i max(const __m128i first, const __m128i second) noexcept
    {
        return _mm_blendv_epi8(first, second, cmp_gt(second, first));
    }

    static __m128i mask_cast(const __m128i mask) noexcept
    {
        return mask;
    }
};

    #endif

#endif

struct traits_f_base
{
    static constexpr bool is_floating = true;

    using signed_t = float;
    using unsigned_t = void;

    static constexpr signed_t init_min_val = __builtin_huge_valf();
    static constexpr signed_t init_max_val = -__builtin_huge_valf();

    using minmax_i_t = _priv::min_max_f;
    using minmax_u_t = void;

#if defined(VX_ARCH_WORD_BITS_32)
    static constexpr bool has_portion_max = false;
#else  // ^^^ 32-bit / 64-bit vvv
    static constexpr bool has_portion_max = true;
    static constexpr size_t portion_max = 0x1'0000'0000ULL;
#endif // ^^^ 64-bit ^^^
};

#if defined(USE_ARM_NEON)

struct traits_f_neon : traits_f_base, traits_neon_base
{
    using vec_t = float32x4_t;
    using idx_t = int32x4_t;
    static constexpr bool has_unsigned_cmp = false;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    static idx_t zero() noexcept
    {
        return vdupq_n_s32(0);
    }

    static idx_t all_ones() noexcept
    {
        return vreinterpretq_s32_s8(vdupq_n_s8(static_cast<int8_t>(0xFF)));
    }

    static idx_t blend(const idx_t px1, const idx_t px2, const idx_t msk) noexcept
    {
        return vbslq_s32(vreinterpretq_u32_s32(msk), px2, px1);
    }

    static uint64_t mask(const idx_t val) noexcept
    {
        return traits_4_neon::mask(val);
    }

    static uint64_t match_mask(const idx_t val_lo, const idx_t val_hi) noexcept
    {
        return traits_4_neon::match_mask(val_lo, val_hi);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return traits_4_neon::get_first_h_pos(mask);
    }

    static unsigned long get_last_h_pos(const uint64_t mask) noexcept
    {
        return traits_4_neon::get_last_h_pos(mask);
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_f32(reinterpret_cast<const float32_t*>(src));
    }

    static idx_t inc(const idx_t idx) noexcept
    {
        return traits_4_neon::inc(idx);
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        return vdupq_n_f32(vminvq_f32(cur));
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        return vdupq_n_f32(vmaxvq_f32(cur));
    }

    static idx_t h_min_u(const idx_t cur) noexcept
    {
        return traits_4_neon::h_min_u(cur);
    }

    static idx_t h_max_u(const idx_t cur) noexcept
    {
        return traits_4_neon::h_max_u(cur);
    }

    static float get_any(const vec_t cur) noexcept
    {
        return vgetq_lane_f32(cur, 0);
    }

    static traits_4_neon::unsigned_t get_v_pos(const idx_t cur) noexcept
    {
        return traits_4_neon::get_v_pos(cur);
    }

    static idx_t cmp_eq(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s32_u32(vceqq_f32(first, second));
    }

    static idx_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s32_u32(vcgtq_f32(first, second));
    }

    static idx_t cmp_eq_idx(const idx_t first, const idx_t second) noexcept
    {
        return traits_4_neon::cmp_eq_idx(first, second);
    }

    static vec_t min(const vec_t first, const vec_t second, idx_t = vdupq_n_s32(0)) noexcept
    {
        return vminq_f32(first, second);
    }

    static vec_t max(const vec_t first, const vec_t second, idx_t = vdupq_n_s32(0)) noexcept
    {
        return vmaxq_f32(first, second);
    }

    static idx_t mask_cast(const idx_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_f_avx : traits_f_base, traits_avx_base
{
    static constexpr size_t tail_mask = 0x1C;
    using vec_t = __m256;

    static __m256 blendval(const __m256 px1, const __m256 px2, const __m256i msk) noexcept
    {
        return _mm256_blendv_ps(px1, px2, _mm256_castsi256_ps(msk));
    }

    static __m256 load(const void* const src) noexcept
    {
        return _mm256_loadu_ps(reinterpret_cast<const float*>(src));
    }

    static __m256 load_mask(const void* const src, const __m256i mask) noexcept
    {
        return _mm256_maskload_ps(reinterpret_cast<const float*>(src), mask);
    }

    static __m256 sign_correction(const __m256 val, bool) noexcept
    {
        return val;
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi32(idx, _mm256_set1_epi32(1));
    }

    template <typename Fn>
    static __m256 h_func(const __m256 cur, const Fn funct) noexcept
    {
        __m256 h_min_val = cur;
        h_min_val = funct(_mm256_shuffle_ps(h_min_val, h_min_val, _MM_SHUFFLE(2, 3, 0, 1)), h_min_val);
        h_min_val = funct(_mm256_shuffle_ps(h_min_val, h_min_val, _MM_SHUFFLE(1, 0, 3, 2)), h_min_val);
        h_min_val = funct(_mm256_permute2f128_ps(h_min_val, _mm256_undefined_ps(), 0x01), h_min_val);
        return h_min_val;
    }

    static __m256 h_min(const __m256 cur) noexcept
    {
        return h_func(
            cur,
            [](const __m256 val1, const __m256 val2) noexcept
            { return _mm256_min_ps(val1, val2); });
    }

    static __m256 h_max(const __m256 cur) noexcept
    {
        return h_func(
            cur,
            [](const __m256 val1, const __m256 val2) noexcept
            { return _mm256_max_ps(val1, val2); });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return traits_4_avx::h_min_u(cur);
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return traits_4_avx::h_max_u(cur);
    }

    static float get_any(const __m256 cur) noexcept
    {
        return _mm256_cvtss_f32(cur);
    }

    static uint32_t get_v_pos(const __m256i idx) noexcept
    {
        return traits_4_avx::get_v_pos(idx);
    }

    static __m256 cmp_eq(const __m256 first, const __m256 second) noexcept
    {
        return _mm256_cmp_ps(first, second, _CMP_EQ_OQ);
    }

    static __m256 cmp_gt(const __m256 first, const __m256 second) noexcept
    {
        return _mm256_cmp_ps(first, second, _CMP_GT_OQ);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi32(first, second);
    }

    static __m256 min(const __m256 first, const __m256 second, __m256 = _mm256_undefined_ps()) noexcept
    {
        return _mm256_min_ps(second, first);
    }

    static __m256 max(const __m256 first, const __m256 second, __m256 = _mm256_undefined_ps()) noexcept
    {
        return _mm256_max_ps(second, first);
    }

    static __m256i mask_cast(const __m256 mask) noexcept
    {
        return _mm256_castps_si256(mask);
    }
};

    #elif defined(USE_SSE2)

struct traits_f_sse : traits_f_base, traits_sse_base
{
    using vec_t = __m128;

    static __m128 load(const void* const src) noexcept
    {
        return _mm_loadu_ps(reinterpret_cast<const float*>(src));
    }

    static __m128 sign_correction(const __m128 val, bool) noexcept
    {
        return val;
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi32(idx, _mm_set1_epi32(1));
    }

    template <typename Fn>
    static __m128 h_func(const __m128 cur, const Fn funct) noexcept
    {
        __m128 h_min_val = cur;
        h_min_val = funct(_mm_shuffle_ps(h_min_val, h_min_val, _MM_SHUFFLE(2, 3, 0, 1)), h_min_val);
        h_min_val = funct(_mm_shuffle_ps(h_min_val, h_min_val, _MM_SHUFFLE(1, 0, 3, 2)), h_min_val);
        return h_min_val;
    }

    static __m128 h_min(const __m128 cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128 val1, const __m128 val2) noexcept
            { return _mm_min_ps(val1, val2); });
    }

    static __m128 h_max(const __m128 cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128 val1, const __m128 val2) noexcept
            { return _mm_max_ps(val1, val2); });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return traits_4_sse::h_min_u(cur);
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return traits_4_sse::h_max_u(cur);
    }

    static float get_any(const __m128 cur) noexcept
    {
        return _mm_cvtss_f32(cur);
    }

    static uint32_t get_v_pos(const __m128i idx) noexcept
    {
        return traits_4_sse::get_v_pos(idx);
    }

    static __m128 cmp_eq(const __m128 first, const __m128 second) noexcept
    {
        return _mm_cmpeq_ps(first, second);
    }

    static __m128 cmp_gt(const __m128 first, const __m128 second) noexcept
    {
        return _mm_cmpgt_ps(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi32(first, second);
    }

    static __m128 min(const __m128 first, const __m128 second, __m128 = _mm_undefined_ps()) noexcept
    {
        return _mm_min_ps(second, first);
    }

    static __m128 max(const __m128 first, const __m128 second, __m128 = _mm_undefined_ps()) noexcept
    {
        return _mm_max_ps(second, first);
    }

    static __m128i mask_cast(const __m128 mask) noexcept
    {
        return _mm_castps_si128(mask);
    }
};

    #endif

#endif

struct traits_d_base
{
    static constexpr bool is_floating = true;

    using signed_t = double;
    using unsigned_t = void;

    static constexpr signed_t init_min_val = __builtin_huge_val();
    static constexpr signed_t init_max_val = -__builtin_huge_val();

    using minmax_i_t = _priv::min_max_d;
    using minmax_u_t = void;

    static constexpr bool has_portion_max = false;
};

#if defined(USE_ARM_NEON)

struct traits_d_neon : traits_d_base, traits_neon_base
{
    using vec_t = float64x2_t;
    using idx_t = int64x2_t;
    static constexpr bool has_unsigned_cmp = false;

    static vec_t sign_correction(const vec_t val, bool) noexcept
    {
        return val;
    }

    static idx_t zero() noexcept
    {
        return vdupq_n_s64(0);
    }

    static idx_t all_ones() noexcept
    {
        return vreinterpretq_s64_s8(vdupq_n_s8(static_cast<int8_t>(0xFF)));
    }

    static idx_t blend(const idx_t px1, const idx_t px2, const idx_t msk) noexcept
    {
        return vbslq_s64(vreinterpretq_u64_s64(msk), px2, px1);
    }

    // Compresses a 128-bit Mask of 2 64-bit values into a 64-bit Mask of 2 32-bit values.
    static uint64_t mask(const int64x2_t val) noexcept
    {
        return traits_8_neon::mask(val);
    }

    static uint64_t match_mask(const idx_t val_lo, const idx_t val_hi) noexcept
    {
        return traits_8_neon::match_mask(val_lo, val_hi);
    }

    static unsigned long get_first_h_pos(const uint64_t mask) noexcept
    {
        return traits_8_neon::get_first_h_pos(mask);
    }

    static unsigned long get_last_h_pos(const uint64_t mask) noexcept
    {
        return 15 - (_CountLeadingZeros64(mask) >> 2);
    }

    static vec_t load(const void* const src) noexcept
    {
        return vld1q_f64(reinterpret_cast<const float64_t*>(src));
    }

    static idx_t inc(const idx_t idx) noexcept
    {
        return vaddq_s64(idx, vdupq_n_s64(1));
    }

    static vec_t h_min(const vec_t cur) noexcept
    {
        return vdupq_n_f64(vminvq_f64(cur));
    }

    static vec_t h_max(const vec_t cur) noexcept
    {
        return vdupq_n_f64(vmaxvq_f64(cur));
    }

    static idx_t h_min_u(const idx_t cur) noexcept
    {
        return traits_8_neon::h_min_u(cur);
    }

    static idx_t h_max_u(const idx_t cur) noexcept
    {
        return traits_8_neon::h_max_u(cur);
    }

    static double get_any(const vec_t cur) noexcept
    {
        return vgetq_lane_f64(cur, 0);
    }

    static traits_8_base::unsigned_t get_v_pos(const idx_t cur) noexcept
    {
        return static_cast<traits_8_base::unsigned_t>(vgetq_lane_s64(cur, 0));
    }

    static idx_t cmp_eq(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s64_u64(vceqq_f64(first, second));
    }

    static idx_t cmp_gt(const vec_t first, const vec_t second) noexcept
    {
        return vreinterpretq_s64_u64(vcgtq_f64(first, second));
    }

    static idx_t cmp_eq_idx(const idx_t first, const idx_t second) noexcept
    {
        return vreinterpretq_s64_u64(vceqq_s64(first, second));
    }

    static vec_t min(const vec_t first, const vec_t second, idx_t = vdupq_n_s64(0)) noexcept
    {
        return vminq_f64(first, second);
    }

    static vec_t max(const vec_t first, const vec_t second, idx_t = vdupq_n_s64(0)) noexcept
    {
        return vmaxq_f64(first, second);
    }

    static idx_t mask_cast(const idx_t mask) noexcept
    {
        return mask;
    }
};

#elif defined(USE_X86)

    #if defined(USE_AVX2)

struct traits_d_avx : traits_d_base, traits_avx_base
{
    static constexpr size_t tail_mask = 0x18;
    using vec_t = __m256d;

    static __m256d blendval(const __m256d px1, const __m256d px2, const __m256i msk) noexcept
    {
        return _mm256_blendv_pd(px1, px2, _mm256_castsi256_pd(msk));
    }

    static __m256d load(const void* const src) noexcept
    {
        return _mm256_loadu_pd(reinterpret_cast<const double*>(src));
    }

    static __m256d load_mask(const void* const src, const __m256i mask) noexcept
    {
        return _mm256_maskload_pd(reinterpret_cast<const double*>(src), mask);
    }

    static __m256d sign_correction(const __m256d val, bool) noexcept
    {
        return val;
    }

    static __m256i inc(const __m256i idx) noexcept
    {
        return _mm256_add_epi64(idx, _mm256_set1_epi64x(1));
    }

    template <typename Fn>
    static __m256d h_func(const __m256d cur, const Fn funct) noexcept
    {
        __m256d h_min_val = cur;
        h_min_val = funct(_mm256_shuffle_pd(h_min_val, h_min_val, 0b0101), h_min_val);
        h_min_val = funct(_mm256_permute4x64_pd(h_min_val, _MM_SHUFFLE(1, 0, 3, 2)), h_min_val);
        return h_min_val;
    }

    static __m256d h_min(const __m256d cur) noexcept
    {
        return h_func(cur,
            [](const __m256d val1, const __m256d val2) noexcept
            { return _mm256_min_pd(val1, val2); });
    }

    static __m256d h_max(const __m256d cur) noexcept
    {
        return h_func(cur,
            [](const __m256d val1, const __m256d val2) noexcept
            { return _mm256_max_pd(val1, val2); });
    }

    static __m256i h_min_u(const __m256i cur) noexcept
    {
        return traits_8_avx::h_min_u(cur);
    }

    static __m256i h_max_u(const __m256i cur) noexcept
    {
        return traits_8_avx::h_max_u(cur);
    }

    static double get_any(const __m256d cur) noexcept
    {
        return _mm256_cvtsd_f64(cur);
    }

    static uint64_t get_v_pos(const __m256i idx) noexcept
    {
        return traits_8_avx::get_v_pos(idx);
    }

    static __m256d cmp_eq(const __m256d first, const __m256d second) noexcept
    {
        return _mm256_cmp_pd(first, second, _CMP_EQ_OQ);
    }

    static __m256d cmp_gt(const __m256d first, const __m256d second) noexcept
    {
        return _mm256_cmp_pd(first, second, _CMP_GT_OQ);
    }

    static __m256i cmp_eq_idx(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpeq_epi64(first, second);
    }

    static __m256d min(const __m256d first, const __m256d second, __m256d = _mm256_undefined_pd()) noexcept
    {
        return _mm256_min_pd(second, first);
    }

    static __m256d max(const __m256d first, const __m256d second, __m256d = _mm256_undefined_pd()) noexcept
    {
        return _mm256_max_pd(second, first);
    }

    static __m256i mask_cast(const __m256d mask) noexcept
    {
        return _mm256_castpd_si256(mask);
    }
};

    #elif defined(USE_SSE2)

struct traits_d_sse : traits_d_base, traits_sse_base
{
    using vec_t = __m128d;

    static __m128d load(const void* const src) noexcept
    {
        return _mm_loadu_pd(reinterpret_cast<const double*>(src));
    }

    static __m128d sign_correction(const __m128d val, bool) noexcept
    {
        return val;
    }

    static __m128i inc(const __m128i idx) noexcept
    {
        return _mm_add_epi64(idx, _mm_set1_epi64x(1));
    }

    template <typename Fn>
    static __m128d h_func(const __m128d cur, const Fn funct) noexcept
    {
        __m128d h_min_val = cur;
        h_min_val = funct(_mm_shuffle_pd(h_min_val, h_min_val, 1), h_min_val);
        return h_min_val;
    }

    static __m128d h_min(const __m128d cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128d val1, const __m128d val2) noexcept
            { return _mm_min_pd(val1, val2); });
    }

    static __m128d h_max(const __m128d cur) noexcept
    {
        return h_func(
            cur,
            [](const __m128d val1, const __m128d val2) noexcept
            { return _mm_max_pd(val1, val2); });
    }

    static __m128i h_min_u(const __m128i cur) noexcept
    {
        return traits_8_sse::h_min_u(cur);
    }

    static __m128i h_max_u(const __m128i cur) noexcept
    {
        return traits_8_sse::h_max_u(cur);
    }
    static double get_any(const __m128d cur) noexcept
    {
        return _mm_cvtsd_f64(cur);
    }

    static uint64_t get_v_pos(const __m128i idx) noexcept
    {
        return traits_8_sse::get_v_pos(idx);
    }

    static __m128d cmp_eq(const __m128d first, const __m128d second) noexcept
    {
        return _mm_cmpeq_pd(first, second);
    }

    static __m128d cmp_gt(const __m128d first, const __m128d second) noexcept
    {
        return _mm_cmpgt_pd(first, second);
    }

    static __m128i cmp_eq_idx(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpeq_epi64(first, second);
    }

    static __m128d min(const __m128d first, const __m128d second, __m128d = _mm_undefined_pd()) noexcept
    {
        return _mm_min_pd(second, first);
    }

    static __m128d max(const __m128d first, const __m128d second, __m128d = _mm_undefined_pd()) noexcept
    {
        return _mm_max_pd(second, first);
    }

    static __m128i mask_cast(const __m128d mask) noexcept
    {
        return _mm_castpd_si128(mask);
    }
};

    #endif

#endif

struct traits_1
{
    using scalar = traits_scalar<traits_1_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_1_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_1_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_1_sse;
    #endif

#endif
};

struct traits_2
{
    using scalar = traits_scalar<traits_2_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_2_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_2_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_2_sse;
    #endif

#endif
};

struct traits_4
{
    using scalar = traits_scalar<traits_4_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_4_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_4_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_4_sse;
    #endif

#endif
};

struct traits_8
{
    using scalar = traits_scalar<traits_8_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_8_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_8_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_8_sse;
    #endif

#endif
};

struct traits_f
{
    using scalar = traits_scalar<traits_f_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_f_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_f_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_f_sse;
    #endif

#endif
};

struct traits_d
{
    using scalar = traits_scalar<traits_d_base>;

#if defined(USE_ARM_NEON)
    using neon = traits_d_neon;
#elif defined(USE_X86)

    #if defined(USE_AVX2)
    using avx = traits_d_avx;
    #endif

    #if defined(USE_SSE2)
    using sse = traits_d_sse;
    #endif

#endif
};

template <typename T>
const void* min_tail(const void* const first, const void* const last, const void* res, T cur) noexcept
{
    for (auto ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        if (*ptr < cur)
        {
            res = ptr;
            cur = *ptr;
        }
    }

    return res;
}

template <typename T>
const void* max_tail(const void* const first, const void* const last, const void* res, T cur) noexcept
{
    for (auto ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        if (cur < *ptr)
        {
            res = ptr;
            cur = *ptr;
        }
    }

    return res;
}

template <typename T>
_priv::min_max_element_t both_tail(const void* const first, const void* const last, _priv::min_max_element_t& res, T cur_min, T cur_max) noexcept
{
    for (auto ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        if (*ptr < cur_min)
        {
            res.min = ptr;
            cur_min = *ptr;
        }
        // Not else!
        // * Needed for correctness if start with maximum, as we don't handle specially the first element.
        // * Promote branchless code generation.
        if (cur_max <= *ptr)
        {
            res.max = ptr;
            cur_max = *ptr;
        }
    }

    return res;
}

#if defined(USE_ARM_NEON)

template <min_max_mode mode, typename Traits, bool sign>
auto minmax_element_impl(const void* first, const void* const last) noexcept
{

#else

template <min_max_mode mode, typename Traits>
auto minmax_element_impl(const void* first, const void* const last, const bool sign) noexcept
{
#endif

    _priv::min_max_element_t res = { first, first };
    auto cur_min_val = Traits::init_min_val;
    auto cur_max_val = Traits::init_max_val;

    VX_IF_CONSTEXPR (Traits::vectorized)
    {
        auto base = static_cast<const char*>(first);
        size_t portion_byte_size = byte_length(first, last) & ~Traits::vec_mask;

        VX_IF_CONSTEXPR (Traits::has_portion_max)
        {
            // vector of indices will wrap around at exactly this size
            constexpr size_t max_portion_byte_size = Traits::portion_max * Traits::vec_size;
            if (portion_byte_size > max_portion_byte_size)
            {
                portion_byte_size = max_portion_byte_size;
            }
        }

        const void* stop_at = first;
        advance_bytes(stop_at, portion_byte_size);

        // Load values and if unsigned adjust them to be signed_ (for signed_ vector comparisons)
        auto cur_vals = Traits::sign_correction(Traits::load(first), sign);
        auto cur_vals_min = cur_vals;      // vector of vertical minimum values
        auto cur_idx_min = Traits::zero(); // vector of vertical minimum indices
        auto cur_vals_max = cur_vals;      // vector of vertical maximum values
        auto cur_idx_max = Traits::zero(); // vector of vertical maximum indices
        auto cur_idx = Traits::zero();     // current vector of indices

#if defined(USE_ARM_NEON)

        const auto cmp_gt_wrap = [](const auto first, const auto second) noexcept
        {
            VX_IF_CONSTEXPR (sign || !Traits::has_unsigned_cmp)
            {
                return Traits::cmp_gt(first, second);
            }
            else
            {
                return Traits::cmp_gt_u(first, second);
            }
        };
        const auto min_wrap = [](const auto first, const auto second, const auto mask) noexcept
        {
            VX_IF_CONSTEXPR (sign || !Traits::has_unsigned_cmp)
            {
                return Traits::min(first, second, mask);
            }
            else
            {
                return Traits::min_u(first, second, mask);
            }
        };
        const auto max_wrap = [](const auto first, const auto second, const auto mask) noexcept
        {
            VX_IF_CONSTEXPR (sign || !Traits::has_unsigned_cmp)
            {
                return Traits::max(first, second, mask);
            }
            else
            {
                return Traits::max_u(first, second, mask);
            }
        };
        const auto h_min_wrap = [](const auto vals) noexcept
        {
            VX_IF_CONSTEXPR (sign || !Traits::has_unsigned_cmp)
            {
                return Traits::h_min(vals);
            }
            else
            {
                return Traits::h_min_u(vals);
            }
        };
        const auto h_max_wrap = [](const auto vals) noexcept
        {
            VX_IF_CONSTEXPR (sign || !Traits::has_unsigned_cmp)
            {
                return Traits::h_max(vals);
            }
            else
            {
                return Traits::h_max_u(vals);
            }
        };

#else

        const auto cmp_gt_wrap = [](const auto first, const auto second) noexcept
        {
            return Traits::cmp_gt(first, second);
        };
        const auto min_wrap = [](const auto first, const auto second, const auto mask) noexcept
        {
            return Traits::min(first, second, mask);
        };
        const auto max_wrap = [](const auto first, const auto second, const auto mask) noexcept
        {
            return Traits::max(first, second, mask);
        };
        const auto h_min_wrap = [](const auto vals) noexcept
        { return Traits::h_min(vals); };
        const auto h_max_wrap = [](const auto vals) noexcept
        { return Traits::h_max(vals); };

#endif

        const auto update_min_max = [&](const auto cur_vals, VX_MAYBE_UNUSED const auto blend_idx_0, const auto blend_idx_1) noexcept
        {
            VX_IF_CONSTEXPR ((mode & mode_min) != 0)
            {
                // Looking for the first occurrence of minimum, don't overwrite with newly found occurrences
                const auto is_less = cmp_gt_wrap(cur_vals_min, cur_vals); // cur_vals < cur_vals_min
                // Remember their vertical indices
                cur_idx_min = blend_idx_1(cur_idx_min, cur_idx, Traits::mask_cast(is_less));
                cur_vals_min = min_wrap(cur_vals_min, cur_vals, is_less); // Update the current minimum
            }

            VX_IF_CONSTEXPR (mode == mode_max)
            {
                // Looking for the first occurrence of maximum, don't overwrite with newly found occurrences
                // cur_vals > cur_vals_max
                const auto is_greater = cmp_gt_wrap(cur_vals, cur_vals_max);
                // Remember their vertical indices
                cur_idx_max = blend_idx_1(cur_idx_max, cur_idx, Traits::mask_cast(is_greater));
                // Update the current maximum
                cur_vals_max = max_wrap(cur_vals_max, cur_vals, is_greater);
            }
            else VX_IF_CONSTEXPR (mode == mode_both)
            {
                // Looking for the last occurrence of maximum, do overwrite with newly found occurrences
                // !(cur_vals >= cur_vals_max)
                const auto is_less = cmp_gt_wrap(cur_vals_max, cur_vals);
                // Remember their vertical indices
                cur_idx_max = blend_idx_0(cur_idx_max, cur_idx, Traits::mask_cast(is_less));
                cur_vals_max = max_wrap(cur_vals, cur_vals_max, is_less); // Update the current maximum
            }
        };

        const auto blend_idx_0 = [](const auto prev, const auto cur, const auto mask) noexcept
        {
            return Traits::blend(cur, prev, mask);
        };

        const auto blend_idx_1 = [](const auto prev, const auto cur, const auto mask) noexcept
        {
            return Traits::blend(prev, cur, mask);
        };

        for (;;)
        {
            advance_bytes(first, Traits::vec_size);

            // Increment vertical indices. Will stop at exactly wrap around, if not reach the end before
            cur_idx = Traits::inc(cur_idx);

            if (first != stop_at)
            {
                // This is the main part, finding vertical minimum/maximum

                // Load values and if unsigned adjust them to be signed_ (for signed_ vector comparisons)
                cur_vals = Traits::sign_correction(Traits::load(first), sign);

                update_min_max(cur_vals, blend_idx_0, blend_idx_1);
            }
            else
            {
#if defined(USE_AVX2)

                VX_IF_CONSTEXPR (Traits::tail_mask != 0)
                {
                    const size_t remaining_byte_size = byte_length(first, last);
                    bool last_portion;

                    VX_IF_CONSTEXPR (Traits::has_portion_max)
                    {
                        last_portion = (remaining_byte_size & ~Traits::vec_mask) == 0;
                    }
                    else
                    {
                        last_portion = true;
                    }

                    const size_t tail_byte_size = remaining_byte_size & Traits::tail_mask;

                    if (last_portion && tail_byte_size != 0)
                    {
                        const auto tail_mask = avx2_tail_mask_32(tail_byte_size);
                        const auto tail_vals =
                            Traits::sign_correction(Traits::load_mask(first, tail_mask), sign);
                        cur_vals = Traits::blendval(cur_vals, tail_vals, tail_mask);

                        const auto blend_idx_0_mask = [tail_mask](const auto prev, const auto cur, const auto mask) noexcept
                        {
                            return Traits::blend(prev, cur, _mm256_andnot_si256(mask, tail_mask));
                        };

                        const auto blend_idx_1_mask = [tail_mask](const auto prev, const auto cur, const auto mask) noexcept
                        {
                            return Traits::blend(prev, cur, _mm256_and_si256(tail_mask, mask));
                        };

                        update_min_max(cur_vals, blend_idx_0_mask, blend_idx_1_mask);
                        advance_bytes(first, tail_byte_size);
                    }
                }

#endif // defined(USE_AVX2)

                // Reached end or indices wrap around point.
                // Compute horizontal min and/or max. Determine horizontal and vertical position of it.

                VX_IF_CONSTEXPR ((mode & mode_min) != 0)
                {
                    // Vector populated by the smallest element
                    const auto h_min = h_min_wrap(cur_vals_min);
                    const auto h_min_val = Traits::get_any(h_min); // Get any element of it

                    if (h_min_val < cur_min_val)
                    {                            // Current horizontal min is less than the old
                        cur_min_val = h_min_val; // update min
                        // Mask of all elems eq to min
                        const auto eq_mask = Traits::cmp_eq(h_min, cur_vals_min);
                        auto mask = Traits::mask(Traits::mask_cast(eq_mask));
                        // Indices of minimum elements or the greatest index if none
                        const auto idx_min_val =
                            Traits::blend(Traits::all_ones(), cur_idx_min, Traits::mask_cast(eq_mask));
                        auto idx_min = Traits::h_min_u(idx_min_val); // The smallest indices
                        // Select the smallest vertical indices from the smallest element mask
                        mask &= Traits::mask(Traits::cmp_eq_idx(idx_min, idx_min_val));

                        // Find the smallest horizontal index
                        const unsigned long h_pos = Traits::get_first_h_pos(mask);

                        // Extract its vertical index
                        const auto v_pos = Traits::get_v_pos(idx_min);
                        // Finally, compute the pointer
                        res.min = base + static_cast<size_t>(v_pos) * Traits::vec_size + h_pos;
                    }
                }

                VX_IF_CONSTEXPR ((mode & mode_max) != 0)
                {
                    // Vector populated by the largest element
                    const auto h_max = h_max_wrap(cur_vals_max);
                    const auto h_max_val = Traits::get_any(h_max); // Get any element of it

                    if (mode == mode_both && cur_max_val <= h_max_val || mode == mode_max && cur_max_val < h_max_val)
                    {
                        // max_element: current horizontal max is greater than the old, update max
                        // minmax_element: current horizontal max is not less than the old, update max
                        cur_max_val = h_max_val;

                        // Mask of all elems eq to max
                        const auto eq_mask = Traits::cmp_eq(h_max, cur_vals_max);
                        auto mask = Traits::mask(Traits::mask_cast(eq_mask));

                        unsigned long h_pos;
                        size_t v_pos;
                        VX_IF_CONSTEXPR (mode == mode_both)
                        {
                            // Looking for the last occurrence of maximum
                            // Indices of maximum elements or zero if none
                            const auto idx_max_val =
                                Traits::blend(Traits::zero(), cur_idx_max, Traits::mask_cast(eq_mask));
                            const auto idx_max = Traits::h_max_u(idx_max_val); // The greatest indices
                            // Select the greatest vertical indices from the largest element mask
                            mask &= Traits::mask(Traits::cmp_eq_idx(idx_max, idx_max_val));

                            // Find the largest horizontal index
                            h_pos = Traits::get_last_h_pos(mask);
                            h_pos -= sizeof(cur_max_val) - 1; // Correct from highest val bit to lowest

                            // Extract its vertical index
                            v_pos = static_cast<size_t>(Traits::get_v_pos(idx_max));
                        }
                        else
                        {
                            // Looking for the first occurrence of maximum
                            // Indices of maximum elements or the greatest index if none
                            const auto idx_max_val = Traits::blend(
                                Traits::all_ones(),
                                cur_idx_max,
                                Traits::mask_cast(eq_mask));
                            const auto idx_max = Traits::h_min_u(idx_max_val); // The smallest indices

                            // Select the smallest vertical indices from the largest element mask
                            mask &= Traits::mask(Traits::cmp_eq_idx(idx_max, idx_max_val));

                            // Find the smallest horizontal index
                            h_pos = Traits::get_first_h_pos(mask);

                            // Extract its vertical index
                            v_pos = static_cast<size_t>(Traits::get_v_pos(idx_max));
                        }

                        // Finally, compute the pointer
                        res.max = base + v_pos * Traits::vec_size + h_pos;
                    }
                }
                // Horizontal part done, results are saved, now need to see if there is another portion.

                VX_IF_CONSTEXPR (Traits::has_portion_max)
                {
                    // Either the last portion or wrapping point reached, need to determine
                    portion_byte_size = byte_length(first, last) & ~Traits::vec_mask;
                    if (portion_byte_size == 0)
                    {
                        break; // That was the last portion
                    }
                    // Start next portion to handle the wrapping indices. Assume cur_idx is zero
                    constexpr size_t max_portion_byte_size = Traits::portion_max * Traits::vec_size;
                    if (portion_byte_size > max_portion_byte_size)
                    {
                        portion_byte_size = max_portion_byte_size;
                    }

                    advance_bytes(stop_at, portion_byte_size);
                    // Indices will be relative to the new base
                    base = static_cast<const char*>(first);
                    // Load values and if unsigned adjust them to be signed_ (for signed_ vector comparisons)
                    cur_vals = Traits::sign_correction(Traits::load(first), sign);

                    VX_IF_CONSTEXPR ((mode & mode_min) != 0)
                    {
                        cur_vals_min = cur_vals;
                        cur_idx_min = Traits::zero();
                    }

                    VX_IF_CONSTEXPR ((mode & mode_max) != 0)
                    {
                        cur_vals_max = cur_vals;
                        cur_idx_max = Traits::zero();
                    }
                }
                else
                {
                    break; // No wrapping, so it was the only portion
                }
            }
        }

        Traits::exit_vectorized(); // TRANSITION, DevCom-10331414
    }

    VX_IF_CONSTEXPR (Traits::is_floating)
    {
        VX_IF_CONSTEXPR (mode == mode_min)
        {
            return min_tail(first, last, res.min, cur_min_val);
        }
        else VX_IF_CONSTEXPR (mode == mode_max)
        {
            return max_tail(first, last, res.max, cur_max_val);
        }
        else
        {
            return both_tail(first, last, res, cur_min_val, cur_max_val);
        }
    }
    else
    {
        using ST = typename Traits::signed_t;
        using UT = typename Traits::unsigned_t;

        constexpr UT correction = Traits::has_unsigned_cmp ? 0 : UT{ 1 } << (sizeof(UT) * 8 - 1);

        VX_IF_CONSTEXPR (mode == mode_min)
        {
            if (sign)
            {
                return min_tail(first, last, res.min, static_cast<ST>(cur_min_val));
            }
            else
            {
                return min_tail(first, last, res.min, static_cast<UT>(cur_min_val + correction));
            }
        }
        else VX_IF_CONSTEXPR (mode == mode_max)
        {
            if (sign)
            {
                return max_tail(first, last, res.max, static_cast<ST>(cur_max_val));
            }
            else
            {
                return max_tail(first, last, res.max, static_cast<UT>(cur_max_val + correction));
            }
        }
        else
        {
            if (sign)
            {
                return both_tail(
                    first,
                    last,
                    res,
                    static_cast<ST>(cur_min_val),
                    static_cast<ST>(cur_max_val));
            }
            else
            {
                return both_tail(first, last, res, static_cast<UT>(cur_min_val + correction), static_cast<UT>(cur_max_val + correction));
            }
        }
    }
}

template <min_max_mode mode, typename Traits>
auto VX_STDCALL minmax_element_disp(
    const void* const first,
    const void* const last,
    const bool sign) noexcept
{
#if defined(USE_ARM_NEON)

    if (byte_length(first, last) >= 16)
    {
        if (sign)
        {
            return minmax_element_impl<mode, typename Traits::neon, true>(first, last);
        }
        else
        {
            return minmax_element_impl<mode, typename Traits::neon, false>(first, last);
        }
    }

    if (sign)
    {
        return minmax_element_impl<mode, typename Traits::scalar, true>(first, last);
    }
    else
    {
        return minmax_element_impl<mode, typename Traits::scalar, false>(first, last);
    }

#elif defined(USE_X86)

    #if defined(USE_AVX2)

    if (byte_length(first, last) >= 32)
    {
        return minmax_element_impl<mode, typename Traits::avx>(first, last, sign);
    }

    #endif

    #if defined(USE_SSE2)

    if (byte_length(first, last) >= 16)
    {
        return minmax_element_impl<mode, typename Traits::sse>(first, last, sign);
    }

    #endif

#endif

    return minmax_element_impl<mode, typename Traits::scalar>(first, last, sign);
}

template <min_max_mode mode, typename Traits, bool sign, bool unrolled = false>
auto minmax_impl(const void* first, const void* const last) noexcept
{
    using T = std::conditional_t<sign, typename Traits::signed_t, typename Traits::unsigned_t>;
    using vec_t = typename Traits::vec_t;

    T cur_min_val; // initialized in both of the branches below
    T cur_max_val; // initialized in both of the branches below

    VX_IF_CONSTEXPR (Traits::vectorized)
    {
        constexpr size_t lanes = unrolled ? 2 : 1;
        constexpr size_t bytes_per_iter = lanes * Traits::vec_size;

        const size_t total_size_bytes = byte_length(first, last);
        const size_t vec_byte_size = total_size_bytes & ~(bytes_per_iter - 1);

        const void* stop_at = first;
        advance_bytes(stop_at, vec_byte_size);

        // We don't have unsigned 64-bit stuff, so we'll use sign correction just for that case
        constexpr bool sign_correction = sizeof(T) == 8 && !sign && !Traits::has_unsigned_cmp;

        vec_t cur_vals[lanes];
        vec_t cur_vals_min[lanes]; // vector of vertical minimum values
        vec_t cur_vals_max[lanes]; // vector of vertical maximum values
        for (size_t lane = 0; lane < lanes; ++lane)
        {
            cur_vals[lane] = Traits::load(static_cast<const uint8_t*>(first) + lane * Traits::vec_size);
            VX_IF_CONSTEXPR (sign_correction)
            {
                cur_vals[lane] = Traits::sign_correction(cur_vals[lane], false);
            }
            cur_vals_min[lane] = cur_vals[lane];
            cur_vals_max[lane] = cur_vals[lane];
        }

        const auto update_min_max = [&](const auto cur_vals, size_t lane = 0) noexcept
        {
            VX_IF_CONSTEXPR ((mode & mode_min) != 0)
            {
                VX_IF_CONSTEXPR (sign || sign_correction)
                {
                    cur_vals_min[lane] =
                        Traits::min(cur_vals_min[lane], cur_vals); // Update the current minimum
                }
                else
                {
                    cur_vals_min[lane] =
                        Traits::min_u(cur_vals_min[lane], cur_vals); // Update the current minimum
                }
            }

            VX_IF_CONSTEXPR ((mode & mode_max) != 0)
            {
                VX_IF_CONSTEXPR (sign || sign_correction)
                {
                    cur_vals_max[lane] =
                        Traits::max(cur_vals_max[lane], cur_vals); // Update the current maximum
                }
                else
                {
                    cur_vals_max[lane] =
                        Traits::max_u(cur_vals_max[lane], cur_vals); // Update the current maximum
                }
            }
        };

        for (;;)
        {
            advance_bytes(first, bytes_per_iter);

            if (first != stop_at)
            {
                // This is the main part, finding vertical minimum/maximum

                for (size_t lane = 0; lane < lanes; ++lane)
                {
                    cur_vals[lane] =
                        Traits::load(static_cast<const uint8_t*>(first) + lane * Traits::vec_size);

                    VX_IF_CONSTEXPR (sign_correction)
                    {
                        cur_vals[lane] = Traits::sign_correction(cur_vals[lane], false);
                    }

                    update_min_max(cur_vals[lane], lane);
                }
            }
            else
            {
                VX_IF_CONSTEXPR (unrolled)
                {
                    const bool has_vec_tail = (byte_length(first, last) & ~Traits::vec_mask) != 0;

                    if (has_vec_tail)
                    {
                        cur_vals[0] = Traits::load(first);

                        VX_IF_CONSTEXPR (sign_correction)
                        {
                            cur_vals[0] = Traits::sign_correction(cur_vals[0], false);
                        }

                        update_min_max(cur_vals[0], 0);

                        advance_bytes(first, Traits::vec_size);
                    }
                }

#if defined(USE_AVX2)

                VX_IF_CONSTEXPR (Traits::tail_mask != 0)
                {
                    const size_t tail_byte_size = total_size_bytes & Traits::tail_mask;
                    if (tail_byte_size != 0)
                    {
                        const auto tail_mask = avx2_tail_mask_32(tail_byte_size);
                        auto tail_vals = Traits::load_mask(first, tail_mask);

                        VX_IF_CONSTEXPR (sign_correction)
                        {
                            tail_vals = Traits::sign_correction(tail_vals, false);
                        }

                        tail_vals = Traits::blendval(cur_vals[0], tail_vals, tail_mask);

                        update_min_max(tail_vals);

                        advance_bytes(first, tail_byte_size);
                    }
                }

#endif // defined(USE_AVX2)

                // Reached end. Compute horizontal min and/or max.

                VX_IF_CONSTEXPR ((mode & mode_min) != 0)
                {
                    VX_IF_CONSTEXPR (sign || sign_correction)
                    {
                        VX_IF_CONSTEXPR (unrolled)
                        {
                            for (size_t lane = 1; lane < lanes; ++lane)
                            {
                                cur_vals_min[0] = Traits::min(cur_vals_min[0], cur_vals_min[lane]);
                            }
                        }

                        // Vector populated by the smallest element
                        const auto h_min = Traits::h_min(cur_vals_min[0]);
                        cur_min_val = Traits::get_any(h_min); // Get any element of it
                    }
                    else
                    {
                        VX_IF_CONSTEXPR (unrolled)
                        {
                            for (size_t lane = 1; lane < lanes; ++lane)
                            {
                                cur_vals_min[0] = Traits::min_u(cur_vals_min[0], cur_vals_min[lane]);
                            }
                        }

                        // Vector populated by the smallest element
                        const auto h_min = Traits::h_min_u(cur_vals_min[0]);
                        cur_min_val = Traits::get_any(h_min); // Get any element of it
                    }
                }

                VX_IF_CONSTEXPR ((mode & mode_max) != 0)
                {
                    VX_IF_CONSTEXPR (sign || sign_correction)
                    {
                        VX_IF_CONSTEXPR (unrolled)
                        {
                            for (size_t lane = 1; lane < lanes; ++lane)
                            {
                                cur_vals_max[0] = Traits::max(cur_vals_max[0], cur_vals_max[lane]);
                            }
                        }

                        // Vector populated by the largest element
                        const auto h_max = Traits::h_max(cur_vals_max[0]);
                        cur_max_val = Traits::get_any(h_max); // Get any element of it
                    }
                    else
                    {
                        VX_IF_CONSTEXPR (unrolled)
                        {
                            for (size_t lane = 1; lane < lanes; ++lane)
                            {
                                cur_vals_max[0] = Traits::max_u(cur_vals_max[0], cur_vals_max[lane]);
                            }
                        }

                        // Vector populated by the largest element
                        const auto h_max = Traits::h_max_u(cur_vals_max[0]);
                        cur_max_val = Traits::get_any(h_max); // Get any element of it
                    }
                }

                VX_IF_CONSTEXPR (sign_correction)
                {
                    constexpr T correction = T{ 1 } << (sizeof(T) * 8 - 1);

                    VX_IF_CONSTEXPR ((mode & mode_min) != 0)
                    {
                        cur_min_val += correction;
                    }

                    VX_IF_CONSTEXPR ((mode & mode_max) != 0)
                    {
                        cur_max_val += correction;
                    }
                }

                break;
            }
        }

        Traits::exit_vectorized(); // TRANSITION, DevCom-10331414
    }
    else
    {
        cur_min_val = *reinterpret_cast<const T*>(first);
        cur_max_val = *reinterpret_cast<const T*>(first);

        advance_bytes(first, sizeof(T));
    }

// Avoid auto-vectorization of the scalar tail, as this is not beneficial for performance.
#if defined(_MSC_VER)
    #pragma loop(no_vector)
#elif defined(__clang__)
    _Pragma("clang loop vectorize(disable)")
#elif defined(__GNUC__)
    #if __GNUC__ >= 10
    _Pragma("GCC loop vectorize(disable)")
    #endif
#endif

    for (auto ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        VX_IF_CONSTEXPR ((mode & mode_min) != 0)
        {
            if (*ptr < cur_min_val)
            {
                cur_min_val = *ptr;
            }
        }

        VX_IF_CONSTEXPR ((mode & mode_max) != 0)
        {
            if (cur_max_val < *ptr)
            {
                cur_max_val = *ptr;
            }
        }

        // mode_both could have been handled separately with 'else'.
        // We have cur_min_val / cur_max_val initialized by processing at least one element,
        // so the 'else' would be correct here.
        // But still separate 'if' statements promote branchless codegen.
    }

    VX_IF_CONSTEXPR (mode == mode_min)
    {
        return cur_min_val;
    }
    else VX_IF_CONSTEXPR (mode == mode_max)
    {
        return cur_max_val;
    }
    else
    {
        using Rx = std::conditional_t<sign, typename Traits::minmax_i_t, typename Traits::minmax_u_t>;
        return Rx{ cur_min_val, cur_max_val };
    }
}

#if !defined(USE_ARM_NEON)

// TRANSITION, DevCom-10767462
template <min_max_mode mode, typename Traits, bool sign>
auto minmax_impl_wrap(const void* const first, const void* const last) noexcept
{
    auto rx = minmax_impl<mode, Traits, sign>(first, last);
    _mm256_zeroupper();
    return rx;
}

#endif // !defined(USE_ARM_NEON)

template <min_max_mode mode, typename Traits, bool sign>
auto VX_STDCALL minmax_disp(const void* const first, const void* const last) noexcept
{
#if defined(USE_ARM_NEON)

    if (byte_length(first, last) >= 32)
    {
        return minmax_impl<mode, typename Traits::neon, sign, true>(first, last);
    }

    if (byte_length(first, last) >= 16)
    {
        return minmax_impl<mode, typename Traits::neon, sign, false>(first, last);
    }

#elif defined(USE_AVX2)

    if (byte_length(first, last) >= 32)
    {
        VX_IF_CONSTEXPR (Traits::avx::is_floating)
        {
            return minmax_impl_wrap<mode, typename Traits::avx, sign>(first, last);
        }
        else
        {
            return minmax_impl<mode, typename Traits::avx, sign>(first, last);
        }
    }

#elif defined(USE_SSE2)

    if (byte_length(first, last) >= 16)
    {
        return minmax_impl<mode, typename Traits::sse, sign>(first, last);
    }

#endif

    return minmax_impl<mode, typename Traits::scalar, sign>(first, last);
}

#if defined(USE_ARM_NEON)

template <typename Traits, typename T>
const void* is_sorted_until_impl(const void* first, const void* const last, const bool greater) noexcept
{
    const ptrdiff_t left_off = 0 - static_cast<ptrdiff_t>(greater);
    const ptrdiff_t right_off = static_cast<ptrdiff_t>(greater) - 1;

    VX_IF_CONSTEXPR (Traits::vectorized)
    {
        const size_t total_size_bytes = byte_length(first, last);

        const auto cmp_gt_wrap = [](const auto right, const auto left) noexcept
        {
            constexpr bool unsigned_ = static_cast<T>(-1) > T{ 0 };

            VX_IF_CONSTEXPR (unsigned_ && Traits::has_unsigned_cmp)
            {
                return Traits::cmp_gt_u(right, left);
            }
            else
            {
                return Traits::cmp_gt(right, left);
            }
        };

        if (total_size_bytes >= 32)
        {
            constexpr size_t bytes_per_iter = 2 * Traits::vec_size;
            const size_t vec_byte_size = total_size_bytes & ~(bytes_per_iter - 1);
            const void* stop_at = first;
            advance_bytes(stop_at, vec_byte_size);

            do
            {
                const void* const next = static_cast<const uint8_t*>(first) + Traits::vec_size;

                const auto left_lo = Traits::load(static_cast<const T*>(first) + left_off);
                const auto right_lo = Traits::load(static_cast<const T*>(first) + right_off);
                const auto left_hi = Traits::load(static_cast<const T*>(next) + left_off);
                const auto right_hi = Traits::load(static_cast<const T*>(next) + right_off);

                const auto is_less_lo = cmp_gt_wrap(right_lo, left_lo);
                const auto is_less_hi = cmp_gt_wrap(right_hi, left_hi);
                const auto any_match = Traits::match_mask(is_less_lo, is_less_hi);

                if (any_match != 0)
                {
                    const auto mask_lo = Traits::mask(is_less_lo);
                    if (mask_lo != 0)
                    {
                        const unsigned long h_pos = Traits::get_first_h_pos(mask_lo);
                        advance_bytes(first, h_pos);
                        return first;
                    }

                    const auto mask_hi = Traits::mask(is_less_hi);
                    const unsigned long h_pos = Traits::get_first_h_pos(mask_hi) + Traits::vec_size;
                    advance_bytes(first, h_pos);
                    return first;
                }

                advance_bytes(first, 2 * Traits::vec_size);
            } while (first != stop_at);
        }

        const size_t has_vec_tail = (byte_length(first, last) & ~Traits::vec_mask) != 0;
        if (has_vec_tail)
        {
            const auto left = Traits::load(static_cast<const T*>(first) + left_off);
            const auto right = Traits::load(static_cast<const T*>(first) + right_off);

            const auto is_less = cmp_gt_wrap(right, left);
            const auto mask = Traits::mask(Traits::mask_cast(is_less));

            if (mask != 0)
            {
                const unsigned long h_pos = Traits::get_first_h_pos(mask);
                advance_bytes(first, h_pos);
                return first;
            }

            advance_bytes(first, Traits::vec_size);
        }
    }

    for (const T* ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        if (ptr[left_off] < ptr[right_off])
        {
            return ptr;
        }
    }

    return last;
}

#else

template <typename Traits, typename T>
const void* is_sorted_until_impl(const void* first, const void* const last, const bool greater) noexcept
{
    const ptrdiff_t left_off = 0 - static_cast<ptrdiff_t>(greater);
    const ptrdiff_t right_off = static_cast<ptrdiff_t>(greater) - 1;

    VX_IF_CONSTEXPR (Traits::vectorized)
    {
        constexpr bool sign_cor = static_cast<T>(-1) > T{ 0 };
        VX_MAYBE_UNUSED typename Traits::guard guard; // TRANSITION, DevCom-10331414

        const size_t total_size_bytes = byte_length(first, last);
        const size_t vec_byte_size = total_size_bytes & ~Traits::vec_mask;

        const void* stop_at = first;
        advance_bytes(stop_at, vec_byte_size);

        do
        {
            auto left = Traits::load(static_cast<const T*>(first) + left_off);
            auto right = Traits::load(static_cast<const T*>(first) + right_off);

            VX_IF_CONSTEXPR (sign_cor)
            {
                left = Traits::sign_correction(left, false);
                right = Traits::sign_correction(right, false);
            }

            const auto is_less = Traits::cmp_gt(right, left);
            const auto mask = Traits::mask(Traits::mask_cast(is_less));

            if (mask != 0)
            {
                const unsigned long h_pos = Traits::get_first_h_pos(mask);
                advance_bytes(first, h_pos);
                return first;
            }

            advance_bytes(first, Traits::vec_size);
        } while (first != stop_at);

        VX_IF_CONSTEXPR (Traits::tail_mask != 0)
        {
            const size_t tail_byte_size = total_size_bytes & Traits::tail_mask;
            if (tail_byte_size != 0)
            {
    #if defined(USE_AVX2)

                const auto tail_mask = avx2_tail_mask_32(tail_byte_size);

                auto left = Traits::load_mask(static_cast<const T*>(first) + left_off, tail_mask);
                auto right = Traits::load_mask(static_cast<const T*>(first) + right_off, tail_mask);

                VX_IF_CONSTEXPR (sign_cor)
                {
                    left = Traits::sign_correction(left, false);
                    right = Traits::sign_correction(right, false);
                }

                const auto is_less = Traits::cmp_gt(right, left);
                const auto mask = Traits::mask(_mm256_and_si256(Traits::mask_cast(is_less), tail_mask));

                if (mask != 0)
                {
                    const unsigned long h_pos = Traits::get_first_h_pos(mask);
                    advance_bytes(first, h_pos);
                    return first;
                }

                advance_bytes(first, tail_byte_size);

    #endif // defined(USE_AVX2)
            }
        }
    }

    VX_IF_CONSTEXPR ((Traits::tail_mask & sizeof(T)) != sizeof(T))
    {
        for (const T* ptr = static_cast<const T*>(first); ptr != last; ++ptr)
        {
            if (ptr[left_off] < ptr[right_off])
            {
                return ptr;
            }
        }
    }

    return last;
}

#endif

template <typename Traits, typename T>
const void* VX_STDCALL is_sorted_until_disp(
    const void* first,
    const void* const last,
    const bool greater) noexcept
{
    if (first == last)
    {
        return first;
    }

    advance_bytes(first, sizeof(T));

#if defined(USE_ARM_NEON)

    if (byte_length(first, last) >= 16)
    {
        return is_sorted_until_impl<typename Traits::neon, T>(first, last, greater);
    }

#elif defined(USE_X86)

    #if defined(USE_AVX2)

    if (byte_length(first, last) >= 32)
    {
        return is_sorted_until_impl<typename Traits::avx, T>(first, last, greater);
    }

    #endif

    #if defined(USE_SSE2)

    if (byte_length(first, last) >= 16)
    {
        return is_sorted_until_impl<typename Traits::sse, T>(first, last, greater);
    }

    #endif

#endif

    return is_sorted_until_impl<typename Traits::scalar, T>(first, last, greater);
}
} // namespace _sort

//=============================================================================
// sort functions
//=============================================================================

extern "C" {

const void* VX_STDCALL min_element_1(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_1>(first, last, signed_);
}

const void* VX_STDCALL min_element_2(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_2>(first, last, signed_);
}

const void* VX_STDCALL min_element_4(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_4>(first, last, signed_);
}

#ifndef _M_ARM64
const void* VX_STDCALL min_element_8(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_8>(first, last, signed_);
}
#endif // ^^^ !defined(_M_ARM64) ^^^

// TRANSITION, ABI: remove unused `bool`
const void* VX_STDCALL min_element_f(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_f>(first, last, false);
}

// TRANSITION, ABI: remove unused `bool`
const void* VX_STDCALL min_element_d(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_min, _sort::traits_d>(first, last, false);
}

const void* VX_STDCALL max_element_1(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_1>(first, last, signed_);
}

const void* VX_STDCALL max_element_2(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_2>(first, last, signed_);
}

const void* VX_STDCALL max_element_4(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_4>(first, last, signed_);
}

#ifndef _M_ARM64
const void* VX_STDCALL max_element_8(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_8>(first, last, signed_);
}
#endif // ^^^ !defined(_M_ARM64) ^^^

// TRANSITION, ABI: remove unused `bool`
const void* VX_STDCALL max_element_f(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_f>(first, last, false);
}

// TRANSITION, ABI: remove unused `bool`
const void* VX_STDCALL max_element_d(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_max, _sort::traits_d>(first, last, false);
}

_priv::min_max_element_t VX_STDCALL minmax_element_1(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_1>(first, last, signed_);
}

_priv::min_max_element_t VX_STDCALL minmax_element_2(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_2>(first, last, signed_);
}

_priv::min_max_element_t VX_STDCALL minmax_element_4(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_4>(first, last, signed_);
}

#ifndef _M_ARM64
_priv::min_max_element_t VX_STDCALL minmax_element_8(
    const void* const first,
    const void* const last,
    const bool signed_) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_8>(first, last, signed_);
}
#endif // ^^^ !defined(_M_ARM64) ^^^

// TRANSITION, ABI: remove unused `bool`
_priv::min_max_element_t VX_STDCALL minmax_element_f(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_f>(first, last, false);
}

// TRANSITION, ABI: remove unused `bool`
_priv::min_max_element_t VX_STDCALL minmax_element_d(const void* const first, const void* const last, bool) noexcept
{
    return _sort::minmax_element_disp<_sort::mode_both, _sort::traits_d>(first, last, false);
}

VX_NO_ALIAS int8_t VX_STDCALL min_1i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_1, true>(first, last);
}

VX_NO_ALIAS uint8_t VX_STDCALL min_1u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_1, false>(first, last);
}

VX_NO_ALIAS int16_t VX_STDCALL min_2i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_2, true>(first, last);
}

VX_NO_ALIAS uint16_t VX_STDCALL min_2u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_2, false>(first, last);
}

VX_NO_ALIAS int32_t VX_STDCALL min_4i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_4, true>(first, last);
}

VX_NO_ALIAS uint32_t VX_STDCALL min_4u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_4, false>(first, last);
}

VX_NO_ALIAS int64_t VX_STDCALL min_8i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_8, true>(first, last);
}

VX_NO_ALIAS uint64_t VX_STDCALL min_8u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_8, false>(first, last);
}

VX_NO_ALIAS float VX_STDCALL min_f(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_f, true>(first, last);
}

VX_NO_ALIAS double VX_STDCALL min_d(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_min, _sort::traits_d, true>(first, last);
}

VX_NO_ALIAS int8_t VX_STDCALL max_1i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_1, true>(first, last);
}

VX_NO_ALIAS uint8_t VX_STDCALL max_1u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_1, false>(first, last);
}

VX_NO_ALIAS int16_t VX_STDCALL max_2i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_2, true>(first, last);
}

VX_NO_ALIAS uint16_t VX_STDCALL max_2u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_2, false>(first, last);
}

VX_NO_ALIAS int32_t VX_STDCALL max_4i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_4, true>(first, last);
}

VX_NO_ALIAS uint32_t VX_STDCALL max_4u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_4, false>(first, last);
}

VX_NO_ALIAS int64_t VX_STDCALL max_8i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_8, true>(first, last);
}

VX_NO_ALIAS uint64_t VX_STDCALL max_8u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_8, false>(first, last);
}

VX_NO_ALIAS float VX_STDCALL max_f(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_f, true>(first, last);
}

VX_NO_ALIAS double VX_STDCALL max_d(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_max, _sort::traits_d, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_1i VX_STDCALL minmax_1i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_1, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_1u VX_STDCALL minmax_1u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_1, false>(first, last);
}

VX_NO_ALIAS _priv::min_max_2i VX_STDCALL minmax_2i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_2, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_2u VX_STDCALL minmax_2u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_2, false>(first, last);
}

VX_NO_ALIAS _priv::min_max_4i VX_STDCALL minmax_4i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_4, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_4u VX_STDCALL minmax_4u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_4, false>(first, last);
}

VX_NO_ALIAS _priv::min_max_8i VX_STDCALL minmax_8i(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_8, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_8u VX_STDCALL minmax_8u(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_8, false>(first, last);
}

VX_NO_ALIAS _priv::min_max_f VX_STDCALL minmax_f(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_f, true>(first, last);
}

VX_NO_ALIAS _priv::min_max_d VX_STDCALL minmax_d(const void* const first, const void* const last) noexcept
{
    return _sort::minmax_disp<_sort::mode_both, _sort::traits_d, true>(first, last);
}

const void* VX_STDCALL is_sorted_until_1i(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_1, int8_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_1u(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_1, uint8_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_2i(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_2, int16_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_2u(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_2, uint16_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_4i(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_4, int32_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_4u(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_4, uint32_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_8i(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_8, int64_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_8u(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_8, uint64_t>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_f(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_f, float>(first, last, _Greater);
}

const void* VX_STDCALL is_sorted_until_d(
    const void* const first,
    const void* const last,
    const bool _Greater) noexcept
{
    return _sort::is_sorted_until_disp<_sort::traits_d, double>(first, last, _Greater);
}

} // extern "C"

//=============================================================================
// sorted range impl
//=============================================================================

namespace _sorted_range {

#if defined(USE_AVX2)

struct traits_avx
{
    using guard = zeroupper_on_exit;
    static constexpr size_t vec_size = 32;
    static constexpr size_t tail_mask = 0x1C;

    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i load_mask(const void* const src, const __m256i mask) noexcept
    {
        return _mm256_maskload_epi32(reinterpret_cast<const int*>(src), mask);
    }

    static unsigned long mask(const __m256i val) noexcept
    {
        return _mm256_movemask_epi8(val);
    }

    static uint32_t bsf(const uint32_t val) noexcept
    {
        return _tzcnt_u32(val);
    }
};

struct traits_1_avx : traits_avx
{
    static __m256i broadcast(const uint8_t data) noexcept
    {
        return _mm256_broadcastb_epi8(_mm_cvtsi32_si128(static_cast<uint32_t>(data)));
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi8(first, second);
    }

    static __m256i sign_correction(const __m256i data) noexcept
    {
        return _mm256_sub_epi8(data, _mm256_set1_epi8(static_cast<char>(0x80)));
    }
};

struct traits_2_avx : traits_avx
{
    static __m256i broadcast(const uint16_t data) noexcept
    {
        return _mm256_broadcastw_epi16(_mm_cvtsi32_si128(static_cast<uint32_t>(data)));
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi16(first, second);
    }

    static __m256i sign_correction(const __m256i data) noexcept
    {
        return _mm256_sub_epi16(data, _mm256_set1_epi16(static_cast<short>(0x8000)));
    }
};

struct traits_4_avx : traits_avx
{
    static __m256i broadcast(const uint32_t data) noexcept
    {
        return _mm256_broadcastd_epi32(_mm_cvtsi32_si128(data));
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi32(first, second);
    }

    static __m256i sign_correction(const __m256i data) noexcept
    {
        return _mm256_sub_epi32(data, _mm256_set1_epi32(static_cast<long>(0x8000'0000)));
    }
};

struct traits_8_avx : traits_avx
{
    static __m256i broadcast(const uint64_t data) noexcept
    {
    #if defined(VX_ARCH_WORD_BITS_64)
        return _mm256_broadcastq_epi64(_mm_cvtsi64x_si128(data));
    #else
        return _mm256_set1_epi64x(data);
    #endif
    }

    static __m256i cmp_gt(const __m256i first, const __m256i second) noexcept
    {
        return _mm256_cmpgt_epi64(first, second);
    }

    static __m256i sign_correction(const __m256i data) noexcept
    {
        return _mm256_sub_epi64(data, _mm256_set1_epi64x(static_cast<long long>(0x8000'0000'0000'0000)));
    }
};

#else

using traits_1_avx = void;
using traits_2_avx = void;
using traits_4_avx = void;
using traits_8_avx = void;

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

struct traits_sse
{
    using guard = char;
    static constexpr size_t vec_size = 16;
    static constexpr size_t tail_mask = 0;

    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static unsigned long mask(const __m128i val) noexcept
    {
        return _mm_movemask_epi8(val);
    }

    static uint32_t bsf(const uint32_t val) noexcept
    {
        unsigned long index;
        // CodeQL [SM02313] index is always initialized: val != 0; see explanation at call sites.
        _BitScanForward(&index, val);
        return index;
    }
};

struct traits_1_sse : traits_sse
{
    static __m128i broadcast(const uint8_t data) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(static_cast<uint32_t>(data)), _mm_setzero_si128());
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi8(first, second);
    }

    static __m128i sign_correction(const __m128i data) noexcept
    {
        return _mm_sub_epi8(data, _mm_set1_epi8(static_cast<char>(0x80)));
    }
};

struct traits_2_sse : traits_sse
{
    static __m128i broadcast(const uint16_t data) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(static_cast<uint32_t>(data)), _mm_set1_epi16(0x0100));
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi16(first, second);
    }

    static __m128i sign_correction(const __m128i data) noexcept
    {
        return _mm_sub_epi16(data, _mm_set1_epi16(static_cast<short>(0x8000)));
    }
};

struct traits_4_sse : traits_sse
{
    static __m128i broadcast(const uint32_t data) noexcept
    {
        return _mm_shuffle_epi32(_mm_cvtsi32_si128(data), _MM_SHUFFLE(0, 0, 0, 0));
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi32(first, second);
    }

    static __m128i sign_correction(const __m128i data) noexcept
    {
        return _mm_sub_epi32(data, _mm_set1_epi32(static_cast<long>(0x8000'0000)));
    }
};

struct traits_8_sse : traits_sse
{
    static __m128i broadcast(const uint64_t data) noexcept
    {
    #if defined(VX_ARCH_WORD_BITS_64)
        return _mm_shuffle_epi32(_mm_cvtsi64x_si128(data), _MM_SHUFFLE(1, 0, 1, 0));
    #else
        return _mm_set1_epi64x(data);
    #endif
    }

    static __m128i cmp_gt(const __m128i first, const __m128i second) noexcept
    {
        return _mm_cmpgt_epi64(first, second);
    }

    static __m128i sign_correction(const __m128i data) noexcept
    {
        return _mm_sub_epi64(data, _mm_set1_epi64x(static_cast<long long>(0x8000'0000'0000'0000)));
    }
};

#else

using traits_1_sse = void;
using traits_2_sse = void;
using traits_4_sse = void;
using traits_8_sse = void;

#endif // defined(USE_SSE2)

template <typename Traits, typename T>
bool includes_impl(
    const void* first1,
    const void* const last1,
    const void* first2,
    const void* const last2) noexcept
{
    VX_IF_CONSTEXPR (!std::is_void_v<Traits>)
    {
        // Only skipping some parts of haystack that are less than current needle element is vectorized.
        // Otherwise this is scalar algorithm.

        constexpr bool is_signed = std::is_signed_v<T>;
        constexpr uint32_t all_ones_mask = uint32_t{ (uint64_t{ 1 } << Traits::vec_size) - 1 };
        constexpr uint32_t highest_one_mask = 1u << (Traits::vec_size - 1);
        VX_MAYBE_UNUSED typename Traits::guard guard; // TRANSITION, DevCom-10331414

        const size_t size_bytes_1 = byte_length(first1, last1);
        const void* stop1 = first1;
        advance_bytes(stop1, size_bytes_1 & ~(Traits::vec_size - 1));

        T val2 = *reinterpret_cast<const T*>(first2);
        auto start2 = Traits::broadcast(val2);
        VX_IF_CONSTEXPR (!is_signed)
        {
            start2 = Traits::sign_correction(start2);
        }

        do
        {
            auto data1 = Traits::load(first1);
            VX_IF_CONSTEXPR (!is_signed)
            {
                data1 = Traits::sign_correction(data1);
            }

            const void* next1 = first1;
            advance_bytes(next1, Traits::vec_size);

            const uint32_t greater_start_2 = Traits::mask(Traits::cmp_gt(start2, data1));
            // Testing highest_one_mask can be a bit more efficient on AVX2 than comparing against
            // all_ones_mask (will test sign, and can share comparison with != 0 below).
            if ((greater_start_2 & highest_one_mask) != 0)
            {
                // Needle first element is greater than each element of haystack vector.
                // Proceed to the next one, without updating the needle comparand.
                first1 = next1;
            }
            else
            {
                if (greater_start_2 != 0)
                {
                    // Needle first element is greater than some first elements of haystack part.
                    // Advance past these elements.
                    // The input is nonzero because we handled that case with highest_one_mask branch above.
                    const uint32_t skip = Traits::bsf(greater_start_2 ^ all_ones_mask);
                    advance_bytes(first1, skip);
                }

                // The rest is scalar loop that completes the remaining vector-sized haystack part.
                // Except that it updates current needle value to compare against.
                do
                {
                    const T val1 = *static_cast<const T*>(first1);

                    if (val2 < val1)
                    {
                        return false;
                    }

                    if (val2 == val1)
                    {
                        advance_bytes(first2, sizeof(T));
                        if (first2 == last2)
                        {
                            return true;
                        }

                        val2 = *reinterpret_cast<const T*>(first2);
                    }

                    advance_bytes(first1, sizeof(T));
                } while (first1 != next1);

                start2 = Traits::broadcast(val2);
                VX_IF_CONSTEXPR (!is_signed)
                {
                    start2 = Traits::sign_correction(start2);
                }
            }
        } while (first1 != stop1);

        VX_IF_CONSTEXPR (Traits::tail_mask != 0)
        {
            const size_t tail_bytes_size_1 = size_bytes_1 & Traits::tail_mask;
            if (tail_bytes_size_1 != 0)
            {
#if defined(USE_AVX2)

                // Just try to advance past less one more time.
                // Don't need to repeat the scalar part here - falling to scalar loop anyway.
                const auto tail_mask = avx2_tail_mask_32(tail_bytes_size_1);
                auto data1 = Traits::load_mask(first1, tail_mask);
                VX_IF_CONSTEXPR (!is_signed)
                {
                    data1 = Traits::sign_correction(data1);
                }

                const auto cmp = Traits::cmp_gt(start2, data1);
                const uint32_t greater_start_2 = Traits::mask(_mm256_and_si256(cmp, tail_mask));
                if (greater_start_2 != 0)
                {
                    // Needle first element is greater than some first elements of haystack part.
                    // Advance past these elements.
                    // The input is nonzero because tail mask will have zeros for remaining elements.
                    const uint32_t skip = Traits::bsf(greater_start_2 ^ all_ones_mask);
                    advance_bytes(first1, skip);
                }

#endif // defined(USE_AVX2)
            }
        }

        if (first1 == last1)
        {
            return false;
        }
    }

    auto ptr1 = static_cast<const T*>(first1);
    auto ptr2 = static_cast<const T*>(first2);

    for (;;)
    {
        if (*ptr1 < *ptr2)
        {
            ++ptr1;
            if (ptr1 == last1)
            {
                return false;
            }
        }
        else if (*ptr2 < *ptr1)
        {
            return false;
        }
        else
        {
            ++ptr1;
            ++ptr2;
            if (ptr2 == last2)
            {
                return true;
            }
            else if (ptr1 == last1)
            {
                return false;
            }
        }
    }
}

template <typename traits_avx, typename traits_sse, typename T>
bool VX_STDCALL includes_disp(const void* const first1, const void* const last1, const void* const first2, const void* const last2) noexcept
{
    const size_t size_bytes_1 = byte_length(first1, last1);
    const size_t size_bytes_2 = byte_length(first2, last2);
    if (size_bytes_2 == 0)
    {
        return true;
    }
    else if (size_bytes_1 < size_bytes_2)
    {
        return false;
    }

#if defined(USE_AVX2)

    if (size_bytes_1 >= 32)
    {
        return includes_impl<traits_avx, T>(first1, last1, first2, last2);
    }

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

    if (size_bytes_1 >= 16)
    {
        return includes_impl<traits_sse, T>(first1, last1, first2, last2);
    }

#endif // defined(USE_SSE2)

    return includes_impl<void, T>(first1, last1, first2, last2);
}

} // namespace _sorted_range

//=============================================================================
// sorted range functions
//=============================================================================

extern "C" {

VX_NO_ALIAS bool VX_STDCALL includes_less_1i(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_1_avx, _sorted_range::traits_1_sse, int8_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_1u(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_1_avx, _sorted_range::traits_1_sse, uint8_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_2i(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_2_avx, _sorted_range::traits_2_sse, int16_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_2u(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_2_avx, _sorted_range::traits_2_sse, uint16_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_4i(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_4_avx, _sorted_range::traits_4_sse, int32_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_4u(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_4_avx, _sorted_range::traits_4_sse, uint32_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_8i(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_8_avx, _sorted_range::traits_8_sse, int64_t>(
        first1, last1, first2, last2);
}

VX_NO_ALIAS bool VX_STDCALL includes_less_8u(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _sorted_range::includes_disp<_sorted_range::traits_8_avx, _sorted_range::traits_8_sse, uint64_t>(
        first1, last1, first2, last2);
}

} // extern "C"

} // namespace _simd
} // namespace vx
