#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/aligned_storage.hpp"
#include "vertex/std/bit.hpp" // path is a guess
#include "vertex/std/endian.hpp"
#include "vertex/std/expected.hpp"
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

//=========================================================================
// growth policies
//=========================================================================

// A growth policy decides (a) which capacities exist and (b) how a position is
// folded into the "ctrl ring", which has cap + 1 positions (cap slots plus the
// sentinel). Required interface:
//
//   static constexpr size_t min_capacity;
//       Smallest non-zero capacity.
//
//   static constexpr bool is_valid_capacity(size_t cap);
//       True for 0 (unallocated) and for every capacity the policy can produce.
//
//   static constexpr size_t next_capacity(size_t cap);
//       Next capacity after cap (cap == 0 gives min_capacity).
//       Returns 0 on overflow. Must be strictly increasing.
//
//   static size_t wrap(size_t x, size_t cap);
//       x mod (cap + 1). Must work for cap == 0 (returns 0).
//
//   static size_t wrap_lane(size_t x, size_t cap);
//       Same result as wrap(x, cap), but only required to be correct for
//       x < 2 * (cap + 1). probe_seq uses it to map (group offset + lane) to a
//       ring position, where x <= cap + group_width - 1. It is the hot-path
//       variant: policies with an expensive wrap() (modulo) should implement
//       it as a single conditional subtract. Must work for cap == 0.
//       Position cap is the sentinel; positions past it are clones of slots
//       0.. and wrap back to them (cap + 1 -> 0, cap + 2 -> 1, ...).
//
//   static size_t probe_step(size_t index);
//       How far to advance the group offset after walking `index` ctrl bytes
//       (index is a multiple of group_width, already incremented).
//       Triangular probing returns index, linear probing returns group_width.
//       The probe sequence has to reach every position, so triangular
//       probing needs cap + 1 to be a power of two. Anything else must use
//       linear probing.

// Capacities 2^n - 1 (3, 7, 15, ...), so a capacity doubles as a mask.
// Triangular probing. This is the fast default.
struct pow2_growth
{
    static constexpr size_t min_capacity = 3;

    static constexpr bool is_valid_capacity(size_t cap) noexcept
    {
        return (cap & (cap + 1)) == 0;
    }

    static constexpr size_t next_capacity(size_t cap) noexcept
    {
        if (cap == 0)
        {
            return min_capacity;
        }
        return cap > (SIZE_MAX >> 1) ? 0 : cap * 2 + 1;
    }

    static size_t wrap(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }

    static size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }

    static size_t probe_step(size_t index) noexcept
    {
        return index; // triangular
    }
};

// Capacity grows by Num/Den (default 1.5x), any size is allowed. Linear group
// probing and a modulo to wrap. The modulo is a real division, so this trades
// speed for memory use. Swap wrap() for a fastmod or a prime table if it matters.
template <size_t Num = 3, size_t Den = 2>
struct factor_growth
{
    VX_STATIC_ASSERT_MSG(Num > Den && Den > 0, "growth factor must be > 1");

    static constexpr size_t min_capacity = 7;

    VX_STATIC_ASSERT_MSG(min_capacity + group_width - 1 < 2 * (min_capacity + 1),
        "wrap_lane's single subtract needs cap + group_width - 1 < 2 * (cap + 1) for every capacity");

    static constexpr bool is_valid_capacity(size_t cap) noexcept
    {
        return cap == 0 || cap >= min_capacity;
    }

    // TODO: overflow check on (cap / Den) * (Num - Den) for factors above 2x
    static constexpr size_t next_capacity(size_t cap) noexcept
    {
        if (cap == 0)
        {
            return min_capacity;
        }

        // floor(cap * (Num - Den) / Den) without overflowing the multiply
        size_t extra = (cap / Den) * (Num - Den) + ((cap % Den) * (Num - Den)) / Den;
        if (extra == 0)
        {
            extra = 1; // always make progress
        }
        return cap > SIZE_MAX - extra - group_width ? 0 : cap + extra;
    }

    static size_t wrap(size_t x, size_t cap) noexcept
    {
        return x % (cap + 1);
    }

    static size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        const size_t r = cap + 1;
        return (x >= r) ? x - r : x;
    }

    static size_t probe_step(size_t) noexcept
    {
        return group_width; // linear over groups
    }
};

//=========================================================================
// helpers
//=========================================================================

template <typename...>
struct make_void
{
    using type = void;
};

//=========================================================================
// load policies
//=========================================================================

// A load policy decides how many elements a capacity may hold before a grow.
// Required interface:
//
//   static constexpr size_t max_size(size_t cap);   // max_size(0) == 0
//
// Optional interface (detected, defaults to ~78%):
//
//   static size_t cleanup_limit(size_t max_load);
//       Largest live size at which reclaiming tombstones beats growing.
//       0 = always grow; >= max_load = always clean when it frees anything.

inline size_t default_cleanup_limit(size_t m) noexcept
{
    return (m / 32) * 25 + ((m % 32) * 25) / 32; // remainder term: small tables don't round to 0
}

template <size_t Num, size_t Den>
struct max_load_ratio
{
    VX_STATIC_ASSERT_MSG(Num > 0 && Num < Den, "load factor must be in (0, 1)");

    static constexpr size_t max_size(size_t cap) noexcept
    {
        // cap - floor(cap * (Den - Num) / Den), overflow-safe
        constexpr size_t spare = Den - Num;
        const size_t g = cap - ((cap / Den) * spare + ((cap % Den) * spare) / Den);

        // e.g. cap 7 at 7/8: a completely full 7-slot table has no EMPTY byte
        return (g >= cap && cap >= group_width - 1) ? cap - 1 : g;
    }

