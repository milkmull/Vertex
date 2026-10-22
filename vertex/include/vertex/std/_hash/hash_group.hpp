#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/std/bit.hpp"
#include "vertex/std/endian.hpp"
#include "vertex/std/memory.hpp"

//=========================================================================
// simd setup
//=========================================================================

#if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
    #define VX_FLAT_HASH_SSE2 1
    #include <emmintrin.h>
#else
    #define VX_FLAT_HASH_SSE2 0
#endif

#if !VX_FLAT_HASH_SSE2 && defined(VX_SIMD_ARM) && (VX_SIMD_ARM >= VX_SIMD_ARM_NEON_VERSION)
    #define VX_FLAT_HASH_NEON 1
    #include <arm_neon.h>
#else
    #define VX_FLAT_HASH_NEON 0
#endif

//=========================================================================

namespace vx {
namespace _hash_group_priv {

//=========================================================================
// control bytes
//=========================================================================

// One control byte per slot. Encoding:
//
//   0x00..0x7F  FULL      0xxxxxxx, the low 7 bits are the H2 fingerprint
//   0x80        EMPTY     10000000
//   0xFE        DELETED   11111110 (tombstone)
//   0xFF        SENTINEL  11111111 (at ctrl[capacity], never matches anything)
//
// The group ops in group_swar depend on these bit patterns:
//   - bit 7 clear : FULL (is_full, match_full, match_empty*, convert_*)
//   - bit 1 clear : EMPTY among the specials (match_empty)
//   - bit 0 clear : EMPTY or DELETED, not SENTINEL (match_empty_or_deleted)

using ctrl_t = int8_t; // signed so the SSE2 signed compares work later
VX_STATIC_ASSERT_MSG(std::is_trivial<ctrl_t>::value, "ctrl_t must be trivial");

// NOTE: all special control bytes must have the high bit set
// only EMPTY may have bit 1 clear
// EMPTY and DELETED need bit 0 clear, SENTINEL needs it set

enum : ctrl_t
{
    // empty slot, never populated
    ctrl_empty = static_cast<ctrl_t>(0x80),
    // slot was occupied but entry was deleted
    ctrl_deleted = static_cast<ctrl_t>(0xFE),
    // end of the group
    ctrl_sentinel = static_cast<ctrl_t>(0xFF)
};

// indicates a slot is occupied (FULL) or not (EMPTY, DELETED, SENTINEL)
inline bool slot_is_full(ctrl_t c) noexcept
{
    return c >= 0;
}

// EMPTY (-128) and DELETED (-2) are below the sentinel (-1); FULL is >= 0.
inline bool slot_is_empty_or_deleted(ctrl_t c) noexcept
{
    return c < ctrl_sentinel;
}

// fingerprint indicating a slot is occupied (only the low 7 bits are used)
using h2_t = uint8_t; // always in [0, 127]

//=========================================================================
// bit_mask
//=========================================================================

template <typename Word, int Shift, uint32_t Pad = 0>
class bit_mask
{
public:

    explicit bit_mask(Word bits) noexcept
        : m_bits(bits)
    {}

    explicit operator bool() const noexcept
    {
        return m_bits != 0;
    }

    // lane index of the lowest set lane
    uint32_t lowest() const noexcept
    {
        VX_ASSERT(m_bits != 0);
        // we would only call this if we already know there is a bit set
        return static_cast<uint32_t>(bit::countr_zero_assume_nonzero(m_bits)) >> Shift;
    }

    // number of clear lanes before the first set lane (== width if none)
    uint32_t trailing_zeros() const noexcept
    {
        constexpr Word stop = Pad ? static_cast<Word>(Word(1) << Pad) : Word(0);
        return static_cast<uint32_t>(bit::countr_zero(static_cast<Word>(m_bits | stop))) >> Shift;
    }

    // number of clear lanes after the last set lane (== width if none)
    uint32_t leading_zeros() const noexcept
    {
        return (static_cast<uint32_t>(bit::countl_zero(m_bits)) - Pad) >> Shift;
    }

    bit_mask begin() const noexcept
    {
        return *this;
    }

