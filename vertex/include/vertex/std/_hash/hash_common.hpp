#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/hash.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace _hash_priv {

// common size type
using hash_size_t = std::size_t;

//=========================================================================
// small constexpr helpers
//=========================================================================

// Takes by value on purpose: std::max/vx::max take const&, which would
// ODR-use static constexpr members (e.g. min_capacity) and, before C++17,
// require an out-of-class definition or you get a link error in debug builds.
constexpr hash_size_t max_of(hash_size_t a, hash_size_t b) noexcept
{
    return a < b ? b : a;
}

// true for 0, 1, 3, 7, 15, ... (2^k - 1), i.e. values usable as an index mask
constexpr bool is_mask(hash_size_t x) noexcept
{
    return (x & (x + 1)) == 0;
}

//=========================================================================
// sizing policies
//=========================================================================

// Vocabulary (use these words everywhere, in code and comments)
// -------------------------------------------------------------
//   cap       2^k - 1, or 0 when unallocated. Also the index mask.
//   ring      the cap + 1 positions [0, cap] that a hash can map to.
//   flat:     positions [0, cap) are slots and ctrl[cap] is the sentinel,
//             so a flat table has `cap` slots. A probe may start on the
//             sentinel; lanes wrap back onto slots through the mask.
//   chain:    positions [0, cap] are buckets, so a chain table has
//             `cap + 1` buckets (END lives one past them, at index cap + 1).
//   max_load  the most elements a table of this cap may hold.
//
// A Sizing policy must provide:
//
//   static constexpr bool         pow2_capacity;       // wrap() is a plain mask
//   static constexpr hash_size_t  min_capacity;        // 2^k - 1 with max_load >= 1
//   static constexpr hash_size_t  max_capacity;        // 2^k - 1; cap + 1 and
//                                                      // cap + group_width must not overflow
//   static constexpr hash_size_t  max_load(cap);       // max_load(0) == 0
//   static constexpr hash_size_t  wrap(hash, cap);     // hash -> ring position [0, cap]
//   static constexpr hash_size_t  next_capacity(current, required);
//        // Smallest valid cap that is > current, >= min_capacity and has
//        // max_load(cap) >= required. Returns 0 if not representable.
//        // `current` must be 0 or a valid cap. `required == 0` counts as 1.
//
// Flat tables additionally need max_load(cap) < cap for every cap > 0: at
// least one EMPTY byte must always exist, or probing for an absent key never
// terminates. Chain tables have no such limit.
//
// The policy is split into two independent knobs:
//   - WHEN to grow:   a Load struct, one function, max_load(cap).
//   - BY HOW MUCH:    pow2_sizing_t's GrowShift (1 = x2, 2 = x4, ...).
// next_capacity is derived from max_load, so a custom Load never needs to
// supply an inverse function.
//
// (The constexpr functions below use loops and locals, so they need C++14.)

//-------------------------------------------------------------------------
// load policies: when to grow
//-------------------------------------------------------------------------

// Flat default: cap - max(cap / 8, 1). Approaches 7/8 occupancy as the table
// grows and always keeps at least one EMPTY slot:
//
//   cap  | max_load | EMPTY slots left | load at max_load
//   -----+----------+------------------+-----------------
//     1  |     0    |        1         | unusable (below min_capacity)
//     3  |     2    |        1         | 67%
//     7  |     6    |        1         | 86%
//    15  |    14    |        1         | 93%
//    31  |    28    |        3         | 90%
//    63  |    56    |        7         | 89%
//   127  |   112    |       15         | 88%
//
// The max(cap / 8, 1) clamp is what makes the tiny sizes work: with a plain
// `cap - cap / 8`, caps 1, 3 and 7 would have max_load == cap, i.e. no EMPTY
// slot at all. Tables smaller than one group also rely on this guarantee:
// their group loads run past the clones into padding bytes that are always
// EMPTY, and a real EMPTY slot is always found before them.
struct load_7_8
{
    static constexpr hash_size_t max_load(hash_size_t cap) noexcept
    {
        return cap == 0 ? 0 : cap - max_of(cap / 8, 1);
    }
};

// Chain default: one element per bucket on average (cap + 1 buckets).
struct load_1_0
{
    static constexpr hash_size_t max_load(hash_size_t cap) noexcept
    {
        return cap == 0 ? 0 : cap + 1;
    }
};

//-------------------------------------------------------------------------
// power-of-two sizing: by how much
//-------------------------------------------------------------------------

// Load      a load policy (see above)
// MinCap    smallest cap ever allocated; must be 2^k - 1 with max_load > 0.
//           We overallocate a little on the first allocations so early
//           rehashes are rarer. Raise to 7 or 15 to skip more of them.
// GrowShift log2 of the growth factor: 1 doubles, 2 quadruples.
template <typename Load, hash_size_t MinCap = 3, unsigned GrowShift = 1>
struct pow2_sizing_t
{
    VX_STATIC_ASSERT_MSG(is_mask(MinCap), "MinCap must be 2^k - 1");
    VX_STATIC_ASSERT_MSG(GrowShift >= 1 && GrowShift < sizeof(hash_size_t) * 8 - 1,
        "GrowShift must be at least 1 (the table has to grow) and well below the type width");