    static size_t cleanup_limit(size_t max_load) noexcept
    {
        return default_cleanup_limit(max_load);
    }
};

using load_7_8 = max_load_ratio<7, 8>;

template <typename L, typename = void>
struct has_cleanup_limit : std::false_type
{};

template <typename L>
struct has_cleanup_limit<L, typename make_void<decltype(L::cleanup_limit(size_t{}))>::type> : std::true_type
{};

template <typename L>
size_t cleanup_limit_impl(size_t m, std::true_type) noexcept
{
    return L::cleanup_limit(m);
}

template <typename L>
size_t cleanup_limit_impl(size_t m, std::false_type) noexcept
{
    return default_cleanup_limit(m);
}

// clamped so cleanup is only chosen when it can free at least one slot
template <typename L>
size_t cleanup_limit_of(size_t max_load) noexcept
{
    VX_ASSERT(max_load > 0);
    const size_t limit = cleanup_limit_impl<L>(max_load, has_cleanup_limit<L>{});
    return limit < max_load ? limit : max_load - 1;
}

//=========================================================================
// table traits
//=========================================================================

template <typename Growth = pow2_growth, typename Load = load_7_8, bool CacheHash = false>
struct hash_traits
{
    using growth_policy = Growth;
    using load_policy = Load;
    static constexpr bool cache_hash = CacheHash; // consumed by the slot policy later
};

using default_table_traits = hash_traits<>;

// Smallest capacity (in the growth policy's sequence) that holds n elements
// without growing. Returns 0 for n == 0 and on overflow. Cold path.
template <typename Growth, typename Load>
constexpr size_t capacity_for_size(size_t n) noexcept
{
    if (n == 0)
    {
        return 0;
    }

    size_t cap = Growth::min_capacity;
    while (Load::max_size(cap) < n)
    {
        cap = Growth::next_capacity(cap);
        if (cap == 0)
        {
            return 0;
        }
    }
    return cap;
}

//=========================================================================
// hash helpers
//=========================================================================

template <typename H, typename = void>
struct is_avalanching : std::false_type
{};

template <typename H>
struct is_avalanching<H, typename make_void<typename H::is_avalanching>::type> : std::true_type
{};

// Cheap finalizer: multiply, then fold the high half down. The multiply
// pushes entropy upward, and the xor-shift brings it back to the low bits,
// which H2 and the start position both use.
inline size_t finalize_hash(size_t h) noexcept
{
    VX_IF_CONSTEXPR (sizeof(size_t) == 8)
    {
        h *= static_cast<size_t>(0x9E3779B97F4A7C15ull);
        h ^= h >> 32;
    }
    else
    {
        h *= static_cast<size_t>(0x9E3779B1u);
        h ^= h >> 16;
    }
    return h;
}

inline size_t h1_of(size_t hash) noexcept
{
    return hash >> 7;
}

inline h2_t h2_of(size_t hash) noexcept
{
    return static_cast<h2_t>(hash & 0x7F);
}

//=========================================================================
// layout
//=========================================================================

// One allocation: [ ctrl bytes | padding | slots ]
struct layout
{
    size_t slots_offset;
    size_t alloc_size;
    size_t alloc_align;
};

// Returns false if the total size overflows size_t.
inline bool compute_layout(size_t cap, size_t slot_size, size_t slot_align, layout& out) noexcept
{
    VX_ASSERT(cap != 0);
    VX_ASSERT(bit::has_single_bit(slot_align));

    // ctrl bytes = cap slots + 1 sentinel + (width - 1) clones = cap + group_width.
    // The round-up below adds up to slot_align - 1 more, so check for both
    // before doing either addition.
    if (cap > SIZE_MAX - group_width - (slot_align - 1))
    {
        return false;
    }

    const size_t ctrl_bytes = cap + group_width;
    const size_t offset = (ctrl_bytes + slot_align - 1) & ~(slot_align - 1);

    // offset + cap * slot_size must fit
    if (slot_size != 0 && cap > (SIZE_MAX - offset) / slot_size)
    {
        return false;
    }

    out.slots_offset = offset;
    out.alloc_size = offset + cap * slot_size;
    out.alloc_align = slot_align > group_width ? slot_align : group_width;

    if (out.alloc_size > SIZE_MAX - (out.alloc_align - 1))
    {
        return false;
    }

    // multiple of alloc_align, so the block allocator's unit count is exact
    out.alloc_size = (out.alloc_size + out.alloc_align - 1) & ~(out.alloc_align - 1);

    return true;
}

//=========================================================================
// ctrl array
//=========================================================================

// What an unallocated table points at. capacity == 0, so ctrl[capacity] is
// ctrl[0], which is the sentinel. The other width-1 bytes are EMPTY, so:
//   - a find loads this group, matches nothing, sees an EMPTY, and reports a miss
//   - begin() lands on the sentinel, so begin() == end() with no null check
// It is never written: an unallocated table has growth_left == 0, so any insert
// takes the grow path first. Anything that writes ctrl (clear, erase, rehash)
// must check capacity != 0. Never compare a pointer against it (not DLL-safe),
// test capacity == 0 instead.
// Growth policies must therefore accept cap == 0 (wrap(x, 0) == 0).
inline ctrl_t* empty_group() noexcept
{
    // The initializer below is written for width 8. Any other width would
    // zero-fill the tail, and 0x00 is a FULL byte with h2 == 0, so a lookup on
    // an empty table would call eq() on a null slot array.
    VX_STATIC_ASSERT_MSG(group_width == 8, "empty_group() initializer assumes group_width == 8");

    // clang-format off
    alignas(group_width) static const ctrl_t group_bytes[group_width] = {
        ctrl_sentinel,
        ctrl_empty, ctrl_empty, ctrl_empty, ctrl_empty,
        ctrl_empty, ctrl_empty, ctrl_empty
    };
    // clang-format on
    return const_cast<ctrl_t*>(group_bytes);
}