    bit_mask end() const noexcept
    {
        return bit_mask(0);
    }

    uint32_t operator*() const noexcept
    {
        return lowest();
    }

    bit_mask& operator++() noexcept
    {
        m_bits &= (m_bits - 1);
        return *this;
    }

    bool operator!=(const bit_mask& o) const noexcept
    {
        return m_bits != o.m_bits;
    }

private:

    Word m_bits;
};

//=========================================================================
// SWAR Group (SIMD within a register)
//=========================================================================

// SWAR group (width 8)
class group_swar
{
public:

    enum : size_t
    {
        width = 8
    };

    using mask_type = bit_mask<uint64_t, 3>;

    explicit group_swar(const ctrl_t* pos) noexcept
    {
        uint64_t raw;
        mem::copy(&raw, pos, sizeof(raw));
        // lane i == byte i of m_word on every target
        m_word = endian::little_to_native(raw);
    }

    // May report false positives (only in a lane above a true match), never
    // false negatives. Callers verify with Eq (or the cached hash) anyway.
    mask_type match(h2_t h2) const noexcept
    {
        const uint64_t x = m_word ^ (lsbs * h2);
        return mask_type((x - lsbs) & ~x & msbs);
    }

    // byte == 0x80
    mask_type match_empty() const noexcept
    {
        return mask_type((m_word & ~(m_word << 6)) & msbs);
    }

    // byte is 0x80 or 0xFE (sentinel excluded)
    mask_type match_empty_or_deleted() const noexcept
    {
        return mask_type((m_word & ~(m_word << 7)) & msbs);
    }

    // high bit clear
    mask_type match_full() const noexcept
    {
        return mask_type(~m_word & msbs);
    }

    // FULL or SENTINEL, (not EMPTY and not DELETED)
    // Iteration stops on these
    mask_type match_full_or_sentinel() const noexcept
    {
        return mask_type((~m_word | (m_word << 7)) & msbs);
    }

private:

    static constexpr uint64_t lsbs = 0x0101010101010101ULL;
    static constexpr uint64_t msbs = 0x8080808080808080ULL;

    uint64_t m_word;
};

//=========================================================================
// SSE2 Group
//=========================================================================

#if VX_FLAT_HASH_SSE2

// SSE2 group (width 16)
class group_sse2
{
public:

    enum : size_t
    {
        width = 16
    };

    // 16 mask bits live in the low half of a uint32_t
    using mask_type = bit_mask<uint32_t, 0, 16>;

    explicit group_sse2(const ctrl_t* pos) noexcept
        : m_ctrl(_mm_loadu_si128(reinterpret_cast<const __m128i*>(pos)))
    {}

    mask_type match(h2_t h2) const noexcept
    {
        return to_mask(_mm_cmpeq_epi8(m_ctrl, splat(static_cast<ctrl_t>(h2))));
    }

    mask_type match_empty() const noexcept
    {
        return to_mask(_mm_cmpeq_epi8(m_ctrl, splat(ctrl_empty)));
    }

    // c < -1: EMPTY or DELETED, sentinel excluded
    mask_type match_empty_or_deleted() const noexcept
    {
        return to_mask(_mm_cmpgt_epi8(splat(ctrl_sentinel), m_ctrl));
    }

    // c >= 0
    mask_type match_full() const noexcept
    {
        return to_mask(_mm_cmpgt_epi8(m_ctrl, splat(-1)));
    }

    // c > -2: FULL or SENTINEL. Iteration stops on these.
    mask_type match_full_or_sentinel() const noexcept
    {
        return to_mask(_mm_cmpgt_epi8(m_ctrl, splat(-2)));
    }

private:

    static __m128i splat(int v) noexcept
    {
        return _mm_set1_epi8(static_cast<char>(v));
    }

    static mask_type to_mask(__m128i v) noexcept
    {
        return mask_type(static_cast<uint32_t>(_mm_movemask_epi8(v)));
    }

    __m128i m_ctrl;
};

using group = group_sse2;

//=========================================================================
// NEON Group
//=========================================================================

#elif VX_FLAT_HASH_NEON

// NEON group (width 8)
class group_neon
{
public:

