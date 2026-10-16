#pragma once

// Control bytes and group matching for the open-addressing backend.
// Included by hash_common.hpp because the shared growth and load policies need
// group_width. The group type is the one place SSE2/NEON gets swapped in.

#include <cstddef>
#include <cstdint>

#include "vertex/config/language_config.hpp"
#include "vertex/std/bit.hpp" // path is a guess
#include "vertex/std/endian.hpp"
#include "vertex/std/memory.hpp" // path is a guess

namespace vx {
namespace _hash_priv {

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
// The group ops in group_swar depend on these exact bit patterns:
//   - bit 7 set   <=> not full (is_full, match_full, match_empty*, convert_*)
//   - bit 1 clear <=> EMPTY among the specials       (match_empty)
//   - bit 0 clear <=> EMPTY or DELETED, not SENTINEL (match_empty_or_deleted)
// Do not change the constants without redoing those ops. The asserts below
// catch the common mistakes.
using ctrl_t = int8_t; // signed so the SSE2 signed compares work later
VX_STATIC_ASSERT_MSG(std::is_trivial<ctrl_t>::value, "ctrl_t must be trivial");

enum : ctrl_t
{
    ctrl_empty = static_cast<ctrl_t>(0x80),
    ctrl_deleted = static_cast<ctrl_t>(0xFE),
    ctrl_sentinel = static_cast<ctrl_t>(0xFF)
};

// all special control bytes must have the high bit set
VX_STATIC_ASSERT_MSG(
    ((static_cast<uint8_t>(ctrl_empty) & static_cast<uint8_t>(ctrl_deleted) & static_cast<uint8_t>(ctrl_sentinel)) & 0x80) != 0,
    "special control bytes must have the high bit set");

// only EMPTY may have bit 1 clear
VX_STATIC_ASSERT_MSG(
    (static_cast<uint8_t>(ctrl_empty) & 0x02) == 0 && (static_cast<uint8_t>(ctrl_deleted) & 0x02) != 0 && (static_cast<uint8_t>(ctrl_sentinel) & 0x02) != 0,
    "only EMPTY may have bit 1 clear");

// EMPTY and DELETED need bit 0 clear, SENTINEL needs it set
VX_STATIC_ASSERT_MSG(
    (static_cast<uint8_t>(ctrl_empty) & 0x01) == 0 && (static_cast<uint8_t>(ctrl_deleted) & 0x01) == 0 && (static_cast<uint8_t>(ctrl_sentinel) & 0x01) != 0,
    "EMPTY and DELETED need bit 0 clear, SENTINEL needs it set");

// indicates a slot is occupied (FULL) or not (EMPTY, DELETED, SENTINEL)
inline bool is_full(ctrl_t c) noexcept
{
    return c >= 0;
}

// EMPTY (-128) and DELETED (-2) are below the sentinel (-1); FULL is >= 0.
inline bool is_empty_or_deleted(ctrl_t c) noexcept
{
    return c < ctrl_sentinel;
}

// fingerprint indicating a slot is occupied (only the low 7 bits are used)
using h2_t = uint8_t; // always in [0, 127]

//=========================================================================
// bit_mask
//=========================================================================

// One "lane" per control byte. Shift is log2(bits per lane): 3 for SWAR
// (lane i is bit 8*i+7), 0 for SSE2/NEON (lane i is bit i).
template <typename Word, int Shift>
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
        return static_cast<uint32_t>(bit::countr_zero_assume_nonzero(m_bits)) >> Shift;
    }

    // number of clear lanes before the first set lane (== width if none)
    uint32_t trailing_zeros() const noexcept
    {
        return static_cast<uint32_t>(bit::countr_zero(m_bits)) >> Shift;
    }

    // number of clear lanes after the last set lane (== width if none)
    uint32_t leading_zeros() const noexcept
    {
        return static_cast<uint32_t>(bit::countl_zero(m_bits)) >> Shift;
    }

    // range-for support: visits each set lane index, lowest first
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
// SWAR group (width 8)
//=========================================================================

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
        mem::copy(&raw, pos, sizeof(raw)); // unaligned-safe
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

    // exact: byte == 0x80
    mask_type match_empty() const noexcept
    {
        return mask_type((m_word & ~(m_word << 6)) & msbs);
    }

    // exact: byte is 0x80 or 0xFE (sentinel excluded)
    mask_type match_empty_or_deleted() const noexcept
    {
        return mask_type((m_word & ~(m_word << 7)) & msbs);
    }

    // exact: high bit clear
    mask_type match_full() const noexcept
    {
        return mask_type(~m_word & msbs);
    }

    // exact: FULL or SENTINEL, i.e. not EMPTY and not DELETED. Iteration stops on these.
    // (SSE2 later: _mm_cmpgt_epi8(g, _mm_set1_epi8(-2)))
    mask_type match_full_or_sentinel() const noexcept
    {
        return mask_type((~m_word | (m_word << 7)) & msbs);
    }

    // special (empty/deleted/sentinel) -> EMPTY, full -> DELETED
    void convert_special_to_empty_and_full_to_deleted(ctrl_t* dst) const noexcept
    {
        const uint64_t x = m_word & msbs;
        const uint64_t res = (~x + (x >> 7)) & ~lsbs;
        const uint64_t raw = endian::native_to_little(res);
        mem::copy(dst, &raw, sizeof(raw));
    }

private:

    static constexpr uint64_t lsbs = 0x0101010101010101ULL;
    static constexpr uint64_t msbs = 0x8080808080808080ULL;

    uint64_t m_word;
};

using group = group_swar; // the one place SSE2/NEON gets swapped in later

constexpr size_t group_width = group::width;

// A single group load starting at any offset in [0, cap] covers every ring
// position (cap slots + sentinel). A probe never needs a second group, so
// erase can always write EMPTY: tombstones and in-place rehash never occur.
constexpr bool fits_in_one_group(size_t cap) noexcept
{
    return cap < group_width; // cap + 1 <= group_width
}

// ctrl has bytes past the last clone that no slot owns. They are EMPTY from
// reset_ctrl and must stay EMPTY: they are what stops a probe on a full table.
// set_ctrl never touches them, and anything that rewrites clones must not either.
constexpr bool has_phantom_bytes(size_t cap) noexcept
{
    return cap < group_width - 1;
}


} // namespace _hash_priv
} // namespace vx