// ctrl layout: [ cap slots | sentinel | min(cap, W-1) clones | phantoms ]
// For cap >= W - 1 there are no phantoms. For smaller caps the remaining bytes
// stay EMPTY forever (see has_phantom_bytes). Nothing may write them.
inline void reset_ctrl(ctrl_t* ctrl, size_t cap) noexcept
{
    VX_ASSERT(cap != 0);
    mem::set(ctrl, static_cast<int>(ctrl_empty), cap + group_width);
    ctrl[cap] = ctrl_sentinel;
}

// Re-establish the sentinel and clones after bulk-rewriting ctrl[0..cap).
inline void restore_ctrl_clones(ctrl_t* ctrl, size_t cap) noexcept
{
    VX_ASSERT(cap != 0);
    constexpr size_t max_clones = group_width - 1;
    const size_t n = cap < max_clones ? cap : max_clones;

    ctrl[cap] = ctrl_sentinel;
    for (size_t i = 0; i < n; ++i)
    {
        ctrl[cap + 1 + i] = ctrl[i];
    }
}

// Writes ctrl[i] and its clone, if it has one. Every ctrl write after reset_ctrl
// goes through this (insert writes H2, erase writes EMPTY or DELETED).
// Works for any capacity: the clone of slot i (i < width - 1) lives at cap + 1 + i.
inline void set_ctrl(ctrl_t* ctrl, size_t cap, size_t i, ctrl_t value) noexcept
{
    VX_ASSERT(i < cap);

    constexpr size_t cloned = group_width - 1;
    // For i >= cloned the "mirror" is just i again (harmless, branch-free cmov).
    const size_t mirror = i < cloned ? cap + 1 + i : i;

    ctrl[i] = value;
    ctrl[mirror] = value;
}

//=========================================================================
// probing
//=========================================================================

template <typename Growth>
class probe_seq
{
public:

    probe_seq(size_t h1, size_t cap) noexcept
        : m_cap(cap)
        , m_offset(Growth::wrap(h1, cap))
        , m_index(0)
    {
        VX_ASSERT(Growth::is_valid_capacity(cap));
    }

    // Start of the current group, in [0, cap]. Note it can equal cap, where
    // the group starts on the sentinel. That is fine: lanes are mapped to
    // slots by slot_at() below and the sentinel never matches anything.
    size_t offset() const noexcept
    {
        return m_offset;
    }

    // Slot index for lane `lane` of the current group.
    size_t slot_at(size_t lane) const noexcept
    {
        VX_ASSERT(m_offset + lane < 2 * (m_cap + 1));
        return Growth::wrap_lane(m_offset + lane, m_cap);
    }

    // Number of ctrl bytes walked past so far (a multiple of W).
    size_t index() const noexcept
    {
        return m_index;
    }

    void next() noexcept
    {
        // Stepping a second time on a table smaller than one group, or past
        // cap+1 bytes on a larger one, means a group with no EMPTY byte was
        // found, which the load factor rules out. It is a table bug.
        VX_ASSERT(m_index <= m_cap);

        m_index += group_width;
        m_offset = Growth::wrap(m_offset + Growth::probe_step(m_index), m_cap);
    }

private:

    size_t m_cap;
    size_t m_offset;
    size_t m_index;
};

// First EMPTY or DELETED slot on the probe path. Precondition: the table has
// room (growth_left > 0, or a tombstone exists), so one is guaranteed.
// Free function so rehash can run it on a block that is not installed yet.
template <typename Growth>
size_t find_first_non_full(const ctrl_t* ctrl, size_t cap, size_t hash) noexcept
{
    probe_seq<Growth> seq(h1_of(hash), cap);
    while (true)
    {
        const group g(ctrl + seq.offset());
        const auto mask = g.match_empty_or_deleted();
        if (mask)
        {
            return seq.slot_at(mask.lowest());
        }
        seq.next();
    }
}

// start of the group that ends at slot i - 1 (erase's "was it ever surrounded" check)
template <typename Growth>
size_t group_before(size_t i, size_t cap) noexcept
{
    return Growth::wrap_lane(i + (cap + 1) - group_width, cap);
}

//=========================================================================
// slot policies
//=========================================================================

// A slot policy is the only thing that touches slot memory. Required interface:
//
//   key_type, slot_type
//   static const key_type& key(const slot_type&)
//   static void construct(slot_type*, Args&&...)
//   static void destroy(slot_type*) noexcept
//   static void relocate(slot_type* dst, slot_type* src)   // move-construct, then destroy src
//   static constexpr bool mutable_slots                    // iterators hand out slot_type& (maps) or const (sets)
//   static constexpr bool trivially_relocatable

template <typename K>
struct set_policy
{
    using key_type = K;
    using slot_type = K;

    static constexpr bool mutable_slots = false;

    // true => rehash may memcpy slots and skip destroy
    static constexpr bool trivially_relocatable = std::is_trivially_copyable<K>::value;

    // How the table sees a key inside a slot.
    static const K& key(const slot_type& s) noexcept
    {
        return s;
    }

    template <typename... Args>
    static void construct(slot_type* p, Args&&... args)
    {
        mem::construct_in_place(p, std::forward<Args>(args)...);
    }

    static void destroy(slot_type* p) noexcept
    {
        mem::destroy_in_place(p);
    }