    enum : size_t
    {
        width = 8
    };

    // same layout as group_swar: 0x80 in each matching byte
    using mask_type = bit_mask<uint64_t, 3>;

    explicit group_neon(const ctrl_t* pos) noexcept
        : m_ctrl(vld1_s8(pos))
    {}

    mask_type match(h2_t h2) const noexcept
    {
        return to_mask(vceq_s8(m_ctrl, vdup_n_s8(static_cast<int8_t>(h2))));
    }

    mask_type match_empty() const noexcept
    {
        return to_mask(vceq_s8(m_ctrl, vdup_n_s8(ctrl_empty)));
    }

    // c < -1: EMPTY or DELETED, sentinel excluded
    mask_type match_empty_or_deleted() const noexcept
    {
        return to_mask(vclt_s8(m_ctrl, vdup_n_s8(ctrl_sentinel)));
    }

    // c >= 0
    mask_type match_full() const noexcept
    {
        return to_mask(vcge_s8(m_ctrl, vdup_n_s8(0)));
    }

    // c > -2: FULL or SENTINEL. Iteration stops on these.
    mask_type match_full_or_sentinel() const noexcept
    {
        return to_mask(vcgt_s8(m_ctrl, vdup_n_s8(-2)));
    }

private:

    static mask_type to_mask(uint8x8_t v) noexcept
    {
        // 0xFF per matching byte -> 0x80, so iteration clears one lane per ++
        const uint64_t raw = vget_lane_u64(vreinterpret_u64_u8(v), 0);
        // lane i == byte i of the word on every target (same as group_swar)
        const uint64_t bits = endian::little_to_native(raw);
        return mask_type(bits & 0x8080808080808080ULL);
    }

    int8x8_t m_ctrl;
};

using group = group_neon;

#else

using group = group_swar;

#endif

constexpr size_t group_width = group::width;

//=========================================================================
// ctrl array
//=========================================================================

// Empty read only table used to prevent allocation of memory when an emty set is created
inline ctrl_t* empty_group() noexcept
{
    // clang-format off
#if VX_FLAT_HASH_SSE2

    VX_STATIC_ASSERT_MSG(group_width == 16, "empty_group() initializer assumes group_width == 16");
    alignas(group_width) static const ctrl_t group_bytes[group_width] = {
        ctrl_sentinel,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty
    };

#else

    VX_STATIC_ASSERT_MSG(group_width == 8, "empty_group() initializer assumes group_width == 8");
    alignas(group_width) static const ctrl_t group_bytes[group_width] = {
        ctrl_sentinel,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty
    };

#endif
    // clang-format on

    return const_cast<ctrl_t*>(group_bytes);
}

// Initializes the control-byte array for an allocated table by marking all
// slots and extra control bytes as empty + sentinel on the end.
inline void reset_ctrl(ctrl_t* ctrl, size_t capacity) noexcept
{
    VX_ASSERT(capacity != 0);
    mem::set(ctrl, static_cast<int>(ctrl_empty), capacity + group_width);
    ctrl[capacity] = ctrl_sentinel;
}

// Updates the control byte for slot i and its mirrored clone, if applicable.
//
// The first group_width - 1 control bytes are cloned immediately after the
// sentinel at ctrl[capacity]. These clones allow a group load near the end
// of the table to read across the table boundary in a single contiguous load,
// without requiring a separate load for the wrapped portion.
//
// Whenever one of these slots changes, its clone must be updated as well so
// lookups see consistent control bytes regardless of where a group begins.
// The remaining slots have no clones and are updated in place.
inline void set_ctrl(ctrl_t* ctrl, size_t capacity, size_t i, ctrl_t value) noexcept
{
    VX_ASSERT(i < capacity);

    constexpr size_t cloned = group_width - 1;
    const size_t mirror = i < cloned ? capacity + 1 + i : i;

    ctrl[i] = value;
    ctrl[mirror] = value;
}

} // namespace _hash_group_priv
} // namespace vx