    static constexpr bool pow2_capacity = true;
    static constexpr hash_size_t min_capacity = MinCap;

    // Largest cap we will ever hand out. Keeps cap + 1 and cap + group_width
    // from overflowing hash_size_t. The allocator will usually fail long before.
    static constexpr hash_size_t max_capacity = std::numeric_limits<hash_size_t>::max() >> 1;

    static constexpr hash_size_t max_load(hash_size_t cap) noexcept
    {
        return Load::max_load(cap);
    }

    static constexpr hash_size_t wrap(hash_size_t hash, hash_size_t cap) noexcept
    {
        return hash & cap;
    }

    static constexpr hash_size_t next_capacity(hash_size_t current, hash_size_t required) noexcept
    {
        const hash_size_t req = required == 0 ? 1 : required;

        // the geometric step below must stay <= max_capacity
        if (current > (max_capacity >> GrowShift))
        {
            return 0;
        }

        // can't satisfy the request even at the largest cap
        if (req > max_load(max_capacity))
        {
            return 0;
        }

        // ((current + 1) << shift) - 1 keeps the 2^k - 1 form, and always grows
        hash_size_t cap = ((current + 1) << GrowShift) - 1;
        if (cap < MinCap)
        {
            cap = MinCap;
        }

        // Jump further if the step isn't enough. Every value visited is already
        // 2^k - 1, so nothing needs rounding. At most ~63 iterations, and only
        // on a rehash, which is O(n) anyway.
        while (max_load(cap) < req)
        {
            if (cap >= max_capacity)
            {
                return 0;
            }

            cap = cap * 2 + 1;
        }

        return cap;
    }
};

using pow2_sizing = pow2_sizing_t<load_7_8>;
using default_sizing = pow2_sizing;                   // flat
using default_chain_sizing = pow2_sizing_t<load_1_0>; // chain

//-------------------------------------------------------------------------
// policy validation (each table static-asserts these on its Sizing)
//-------------------------------------------------------------------------

template <typename S>
constexpr bool valid_sizing() noexcept
{
    return S::max_load(0) == 0 && is_mask(S::min_capacity) && is_mask(S::max_capacity) && S::max_load(S::min_capacity) >= 1;
}

// flat tables only: the minimum size must keep a free slot
template <typename S>
constexpr bool valid_flat_sizing() noexcept
{
    return valid_sizing<S>() && S::max_load(S::min_capacity) < S::min_capacity;
}

//-------------------------------------------------------------------------
// compile-time tests
// (must live outside the classes: constexpr members can't be used in
// constant expressions until the class is complete)
//-------------------------------------------------------------------------

VX_STATIC_ASSERT_MSG(valid_flat_sizing<default_sizing>(), "default flat sizing");
VX_STATIC_ASSERT_MSG(valid_sizing<default_chain_sizing>(), "default chain sizing");
VX_STATIC_ASSERT_MSG(is_mask(default_sizing::max_capacity), "max_capacity must be 2^k - 1");
VX_STATIC_ASSERT_MSG(default_sizing::max_capacity + 1 > default_sizing::max_capacity, "cap + 1 must not overflow");

// flat: the ladder of usable sizes
VX_STATIC_ASSERT_MSG(default_sizing::max_load(0) == 0, "max_load(0) must be 0");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(1) == 0, "cap 1 holds nothing, hence min_capacity > 1");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(3) == 2, "cap 3 -> 2 elements");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(7) == 6, "cap 7 -> 6 elements");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(15) == 14, "cap 15 -> 14 elements");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(31) == 28, "cap 31 -> 28 elements");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(63) == 56, "cap 63 -> 56 elements");
VX_STATIC_ASSERT_MSG(default_sizing::max_load(1) < 1 && default_sizing::max_load(3) < 3 && default_sizing::max_load(7) < 7 && default_sizing::max_load(15) < 15 && default_sizing::max_load(31) < 31,
    "flat: max_load(cap) must be < cap");

// flat: growth from empty
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 1) == 3, "first allocation");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 2) == 3, "2 elements fit in cap 3");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 3) == 7, "3 elements need cap 7");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 6) == 7, "6 elements fit in cap 7");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 7) == 15, "7 elements need cap 15");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 28) == 31, "28 elements fit in cap 31");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, 29) == 63, "29 elements need cap 63");

// flat: growth from an existing table doubles at minimum, jumps further if asked
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(3, 3) == 7, "3 -> 7");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(7, 7) == 15, "7 -> 15");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(15, 15) == 31, "15 -> 31");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(31, 1) == 63, "always grows, even if required is small");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(7, 100) == 127, "can skip levels");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(3, 0) == 7, "required == 0 is treated as 1");