    // Move src into raw memory at dst, then destroy src. Used by rehash.
    static void relocate(slot_type* dst, slot_type* src)
    {
        construct(dst, std::move(*src));
        destroy(src);
    }
};

//=========================================================================
// iterators
//=========================================================================

// Backend iterator: a (ctrl*, slot*) pair that advances in lockstep.
// Slot is slot_type or const slot_type.
template <typename Slot>
class table_iterator
{
    template <typename>
    friend class table_iterator;

public:

    using iterator_category = std::forward_iterator_tag; // swap for your own tag type
    using value_type = typename std::remove_const<Slot>::type;
    using difference_type = ptrdiff_t;
    using pointer = Slot*;
    using reference = Slot&;

    table_iterator() noexcept = default;

    table_iterator(ctrl_t* ctrl, Slot* slot) noexcept
        : m_ctrl(ctrl)
        , m_slot(slot)
    {}

    // iterator -> const_iterator
    template <typename S2, VX_REQUIRES(std::is_same<const S2, Slot>::value && !std::is_same<S2, Slot>::value)>
    table_iterator(const table_iterator<S2>& o) noexcept
        : m_ctrl(o.m_ctrl)
        , m_slot(o.m_slot)
    {}

    reference operator*() const noexcept
    {
        VX_ASSERT(is_full(*m_ctrl));
        return *m_slot;
    }

    pointer operator->() const noexcept
    {
        return m_slot;
    }

    table_iterator& operator++() noexcept
    {
        VX_ASSERT(*m_ctrl != ctrl_sentinel); // cannot advance end()
        ++m_ctrl;
        ++m_slot;
        skip_empty_or_deleted();
        return *this;
    }

    table_iterator operator++(int) noexcept
    {
        table_iterator it = *this;
        ++*this;
        return it;
    }

    // Moves forward until the current byte is FULL or the SENTINEL. Each step
    // jumps over a whole run of EMPTY/DELETED lanes. Used by begin().
    void skip_empty_or_deleted() noexcept
    {
        while (is_empty_or_deleted(*m_ctrl))
        {
            // all lanes empty/deleted gives trailing_zeros == width, so we skip a
            // whole group, and the sentinel can never be jumped over
            const uint32_t n = group(m_ctrl).match_full_or_sentinel().trailing_zeros();
            m_ctrl += n;
            m_slot += n;
        }
    }

    ctrl_t* ctrl() const noexcept
    {
        return m_ctrl;
    }

    friend bool operator==(const table_iterator& a, const table_iterator& b) noexcept
    {
        return a.m_ctrl == b.m_ctrl;
    }

    friend bool operator!=(const table_iterator& a, const table_iterator& b) noexcept
    {
        return a.m_ctrl != b.m_ctrl;
    }

private:

    ctrl_t* m_ctrl = nullptr;
    Slot* m_slot = nullptr;
};

//=========================================================================
// table data
//=========================================================================

template <typename SlotType>
struct table_data_base
{
    using slot_type = SlotType;

    ctrl_t* ctrl = empty_group(); // never null
    slot_type* slots = nullptr;   // valid only when capacity != 0
    size_t capacity = 0;
    size_t size = 0;
    size_t growth_left = 0; // load::max_size(capacity) - size - tombstones

    void reset() noexcept
    {
        ctrl = empty_group();
        slots = nullptr;
        capacity = 0;
        size = 0;
        growth_left = 0;
    }

    // same role as vector's data_type::acquire: take other's block, leave it empty
    void acquire(table_data_base& other) noexcept
    {
        *this = other;
        other.reset();
    }
};

//=========================================================================
// hash_table (open addressing backend)
//=========================================================================

template <typename SlotPolicy, typename Hasher, typename Eq, typename Allocator, typename Traits = default_table_traits>
class hash_table
{
    //=========================================================================
    // member types
    //=========================================================================

private:

    using policy = SlotPolicy;
    using slot_type = typename policy::slot_type;
    using growth = typename Traits::growth_policy;
    using load = typename Traits::load_policy;
    using table_data = table_data_base<slot_type>;

    VX_STATIC_ASSERT_MSG(growth::is_valid_capacity(0), "growth policy must accept capacity 0");
    VX_STATIC_ASSERT_MSG(growth::is_valid_capacity(growth::min_capacity), "min_capacity must be valid");
    VX_STATIC_ASSERT_MSG(growth::next_capacity(0) == growth::min_capacity, "next_capacity(0) must be min_capacity");
    VX_STATIC_ASSERT_MSG(growth::next_capacity(growth::min_capacity) > growth::min_capacity, "next_capacity must increase");
    VX_STATIC_ASSERT_MSG(load::max_size(0) == 0, "max_size(0) must be 0");
    VX_STATIC_ASSERT_MSG(load::max_size(growth::min_capacity) >= 1, "load policy leaves no room in min_capacity");
    VX_STATIC_ASSERT_MSG(std::is_same<decltype(Traits::cache_hash), const bool>::value, "Traits::cache_hash must be a bool");

public:

    using allocator_type = Allocator;
    using hasher_type = Hasher;
    using eq_type = Eq;
    using key_type = typename policy::key_type;
    using value_type = slot_type;
    using size_type = size_t;

    // sets expose const slots, maps expose mutable ones (policy::mutable_slots)
    using iterator = table_iterator<typename std::conditional<policy::mutable_slots, slot_type, const slot_type>::type>;
    using const_iterator = table_iterator<const slot_type>;

    static constexpr size_t npos = static_cast<size_t>(-1);

private:

