#pragma once

// Pieces shared by every backend: growth and load policies, traits, hash
// helpers and slot policies. Nothing here knows how elements are stored.

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/memory.hpp"

#include "hash_group.hpp" // group_width, used by the policies below

namespace vx {
namespace _hash_priv {

//=========================================================================
// growth policy
//=========================================================================

// Required:
//   static constexpr bool is_valid_capacity(size_t cap);
//   static constexpr size_t max_size(size_t cap);          // max_size(0) == 0
//   static size_t next_capacity(size_t current, size_t required_size, size_t max_capacity);
//       Returns C with is_valid_capacity(C), C > current, max_size(C) >= required_size,
//       C <= max_capacity. Returns 0 if none exists. current == 0 means unallocated.
//
// Optional (inherit growth_defaults<Self>): cleanup_limit

inline size_t default_cleanup_limit(size_t m) noexcept
{
    return (m / 32) * 25 + ((m % 32) * 25) / 32;
}

template <typename Self>
struct growth_defaults
{
    static size_t cleanup_limit(size_t max_load) noexcept
    {
        return default_cleanup_limit(max_load);
    }
};

// Smallest 2^k - 1 that is >= x.
constexpr size_t round_up_to_mask(size_t x) noexcept
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    VX_IF_CONSTEXPR (sizeof(size_t) == 8)
    {
        x |= x >> 32;
    }
    return x;
}

//-------------------------------------------------------------------------
// pow2_growth: capacities 2^n - 1, load 7/8, triangular probing. Default.
//-------------------------------------------------------------------------
struct pow2_growth : growth_defaults<pow2_growth>
{
    static constexpr size_t min_capacity = 3;

    // for asserts
    static constexpr bool is_valid_capacity(size_t cap) noexcept
    {
        return (cap & (cap + 1)) == 0;
    }

    // number of elements that fit before the next growth, 0 for cap == 0
    static constexpr size_t max_size(size_t cap) noexcept
    {
        const size_t g = cap - cap / 8;
        // a completely full table of cap < group_width has no EMPTY byte
        return (g >= cap && cap >= group_width - 1) ? cap - 1 : g;
    }

    // Contract: max_cap is a valid capacity (for pow2, 2^k - 1), computed once by the table.
    // Never returns 0. If the request can't be met it returns max_cap, so the table
    // checks `required > max_size(max_cap)` for length_error before calling.
    static size_t next_capacity(size_t current, size_t required, size_t max_cap) noexcept
    {
        // Fast path: the next capacity up, which is what a normal grow needs
        size_t cap = min_capacity;
        if (current >= min_capacity)
        {
            if (current > max_cap / 2)
            {
                return max_cap;
            }
            cap = current * 2 + 1;
        }

        if (max_size(cap) >= required)
        {
            return cap;
        }

        // Big jump (reserve): ~9/8 * required, rounded up to 2^k - 1
        if (required > max_size(max_cap))
        {
            return max_cap;
        }

        cap = round_up_to_mask(required + (required >> 3)); // required <= max_cap, no overflow
        if (cap > max_cap)
        {
            cap = max_cap;
        }

        // 9/8 is a bit under 8/7, so this can be one power short
        while (max_size(cap) < required)
        {
            cap = cap * 2 + 1; // can't pass max_cap: max_size(max_cap) >= required
        }

        return cap;
    }
};

//-------------------------------------------------------------------------
// factor_growth: any capacity, grows by GrowNum/GrowDen, load LoadNum/LoadDen,
// linear probing + modulo wrap. Trades speed for memory use.
//-------------------------------------------------------------------------
template <size_t GrowNum = 3, size_t GrowDen = 2, size_t LoadNum = 7, size_t LoadDen = 8>
struct factor_growth : growth_defaults<factor_growth<GrowNum, GrowDen, LoadNum, LoadDen>>
{
    VX_STATIC_ASSERT_MSG(GrowNum > GrowDen && GrowDen > 0, "growth factor must be > 1");
    VX_STATIC_ASSERT_MSG(LoadNum > 0 && LoadNum < LoadDen, "load factor must be in (0, 1)");

    static constexpr size_t min_capacity = 7;

    VX_STATIC_ASSERT_MSG(min_capacity + group_width < 2 * (min_capacity + 1),
        "wrap_lane's single subtract needs cap + group_width - 1 < 2 * (cap + 1) for every capacity");

    static constexpr bool is_valid_capacity(size_t cap) noexcept
    {
        return cap == 0 || cap >= min_capacity;
    }

    static constexpr size_t max_size(size_t cap) noexcept
    {
        // cap - floor(cap * (LoadDen - LoadNum) / LoadDen), overflow-safe
        constexpr size_t spare = LoadDen - LoadNum;
        const size_t g = cap - ((cap / LoadDen) * spare + ((cap % LoadDen) * spare) / LoadDen);

        return (g >= cap && cap >= group_width - 1) ? cap - 1 : g;
    }

    static size_t next_capacity(size_t current, size_t required, size_t max_cap) noexcept
    {
        // keep room for the cloned group bytes
        const size_t limit = max_cap < SIZE_MAX - group_width ? max_cap : SIZE_MAX - group_width;

        // 1) geometric step from the current capacity
        size_t cap = min_capacity;
        if (current != 0)
        {
            constexpr size_t grow = GrowNum - GrowDen;
            const size_t q = current / GrowDen;
            if (q >= SIZE_MAX / grow)
            {
                return 0;
            }
            size_t extra = q * grow + ((current % GrowDen) * grow) / GrowDen;
            if (extra == 0)
            {
                extra = 1;
            }
            if (extra > limit || current > limit - extra)
            {
                return 0;
            }
            cap = current + extra;
        }

        // 2) if that isn't enough, jump to ceil(required * LoadDen / LoadNum)
        if (max_size(cap) < required)
        {
            const size_t q = required / LoadNum;
            if (q > SIZE_MAX / LoadDen)
            {
                return 0;
            }
            const size_t hi = q * LoadDen;
            const size_t lo = ((required % LoadNum) * LoadDen + LoadNum - 1) / LoadNum;
            if (lo > SIZE_MAX - hi)
            {
                return 0;
            }

            cap = hi + lo;
            if (cap < min_capacity)
            {
                cap = min_capacity;
            }
            while (max_size(cap) < required) // estimate may be one short
            {
                ++cap;
            }
        }

        return cap <= limit ? cap : 0;
    }
};

using default_growth = pow2_growth;

template <typename G>
size_t cleanup_limit_of(size_t max_load) noexcept
{
    VX_ASSERT(max_load > 0);
    const size_t limit = G::cleanup_limit(max_load);
    return limit < max_load ? limit : max_load - 1;
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


} // namespace _hash_priv
} // namespace vx
