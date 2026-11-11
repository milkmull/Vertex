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

using hash_size_t = size_t;

//=========================================================================
// small constexpr helpers
//=========================================================================

// Takes by value on purpose: std::max/vx::max take const&, which would
// ODR-use static constexpr members (e.g. min_capacity).
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

template <typename Load>
struct pow2_sizing_t
{
    // smallest cap ever allocated We overallocate a little on the first
    // allocations so early rehashes are rarer.
    static constexpr hash_size_t min_capacity = 3;
    // log2 of the growth factor
    static constexpr hash_size_t growth_shift = 1;

    VX_STATIC_ASSERT_MSG(is_mask(min_capacity), "min_capacity must be 2^k - 1");
    VX_STATIC_ASSERT_MSG(growth_shift >= 1 && growth_shift < sizeof(hash_size_t) * 8 - 1,
        "growth_shift must be at least 1 (the table has to grow) and well below the type width");

    static constexpr bool pow2_capacity = true;

    // Largest cap we will ever hand out. Keeps cap + 1 and cap + group_width
    // from overflowing hash_size_t.
    static constexpr hash_size_t max_capacity = std::numeric_limits<hash_size_t>::max() >> 1;

    // max number of elements we can hold at the given capacity
    static constexpr hash_size_t max_load(hash_size_t cap) noexcept
    {
        return Load::max_load(cap);
    }

    static constexpr hash_size_t wrap(hash_t hash, hash_size_t cap) noexcept
    {
        return hash & cap;
    }