    // Size == Alignment, so sizeof(block_type) == block_align and N units are
    // exactly N * block_align bytes at block_align alignment.
    static constexpr size_t block_align = alignof(slot_type) > group_width ? alignof(slot_type) : group_width;
    using block_type = aligned_storage<block_align, block_align>;
    using block_allocator = typename allocator_type::template rebind<block_type>::other; // or allocator_traits::rebind_alloc

    // Above this slot size the temp slot in rehash_in_place is too large for the
    // stack, so cleanup rebuilds into a fresh block instead.
    static constexpr size_t max_in_place_slot_size = 256;

    //=========================================================================
    // storage
    //=========================================================================

    // allocator -> hasher -> eq -> data. Each `first` is empty-base-optimized,
    // so stateless members take no space.
    using eq_storage = _priv::compressed_pair<Eq, table_data>;
    using hasher_storage = _priv::compressed_pair<Hasher, eq_storage>;
    _priv::compressed_pair<allocator_type, hasher_storage> m_storage;

    allocator_type& alloc() noexcept
    {
        return m_storage.first();
    }

    const allocator_type& alloc() const noexcept
    {
        return m_storage.first();
    }

    Hasher& hasher() noexcept
    {
        return m_storage.second.first();
    }

    const Hasher& hasher() const noexcept
    {
        return m_storage.second.first();
    }

    Eq& eq() noexcept
    {
        return m_storage.second.second.first();
    }

    const Eq& eq() const noexcept
    {
        return m_storage.second.second.first();
    }

    table_data& m_data() noexcept
    {
        return m_storage.second.second.second;
    }

    const table_data& m_data() const noexcept
    {
        return m_storage.second.second.second;
    }

    //=========================================================================
    // memory block helpers
    //=========================================================================

    unsigned char* allocate_block(size_t bytes) noexcept
    {
        return reinterpret_cast<unsigned char*>(block_allocator(alloc()).allocate(bytes / block_align));
    }

    void release_block(size_t cap) noexcept
    {
        layout l;
        const bool ok = compute_layout(cap, sizeof(slot_type), alignof(slot_type), l);
        VX_ASSERT(ok);
        block_allocator(alloc()).deallocate(reinterpret_cast<block_type*>(m_data().ctrl), l.alloc_size / block_align);
    }

    //=========================================================================
    // destruction helpers
    //=========================================================================

    void destroy_slots() noexcept
    {
        auto& d = m_data();
        if (d.capacity == 0 || std::is_trivially_destructible<slot_type>::value)
        {
            return;
        }

        for (size_t i = 0; i < d.capacity; ++i)
        {
            if (is_full(d.ctrl[i]))
            {
                policy::destroy(d.slots + i);
            }
        }
    }

    void destroy_and_release() noexcept
    {
        destroy_slots();
        if (m_data().capacity != 0)
        {
            release_block(m_data().capacity);
        }
    }

    //=========================================================================
    // copy helpers
    //=========================================================================