// limits
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(default_sizing::max_capacity, 1) == 0, "cannot grow past max_capacity");
VX_STATIC_ASSERT_MSG(default_sizing::next_capacity(0, std::numeric_limits<hash_size_t>::max()) == 0, "unrepresentable request");

// chain: one element per bucket
VX_STATIC_ASSERT_MSG(default_chain_sizing::max_load(0) == 0, "max_load(0) must be 0");
VX_STATIC_ASSERT_MSG(default_chain_sizing::max_load(3) == 4, "cap 3 -> 4 buckets -> 4 elements");
VX_STATIC_ASSERT_MSG(default_chain_sizing::next_capacity(0, 4) == 3, "4 elements fit in cap 3");
VX_STATIC_ASSERT_MSG(default_chain_sizing::next_capacity(0, 5) == 7, "5 elements need cap 7");
VX_STATIC_ASSERT_MSG(default_chain_sizing::next_capacity(3, 1) == 7, "always grows");

// custom growth factor: x4
VX_STATIC_ASSERT_MSG((pow2_sizing_t<load_7_8, 3, 2>::next_capacity(3, 1) == 15), "x4 growth: 3 -> 15");
VX_STATIC_ASSERT_MSG((pow2_sizing_t<load_7_8, 3, 2>::next_capacity(0, 1) == 3), "x4 growth: first allocation is still min_capacity");

// custom minimum
VX_STATIC_ASSERT_MSG((pow2_sizing_t<load_7_8, 15>::next_capacity(0, 1) == 15), "min_capacity 15");

//=========================================================================
// slot policies
//=========================================================================

template <typename K>
struct set_policy
{
    using key_type = K;
    using slot_type = K;

    static constexpr bool mutable_slots = false;

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
    // noexcept iff the move is, so rehash can tell whether it can relocate
    // without risking a half-moved table.
    static void relocate(slot_type* dst, slot_type* src) noexcept(std::is_nothrow_move_constructible<slot_type>::value)
    {
        construct(dst, std::move(*src));
        destroy(src);
    }
};

//=========================================================================

template <typename K, typename V>
struct map_policy
{
    using key_type = K;
    using mapped_type = V;
    using slot_type = vx::pair<const K, V>;
    using mutable_slot_type = vx::pair<K, V>;

    static constexpr bool mutable_slots = true;

    static const K& key(const slot_type& s) noexcept
    {
        return s.first;
    }

    // (key, value args...): value is only constructed when we actually insert.
    // Disabled for whole-pair arguments (slot_type or mutable_slot_type) so they
    // reach the non-template overloads below instead of being treated as a key.
    template <
        typename KK,
        typename... VArgs,
        VX_REQUIRES((!std::is_same<VX_STRIP_T(KK), slot_type>::value &&
            !std::is_same<VX_STRIP_T(KK), mutable_slot_type>::value))>
    static void construct(slot_type* p, KK&& k, VArgs&&... v)
    {
        mem::construct_in_place(p, std::piecewise_construct,
            std::forward_as_tuple(std::forward<KK>(k)),
            std::forward_as_tuple(std::forward<VArgs>(v)...));
    }

    // copy of an existing slot (table copy). Non-template, so it wins over the one above.
    static void construct(slot_type* p, const slot_type& s)
    {
        mem::construct_in_place(p, s);
    }

    static void construct(slot_type* p, slot_type&& s)
    {
        mem::construct_in_place(p, std::move(s));
    }

    // pair<K, V> -> pair<const K, V> (e.g. insert(std::make_pair(k, v)))
    static void construct(slot_type* p, const mutable_slot_type& s)
    {
        mem::construct_in_place(p, s);
    }

    static void construct(slot_type* p, mutable_slot_type&& s)
    {
        mem::construct_in_place(p, std::move(s));
    }

    static void destroy(slot_type* p) noexcept
    {
        mem::destroy_in_place(p);
    }

    // Move the key too (a const K member would be copied otherwise).
    //
    // NOTE: this views a pair<const K, V> as a pair<K, V>. That works on every
    // mainstream ABI (it is what older Abseil did) and is guarded by the size /
    // alignment assert, but the standard does not guarantee the two layouts match.
    // The strictly-conforming alternative is to make the slot a
    // union { pair<const K, V> value; pair<K, V> mutable_value; } as Abseil does now.
    static void relocate(slot_type* dst, slot_type* src) noexcept(
        std::is_nothrow_move_constructible<K>::value && std::is_nothrow_move_constructible<V>::value)
    {
        VX_STATIC_ASSERT_MSG(sizeof(mutable_slot_type) == sizeof(slot_type) && alignof(mutable_slot_type) == alignof(slot_type),
            "pair<K, V> and pair<const K, V> must be layout compatible");

        mem::construct_in_place(
            reinterpret_cast<mutable_slot_type*>(dst),
            std::move(*reinterpret_cast<mutable_slot_type*>(src)));
        destroy(src);
    }
};

} // namespace _hash_priv
} // namespace vx