    // Smallest valid cap that is > current, >= min_capacity and has
    // max_load(cap) >= required. Returns 0 if not representable.
    static constexpr hash_size_t next_capacity(hash_size_t current, hash_size_t required) noexcept
    {
        VX_ASSERT(required > 0);

        // the geometric step below must stay <= max_capacity
        if (current > (max_capacity >> growth_shift))
        {
            return 0;
        }

        // can't satisfy the request even at the largest cap
        if (required > max_load(max_capacity))
        {
            return 0;
        }

        // ((current + 1) << shift) - 1 keeps the 2^k - 1 form, and always grows
        hash_size_t cap = ((current + 1) << growth_shift) - 1;
        if (cap < min_capacity)
        {
            cap = min_capacity;
        }

        // jump further if the step isn't enough
        while (max_load(cap) < required)
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

using default_flat_sizing = pow2_sizing_t<load_7_8>;  // flat
using default_chain_sizing = pow2_sizing_t<load_1_0>; // chain

//-------------------------------------------------------------------------
// Compile-time sizing checks
//-------------------------------------------------------------------------

// Generic invariants every sizing policy must satisfy.
template <typename Sizing>
constexpr bool valid_sizing() noexcept
{
    return Sizing::max_load(0) == 0 && is_mask(Sizing::min_capacity) && is_mask(Sizing::max_capacity) && Sizing::min_capacity <= Sizing::max_capacity && Sizing::max_load(Sizing::min_capacity) >= 1 && Sizing::max_capacity + 1 > Sizing::max_capacity; // cap + 1 must not overflow
}

//=========================================================================
// map_pair
//=========================================================================

template <typename K, typename V>
struct map_pair
{
    using key_type = K;
    using value_type = V;

    K key;
    V value;

    constexpr map_pair() : key(), value()
    {}

    map_pair(const map_pair&) = default;
    map_pair(map_pair&&) = default;
    map_pair& operator=(const map_pair&) = default;
    map_pair& operator=(map_pair&&) = default;
    ~map_pair() = default;

    constexpr map_pair(const K& a, const V& b)
        : key(a), value(b)
    {}

    template <typename U1, typename U2, VX_REQUIRES(std::is_constructible<K, U1&&>::value&& std::is_constructible<V, U2&&>::value)>
    constexpr map_pair(U1&& a, U2&& b)
        : key(std::forward<U1>(a))
        , value(std::forward<U2>(b))
    {}

    // map_pair<K, V> -> map_pair<const K, V>
    template <typename U1, typename U2, VX_REQUIRES(std::is_constructible<K, const U1&>::value&& std::is_constructible<V, const U2&>::value)>
    constexpr map_pair(const map_pair<U1, U2>& p)
        : key(p.key), value(p.value)
    {}

    template <typename U1, typename U2, VX_REQUIRES(std::is_constructible<K, U1&&>::value&& std::is_constructible<V, U2&&>::value)>
    constexpr map_pair(map_pair<U1, U2>&& p)
        : key(std::forward<U1>(p.key)), value(std::forward<U2>(p.value))
    {}

    // piecewise (what map_policy::construct uses)
    template <typename... A1, typename... A2>
    map_pair(std::piecewise_construct_t, std::tuple<A1...> a, std::tuple<A2...> b)
        : map_pair(a, b, std::index_sequence_for<A1...>{}, std::index_sequence_for<A2...>{})
    {}

private:

    template <typename Tuple1, typename Tuple2, std::size_t... I1, std::size_t... I2>
    map_pair(Tuple1& a, Tuple2& b, std::index_sequence<I1...>, std::index_sequence<I2...>)
        : key(std::forward<typename std::tuple_element<I1, Tuple1>::type>(std::get<I1>(a))...)
        , value(std::forward<typename std::tuple_element<I2, Tuple2>::type>(std::get<I2>(b))...)
    {}
};

template <typename K, typename V>
constexpr bool operator==(const map_pair<K, V>& a, const map_pair<K, V>& b)
{
    return a.key == b.key && a.value == b.value;
}

template <typename K, typename V>
constexpr bool operator!=(const map_pair<K, V>& a, const map_pair<K, V>& b)
{
    return !(a == b);
}

//=========================================================================
// set policy
//=========================================================================

template <typename K>
struct set_policy
{
    using key_type = K;
    using slot_type = K;
    using const_slot_type = K;

    static constexpr bool mutable_slots = false;
    static constexpr bool trivially_copyable_slots = std::is_trivially_copyable<slot_type>::value;

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

    // Move src into raw memory at dst, then destroy src
    static void relocate(slot_type* dst, slot_type* src) noexcept(std::is_nothrow_move_constructible<slot_type>::value)
    {
        VX_IF_CONSTEXPR (trivially_copyable_slots)
        {
            mem::copy(dst, src, sizeof(slot_type));
        }
        else
        {
            construct(dst, std::move(*src));
            destroy(src);
        }
    }
};

//=========================================================================
// map policy
//=========================================================================

template <typename K, typename V>
struct map_policy
{
    using key_type = K;
    using mapped_type = V;
    using slot_type = map_pair<K, V>;             // what is stored
    using const_slot_type = map_pair<const K, V>; // what iterators show

    static constexpr bool mutable_slots = true;
    static constexpr bool trivially_copyable_slots = std::is_trivially_copyable<slot_type>::value;

    static const K& key(const slot_type& s) noexcept
    {
        return s.key;
    }

    static const K& key(const const_slot_type& s) noexcept
    {
        return s.key;
    }

    // (key, value args...): value is only constructed when we actually insert.
    // Excluded for whole-pair arguments so they reach the overloads below.
    template <
        typename KK,
        typename... VArgs,
        VX_REQUIRES((!std::is_same<VX_STRIP_T(KK), slot_type>::value &&
            !std::is_same<VX_STRIP_T(KK), const_slot_type>::value))>
    static void construct(slot_type* p, KK&& k, VArgs&&... v)
    {
        mem::construct_in_place(p, std::piecewise_construct,
            std::forward_as_tuple(std::forward<KK>(k)),
            std::forward_as_tuple(std::forward<VArgs>(v)...));
    }

    static void construct(slot_type* p, const slot_type& s)
    {
        mem::construct_in_place(p, s);
    }

    static void construct(slot_type* p, slot_type&& s)
    {
        mem::construct_in_place(p, std::move(s));
    }

    // copy from the const-key view (e.g. copying out of an iterator).
    // There is deliberately no && overload: a const K can only be copied.
    static void construct(slot_type* p, const const_slot_type& s)
    {
        mem::construct_in_place(p, s);
    }

    static void destroy(slot_type* p) noexcept
    {
        mem::destroy_in_place(p);
    }

    // The stored type has a non-const key, so relocation is a plain move
    // and the reinterpret_cast is gone.
    static void relocate(slot_type* dst, slot_type* src) noexcept(
        std::is_nothrow_move_constructible<slot_type>::value)
    {
        VX_IF_CONSTEXPR (trivially_copyable_slots)
        {
            mem::copy(dst, src, sizeof(slot_type));
        }
        else
        {
            construct(dst, std::move(*src));
            destroy(src);
        }
    }
};

} // namespace _hash_priv
} // namespace vx