    // Copies ctrl bytes verbatim (sentinel, clones, phantoms, and tombstones), then
    // copy-constructs each full slot at the same index. No hashing, no probing.
    // Requires *this to be unallocated.
    success copy_construct_from(const table_data& src)
    {
        auto& d = m_data();
        VX_ASSERT(d.capacity == 0);

        if (src.capacity == 0)
        {
            return success{};
        }

        layout l;
        VX_RET_ERR_IF(!compute_layout(src.capacity, sizeof(slot_type), alignof(slot_type), l), err::size_error);

        unsigned char* block = allocate_block(l.alloc_size);
        VX_RET_ERR_IF(!block, err::out_of_memory);

        ctrl_t* ctrl = reinterpret_cast<ctrl_t*>(block);
        slot_type* slots = reinterpret_cast<slot_type*>(block + l.slots_offset);

        mem::copy(ctrl, src.ctrl, src.capacity + group_width);

        for (size_t i = 0; i < src.capacity; ++i)
        {
            if (is_full(ctrl[i]))
            {
                policy::construct(slots + i, src.slots[i]);
            }
        }

        d.ctrl = ctrl;
        d.slots = slots;
        d.capacity = src.capacity;
        d.size = src.size;
        d.growth_left = src.growth_left;
        return success{};
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    hash_table() noexcept(noexcept(allocator_type()))
        : m_storage(_priv::zero_then_variadic_args_tag{},
              _priv::zero_then_variadic_args_tag{},
              _priv::zero_then_variadic_args_tag{})
    {}

    // clang-format off

    explicit hash_table(const Hasher& h, const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : m_storage(
            _priv::one_then_variadic_args_tag{}, a,
            _priv::one_then_variadic_args_tag{}, h,
            _priv::one_then_variadic_args_tag{}, e)
    {}

    // size-reserving constructor, VX_VERIFY like vector
    explicit hash_table(size_t n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
        : hash_table(h, e, a)
    {
        const auto ok = reserve(n);
        VX_VERIFY(ok);
    }

    hash_table(const hash_table& other)
        : m_storage(
            _priv::one_then_variadic_args_tag{}, other.alloc(),
            _priv::one_then_variadic_args_tag{}, other.hasher(),
            _priv::one_then_variadic_args_tag{}, other.eq())
    {
        const auto ok = copy_construct_from(other.m_data());
        VX_VERIFY(ok);
    }

    // like vector: the allocator moves along with the block that it produced
    hash_table(hash_table&& other) noexcept
        : m_storage(
            _priv::one_then_variadic_args_tag{}, std::move(other.alloc()),
            _priv::one_then_variadic_args_tag{}, std::move(other.hasher()),
            _priv::one_then_variadic_args_tag{}, std::move(other.eq()))
    {
        m_data().acquire(other.m_data());
    }

    // clang-format on

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<hash_table, error> create(const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
    {
        return hash_table(h, e, a);
    }

    static expected<hash_table, error> create(size_t n, const Hasher& h = Hasher(), const Eq& e = Eq(), const allocator_type& a = allocator_type())
    {
        hash_table t(h, e, a);
        const auto ok = t.reserve(n);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    static expected<hash_table, error> create(const hash_table& other)
    {
        hash_table t(other.hasher(), other.eq(), other.alloc());
        const auto ok = t.copy_construct_from(other.m_data());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return t;
    }

    //=========================================================================
    // destructor
    //=========================================================================

    ~hash_table()
    {
        destroy_and_release();
    }

    //=========================================================================
    // observers
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return alloc();
    }

    Hasher hash_function() const
    {
        return hasher();
    }

    Eq key_eq() const
    {
        return eq();
    }

    //=========================================================================
    // assignment
    //=========================================================================

    hash_table& operator=(const hash_table& other)
    {
        if (this != &other)
        {
            const auto ok = assign(other);
            VX_VERIFY(ok);
        }
        return *this;
    }

    // like vector: takes over the source's allocator along with its block
    hash_table& operator=(hash_table&& other) noexcept
    {
        if (this != &other)
        {
            destroy_and_release();
            alloc() = std::move(other.alloc());
            hasher() = std::move(other.hasher());
            eq() = std::move(other.eq());
            m_data().acquire(other.m_data());
        }
        return *this;
    }

    // Strong guarantee: build the copy first, then swap. Keeps this table's allocator.
    success assign(const hash_table& other)
    {
        VX_ASSERT(this != &other);

        hash_table tmp(other.hasher(), other.eq(), alloc());
        const auto ok = tmp.copy_construct_from(other.m_data());
        if (!ok)
        {
            return ok;
        }

        swap(tmp);
        return success{};
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator iterator_at(size_t i) noexcept
    {
        return iterator(m_data().ctrl + i, m_data().slots + i);
    }

    const_iterator iterator_at(size_t i) const noexcept
    {
        return const_iterator(m_data().ctrl + i, m_data().slots + i);
    }

    size_t index_of(const_iterator it) const noexcept
    {
        return static_cast<size_t>(it.ctrl() - m_data().ctrl);
    }

    iterator begin() noexcept
    {
        iterator it = iterator_at(0);
        it.skip_empty_or_deleted();
        return it;
    }

    const_iterator begin() const noexcept
    {
        const_iterator it = iterator_at(0);
        it.skip_empty_or_deleted();
        return it;
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator_at(m_data().capacity);
    }

    const_iterator end() const noexcept
    {
        return iterator_at(m_data().capacity);
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_data().size == 0;
    }

    size_t size() const noexcept
    {
        return m_data().size;
    }

    // Conservative: rehash_to still reports size_error on layout overflow.
    static constexpr size_t max_size() noexcept
    {
        return (SIZE_MAX / 2) / (sizeof(slot_type) + 1);
    }

    //=========================================================================
    // capacity
    //=========================================================================

    // number of elements the table can hold before it must grow
    size_t capacity() const noexcept
    {
        return load::max_size(m_data().capacity);
    }

    // raw ring size (slots), 0 or a capacity produced by the growth policy
    size_t slot_count() const noexcept
    {
        return m_data().capacity;
    }

    // Capacity needed to hold n elements without growing (0 on n == 0 or overflow).
    static constexpr size_t capacity_for_size(size_t n) noexcept
    {
        return _hash_priv::capacity_for_size<growth, load>(n);
    }

    // Tombstones are the budget that erase left behind. Derived, so no extra field.
    size_t tombstone_count() const noexcept
    {
        const auto& d = m_data();
        return load::max_size(d.capacity) - d.size - d.growth_left;
    }

    // true if the next budget exhaustion would clean in place instead of growing
    bool needs_cleanup() const noexcept
    {
        const auto& d = m_data();
        return d.capacity != 0 && tombstone_count() != 0 && d.size <= cleanup_limit_of<load>(load::max_size(d.capacity));
    }

    //=========================================================================
    // hashing
    //=========================================================================

    // Hash plus finalizer. Hashers declaring is_avalanching are trusted as-is.
    template <typename Q>
    size_t hash_of(const Q& key) const noexcept
    {
        const size_t h = static_cast<size_t>(hasher()(key));

        VX_IF_CONSTEXPR (is_avalanching<Hasher>::value)
        {
            return h;
        }
        else
        {
            return finalize_hash(h);
        }
    }

    //=========================================================================
    // lookup
    //=========================================================================

    // Returns the slot index of the key, or npos.
    template <typename Q>
    size_t find_index_with_hash(const Q& key, size_t hash) const noexcept
    {
        const h2_t h2 = h2_of(hash);
        probe_seq<growth> seq(h1_of(hash), m_data().capacity);

        while (true)
        {
            const group g(m_data().ctrl + seq.offset());

            // candidates: lanes whose ctrl byte equals H2
            for (uint32_t lane : g.match(h2))
            {
                const size_t i = seq.slot_at(lane);
                if (eq()(key, policy::key(m_data().slots[i])))
                {
                    return i;
                }
            }

            // an EMPTY lane means no probe path continues past this group
            if (g.match_empty())
            {
                return npos;
            }

            seq.next();
        }
    }

    template <typename Q>
    size_t find_index(const Q& key) const noexcept
    {
        return find_index_with_hash(key, hash_of(key));
    }

    template <typename Q>
    iterator find_with_hash(const Q& key, size_t hash) noexcept
    {
        VX_ASSERT(hash == hash_of(key)); // debug: a precomputed hash must agree with the hasher
        const size_t i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, size_t hash) const noexcept
    {
        VX_ASSERT(hash == hash_of(key));
        const size_t i = find_index_with_hash(key, hash);
        return i == npos ? end() : iterator_at(i);
    }

    template <typename Q>
    iterator find(const Q& key) noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q>
    const_iterator find(const Q& key) const noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q>
    bool contains(const Q& key) const noexcept
    {
        return find_index(key) != npos;
    }

    template <typename Q>
    size_t count(const Q& key) const noexcept
    {
        return contains(key) ? 1 : 0;
    }

private:

    //=========================================================================
    // rehash
    //=========================================================================

    // Capacity to grow into so that `needed` elements fit. The growth policy picks
    // the sequence, so this can step more than once (reserve, tiny load factors).
    // Always steps at least once. Returns 0 on overflow. After allocating:
    //     m_data().growth_left = load::max_size(new_cap) - m_data().size;
    size_t grown_capacity(size_t needed) const noexcept
    {
        size_t cap = growth::next_capacity(m_data().capacity);
        while (cap != 0 && load::max_size(cap) < needed)
        {
            cap = growth::next_capacity(cap);
        }
        return cap;
    }

    // Allocates a block of new_cap slots, relocates every element into it by
    // hash, then frees the old block. The old block is untouched on failure.
    // Serves growth, reserve and shrink_to_fit.
    success rehash_to(size_t new_cap)
    {
        layout l;
        VX_RET_ERR_IF(!compute_layout(new_cap, sizeof(slot_type), alignof(slot_type), l), err::size_error);

        unsigned char* block = allocate_block(l.alloc_size);
        VX_RET_ERR_IF(!block, err::out_of_memory);

        ctrl_t* new_ctrl = reinterpret_cast<ctrl_t*>(block);
        slot_type* new_slots = reinterpret_cast<slot_type*>(block + l.slots_offset);
        reset_ctrl(new_ctrl, new_cap);

        auto& d = m_data();

        if (d.capacity != 0)
        {
            for (size_t i = 0; i < d.capacity; ++i)
            {
                if (!is_full(d.ctrl[i]))
                {
                    continue;
                }

                // keys are unique: first free slot, no match(h2), no Eq
                const size_t hash = hash_of(policy::key(d.slots[i]));
                const size_t t = find_first_non_full<growth>(new_ctrl, new_cap, hash);
                set_ctrl(new_ctrl, new_cap, t, static_cast<ctrl_t>(h2_of(hash)));
                policy::relocate(new_slots + t, d.slots + i);
            }
            release_block(d.capacity);
        }

        d.ctrl = new_ctrl;
        d.slots = new_slots;
        d.capacity = new_cap;
        d.growth_left = load::max_size(new_cap) - d.size;
        return success{};
    }

    // Re-places every element at the first free slot on its probe path without
    // allocating. Elements in their first probe window stay put. Others move into
    // EMPTY slots, or swap with another pending element (one temp slot).
    void rehash_in_place() noexcept
    {
        auto& d = m_data();
        const size_t cap = d.capacity;
        ctrl_t* ctrl = d.ctrl;
        slot_type* slots = d.slots;
        VX_ASSERT(cap != 0 && !fits_in_one_group(cap));

        // 1. FULL -> DELETED ("pending"), everything else -> EMPTY. Groups may
        //    write past the ring into the clone bytes, which the next call rewrites.
        for (size_t pos = 0; pos < cap + 1; pos += group_width)
        {
            group(ctrl + pos).convert_special_to_empty_and_full_to_deleted(ctrl + pos);
        }
        restore_ctrl_clones(ctrl, cap); // sentinel + clones, before any probing

        // 2. Re-place each pending element.
        aligned_storage<sizeof(slot_type), alignof(slot_type)> tmp;

        for (size_t i = 0; i < cap; ++i)
        {
            while (ctrl[i] == ctrl_deleted)
            {
                const size_t hash = hash_of(policy::key(slots[i]));
                const h2_t h2 = h2_of(hash);
                const size_t offset = growth::wrap(h1_of(hash), cap);

                // In the first probe window: every lookup examines it, so the
                // element is already findable. Just mark it full.
                if (growth::wrap_lane(i + (cap + 1) - offset, cap) < group_width)
                {
                    set_ctrl(ctrl, cap, i, static_cast<ctrl_t>(h2));
                    break;
                }

                const size_t t = find_first_non_full<growth>(ctrl, cap, hash);

                if (t == i)
                {
                    set_ctrl(ctrl, cap, i, static_cast<ctrl_t>(h2));
                    break;
                }

                const bool was_empty = (ctrl[t] == ctrl_empty);
                set_ctrl(ctrl, cap, t, static_cast<ctrl_t>(h2));

                if (was_empty)
                {
                    policy::relocate(slots + t, slots + i);
                    set_ctrl(ctrl, cap, i, ctrl_empty);
                    break;
                }

                // t holds another pending element: swap through tmp, then retry
                // slot i, which now holds that displaced element.
                slot_type* s = tmp.template ptr<slot_type>();
                policy::relocate(s, slots + i);
                policy::relocate(slots + i, slots + t);
                policy::relocate(slots + t, s);
            }
        }

        d.growth_left = load::max_size(cap) - d.size;
    }

    success cleanup_tombstones_impl(std::true_type) noexcept
    {
        rehash_in_place();
        return success{};
    }

    success cleanup_tombstones_impl(std::false_type)
    {
        return rehash_to(m_data().capacity);
    }

    // tag dispatch, so the untaken overload is never instantiated
    success cleanup_tombstones()
    {
        return cleanup_tombstones_impl(std::integral_constant<bool, (sizeof(slot_type) <= max_in_place_slot_size)>{});
    }

    // Called when an insert finds growth_left == 0: clean in place if tombstones
    // dominate, otherwise grow.
    success grow_for_insert()
    {
        auto& d = m_data();

        if (d.capacity != 0 && d.size <= cleanup_limit_of<load>(load::max_size(d.capacity)))
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grown_capacity(d.size + 1);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return rehash_to(new_cap);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    // destroys elements, keeps the allocation
    void clear() noexcept
    {
        auto& d = m_data();
        if (d.capacity == 0)
        {
            return;
        }

        destroy_slots();
        reset_ctrl(d.ctrl, d.capacity);
        d.size = 0;
        d.growth_left = load::max_size(d.capacity);
    }

    void clear_and_deallocate() noexcept
    {
        destroy_and_release();
        m_data().reset();
    }

    // Ensures n elements can be inserted with no allocation (and no cleanup).
    success reserve(size_t n)
    {
        auto& d = m_data();

        // size + growth_left is the budget that is actually spendable. It is lower
        // than max_size(capacity) when tombstones are using some of it.
        if (n <= d.size + d.growth_left)
        {
            return success{};
        }

        VX_RET_ERR_IF(n > max_size(), err::size_error);

        // the capacity is big enough, tombstones are the problem: reclaim them
        if (d.capacity != 0 && n <= load::max_size(d.capacity))
        {
            return cleanup_tombstones();
        }

        const size_t new_cap = grown_capacity(n);
        VX_RET_ERR_IF(new_cap == 0, err::size_error);
        return rehash_to(new_cap);
    }

    success shrink_to_fit()
    {
        auto& d = m_data();

        if (d.size == 0)
        {
            clear_and_deallocate();
            return success{};
        }

        const size_t target = capacity_for_size(d.size);
        VX_RET_ERR_IF(target == 0, err::size_error);

        return target < d.capacity ? rehash_to(target) : success{};
    }

    // Explicit tombstone cleanup (decision 8's compact()).
    success compact()
    {
        return tombstone_count() == 0 ? success{} : cleanup_tombstones();
    }

    void swap(hash_table& other) noexcept
    {
        vx::swap(m_storage, other.m_storage); // swaps allocator, hasher, eq and data together
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    // Insert: prepare (find slot, grow if needed), construct, commit.
    expected<size_t, error> prepare_insert(size_t hash)
    {
        auto& d = m_data();
        size_t i = find_first_non_full<growth>(d.ctrl, d.capacity, hash);

        // != DELETED, not == EMPTY: on the singleton and on a full tiny table the
        // probe returns a position that is not free, and that must also grow.
        if (d.growth_left == 0 && d.ctrl[i] != ctrl_deleted)
        {
            const auto ok = grow_for_insert();
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
            i = find_first_non_full<growth>(d.ctrl, d.capacity, hash);
        }
        return i;
    }

    size_t prepare_insert_capacity(size_t hash) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(d.growth_left > 0);
        return find_first_non_full<growth>(d.ctrl, d.capacity, hash);
    }

    // Commits slot i. The element must already be constructed there.
    void insert_index(size_t i, size_t hash) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.capacity && !is_full(d.ctrl[i]));
        d.growth_left -= (d.ctrl[i] == ctrl_empty); // only EMPTY spends budget
        set_ctrl(d.ctrl, d.capacity, i, static_cast<ctrl_t>(h2_of(hash)));
        ++d.size;
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    // Key known absent (the front end did the find). Args must not alias an
    // element of this table: growing invalidates them before construction.
    template <typename... Args>
    expected<iterator, error> emplace_new(size_t hash, Args&&... args)
    {
        const auto r = prepare_insert(hash);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());

        const size_t i = r.value();
        policy::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

    // requires growth_left > 0 (or a tombstone on the probe path)
    template <typename... Args>
    iterator emplace_new_capacity(size_t hash, Args&&... args)
    {
        const size_t i = prepare_insert_capacity(hash);
        policy::construct(m_data().slots + i, std::forward<Args>(args)...);
        insert_index(i, hash);
        return iterator_at(i);
    }

private:

    //=========================================================================
    // erase helpers
    //=========================================================================

    void erase_index(size_t i) noexcept
    {
        auto& d = m_data();
        VX_ASSERT(i < d.capacity && is_full(d.ctrl[i]));
        policy::destroy(d.slots + i);

        // Could any probe window of W bytes have passed over slot i while it was
        // full? Measure the run of non-EMPTY bytes around i. If it is shorter than
        // W, no window fit inside it, so EMPTY is safe. Otherwise leave a tombstone.
        const auto after = group(d.ctrl + i).match_empty();
        const auto before = group(d.ctrl + group_before<growth>(i, d.capacity)).match_empty();
        const bool never_full = before && after && (after.trailing_zeros() + before.leading_zeros()) < group_width;

        set_ctrl(d.ctrl, d.capacity, i, never_full ? ctrl_empty : ctrl_deleted);
        d.growth_left += never_full;
        --d.size;
    }

public:

    //=========================================================================
    // erase
    //=========================================================================

    // Leaves the iterator's position non-full, so a loop can ++it afterwards. No other
    // element moves, so erase_if can be: if (pred(*it)) erase_at(it); ++it;
    void erase_at(const_iterator it) noexcept
    {
        erase_index(index_of(it));
    }
};

//=========================================================================
// swap
//=========================================================================

template <typename P, typename H, typename E, typename A, typename T>
void swap(hash_table<P, H, E, A, T>& a, hash_table<P, H, E, A, T>& b) noexcept
{
    a.swap(b);
}

} // namespace _hash_priv
} // namespace vx
