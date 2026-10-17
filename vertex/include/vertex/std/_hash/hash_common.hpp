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

// Required of a growth policy:
//   static constexpr size_t min_capacity;
//   static constexpr size_t max_size(size_t cap);           // load factor, max_size(0) == 0, monotonic
//   static size_t next_capacity(size_t current, size_t required, size_t max_cap);
//       current: slots now (0 = unallocated). required: elements that must fit.
//       max_cap: raw ceiling in slots (need not be a valid capacity).
//       Returns C > current with max_size(C) >= required and C <= max_cap, or 0.
//   static constexpr size_t wrap(size_t x, size_t cap);      // any x -> [0, cap]
//   static constexpr size_t wrap_lane(size_t x, size_t cap); // x < 2 * (cap + 1) -> [0, cap]
//   static constexpr size_t advance(size_t offset, size_t index, size_t cap);
//   static size_t cleanup_limit(size_t max_load);            // inherit growth_defaults

template <typename Self>
struct growth_defaults
{
    static size_t cleanup_limit(size_t max_load) noexcept
    {
        return (max_load / 32) * 25 + ((max_load % 32) * 25) / 32;
    }

    // General probing, valid for any capacity
    static constexpr size_t wrap(size_t x, size_t cap) noexcept
    {
        return x % (cap + 1);
    }

    static constexpr size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        return x >= cap + 1 ? x - (cap + 1) : x;
    }

    static constexpr size_t advance(size_t offset, size_t, size_t cap) noexcept
    {
        return Self::wrap_lane(offset + group_width, cap);
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
// pow2_growth: capacities 2^n - 1, load 7/8, mask wrap, triangular probing.
//-------------------------------------------------------------------------
struct pow2_growth : growth_defaults<pow2_growth>
{
    static constexpr size_t min_capacity = 3;

    static constexpr bool is_valid_capacity(size_t cap) noexcept
    {
        return (cap & (cap + 1)) == 0;
    }

    // Elements that fit in cap slots (7/8), 0 for cap == 0.
    static constexpr size_t max_size(size_t cap) noexcept
    {
        const size_t g = cap - cap / 8;
        // a completely full table of cap < group_width has no EMPTY byte
        return (g >= cap && cap >= group_width - 1) ? cap - 1 : g;
    }

    static size_t next_capacity(size_t current, size_t required, size_t max_cap) noexcept
    {
        // 1) normal grow: the next capacity up
        size_t cap = min_capacity;
        if (current >= min_capacity)
        {
            if (current > (SIZE_MAX - 1) / 2)
            {
                return 0;
            }
            cap = current * 2 + 1;
        }

        // 2) not enough (reserve, or a big jump): smallest 2^k - 1 that holds `required`
        if (max_size(cap) < required)
        {
            // max_size(c) <= c, so nothing under max_cap can hold this
            if (required > max_cap)
            {
                return 0;
            }

            cap = round_up_to_mask(required);

            // round_up_to_mask(required) can be one power short because of the load factor
            while (max_size(cap) < required)
            {
                if (cap > max_cap)
                {
                    return 0;
                }
                cap = cap * 2 + 1; // cap <= max_cap, so no overflow
            }
        }

        return cap <= max_cap ? cap : 0;
    }

    // mask instead of modulo: valid because cap + 1 is a power of two
    static constexpr size_t wrap(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }

    static constexpr size_t wrap_lane(size_t x, size_t cap) noexcept
    {
        return x & cap;
    }

    // triangular: group starts at 0, W, 3W, 6W, ... which visits every group on a power of two ring
    static constexpr size_t advance(size_t offset, size_t index, size_t cap) noexcept
    {
        return (offset + index) & cap;
    }
};

using default_growth = pow2_growth;

template <typename Growth>
size_t cleanup_limit_of(size_t cap) noexcept
{
    const size_t max_load = Growth::max_size(cap);
    VX_ASSERT(max_load > 0);
    const size_t limit = Growth::cleanup_limit(max_load);
    VX_ASSERT(limit < max_load); // otherwise cleanup would never be chosen
    return limit;
}

//=========================================================================
// cleanup limit
//=========================================================================

template <typename Growth, typename = void>
struct has_cleanup_limit : std::false_type
{};

template <typename Growth>
struct has_cleanup_limit<Growth, type_traits::void_t<decltype(Growth::cleanup_limit(size_t{}))>> : std::true_type
{};

// clamped so cleanup is only chosen when it can free at least one slot
template <typename Growth>
size_t cleanup_limit_of(size_t cap) noexcept
{
    const size_t max_load = Growth::max_size(cap);
    VX_ASSERT(max_load > 0);

    size_t limit;

    VX_IF_CONSTEXPR (has_cleanup_limit<Growth>::value)
    {
        limit = Growth::cleanup_limit(max_load);
    }
    else
    {
        limit = default_cleanup_limit(max_load);
    }

    // limit must be < max load or cleanup will never happen
    VX_ASSERT(limit < max_load);
    return limit;
}

//=========================================================================
// hash helpers
//=========================================================================

template <typename H, typename = void>
struct is_avalanching : std::false_type
{};

template <typename H>
struct is_avalanching<H, type_traits::void_t<typename H::is_avalanching>> : std::true_type
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
