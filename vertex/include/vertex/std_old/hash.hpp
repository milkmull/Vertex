#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

using hash_t = size_t;

//=========================================================================
// internals
//=========================================================================

namespace _hash_priv {

// murmur3 finalizer. Note: mix_bits(0) == 0, so don't use hash 0 as a sentinel.
inline hash_t mix_bits(hash_t x) noexcept
{
    VX_IF_CONSTEXPR (sizeof(hash_t) == 8)
    {
        uint64_t v = static_cast<uint64_t>(x);
        v ^= v >> 33;
        v *= 0xff51afd7ed558ccdull;
        v ^= v >> 33;
        v *= 0xc4ceb9fe1a85ec53ull;
        v ^= v >> 33;
        return static_cast<hash_t>(v);
    }
    else
    {
        uint32_t v = static_cast<uint32_t>(x);
        v ^= v >> 16;
        v *= 0x85ebca6bu;
        v ^= v >> 13;
        v *= 0xc2b2ae35u;
        v ^= v >> 16;
        return static_cast<hash_t>(v);
    }
}

// Reduce a 64-bit value to hash_t, keeping information from both halves
// when hash_t is 32 bits.
inline hash_t fold(uint64_t u) noexcept
{
    VX_IF_CONSTEXPR (sizeof(hash_t) >= sizeof(uint64_t))
    {
        return static_cast<hash_t>(u);
    }
    else
    {
        return static_cast<hash_t>(u ^ (u >> 32));
    }
}

// Returns the raw bits of a float/double, zero-extended to 64 bits.
// +0.0 and -0.0 compare equal, so they are normalized to the same bits.
// (NaNs are not canonicalized: they never compare equal, so it doesn't
// matter for lookups.)
template <typename F>
uint64_t float_bits(F f) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(F) == sizeof(uint32_t) || sizeof(F) == sizeof(uint64_t), "unexpected floating-point size");

    using bits_t = typename std::conditional<sizeof(F) == sizeof(uint32_t), uint32_t, uint64_t>::type;

    if (f == F(0)) // intentionally exact: maps -0.0 to +0.0
    {
        f = F(0);
    }

    // Copy into an integer of the same size so the result doesn't depend on endianness.
    bits_t b = 0;
    mem::copy(&b, &f, sizeof(F));
    return static_cast<uint64_t>(b);
}

// Integers and enums up to 64 bits. Wider types (e.g. __int128) are
// deliberately excluded: they would be silently truncated and collide.
template <typename K>
struct is_int_hashable
{
    static constexpr bool value = (std::is_integral<K>::value || std::is_enum<K>::value) && sizeof(K) <= sizeof(uint64_t);
};

template <typename K>
struct is_float_hashable
{
    using U = typename std::remove_cv<K>::type;
    static constexpr bool value = std::is_same<U, float>::value || std::is_same<U, double>::value;
};

} // namespace _hash_priv

//=========================================================================
// hash
//=========================================================================

// Primary template: not hashable. The call operator is deleted rather than
// static_asserted, so "is this type hashable?" can be detected via SFINAE
// (e.g. decltype(std::declval<hash<K>>()(std::declval<K>()))) while still
// producing a clear error when used directly. Specialize it for your types.
template <typename K, typename = void>
struct hash
{
    hash_t operator()(const K&) const = delete;
};

// integers, enums
template <typename K>
struct hash<K, VX_REQUIRES_TYPE(_hash_priv::is_int_hashable<K>::value)>
{
    using is_avalanching = void;

    hash_t operator()(K k) const noexcept
    {
        VX_IF_CONSTEXPR (sizeof(K) <= sizeof(hash_t))
        {
            return _hash_priv::mix_bits(static_cast<hash_t>(k));
        }
        else
        {
            return _hash_priv::mix_bits(_hash_priv::fold(static_cast<uint64_t>(k)));
        }
    }
};

// pointers
template <typename T>
struct hash<T*>
{
    using is_avalanching = void;

    hash_t operator()(T* p) const noexcept
    {
        return _hash_priv::mix_bits(static_cast<hash_t>(reinterpret_cast<uintptr_t>(p)));
    }
};

// cv-qualified pointers (T* const, etc.)
template <typename T>
struct hash<T* const> : hash<T*>
{};

template <typename T>
struct hash<T* volatile> : hash<T*>
{};

template <typename T>
struct hash<T* const volatile> : hash<T*>
{};

// nullptr (consistent with hash<T*>(nullptr), since mix_bits(0) == 0)
template <>
struct hash<std::nullptr_t>
{
    using is_avalanching = void;

    hash_t operator()(std::nullptr_t) const noexcept
    {
        return 0;
    }
};

// float, double
template <typename K>
struct hash<K, VX_REQUIRES_TYPE(_hash_priv::is_float_hashable<K>::value)>
{
    using is_avalanching = void;

    hash_t operator()(K k) const noexcept
    {
        return _hash_priv::mix_bits(_hash_priv::fold(_hash_priv::float_bits(k)));
    }
};

//=========================================================================
// seeded_hash
//=========================================================================

// Wraps another hasher and mixes a seed into the result. The final mix_bits
// is required even when Inner is already avalanching: without it, the seed
// would only shift the hash by a constant, which doesn't change collisions.
template <typename K, typename Inner = hash<K>>
struct seeded_hash
{
    using is_avalanching = void;

    hash_t seed = 0;
    Inner inner{};

    seeded_hash() = default;

    explicit seeded_hash(hash_t s, Inner i = Inner()) noexcept
        : seed(s)
        , inner(i)
    {}

    hash_t operator()(const K& k) const noexcept(noexcept(std::declval<const Inner&>()(std::declval<const K&>())))
    {
        return _hash_priv::mix_bits(inner(k) + seed * static_cast<hash_t>(0x9E3779B97F4A7C15ull));
    }
};

//=========================================================================
// avalanching
//=========================================================================

// A hash is "avalanching" if flipping any single input bit flips each output
// bit with ~50% probability, i.e. its output bits are well mixed. Hash tables
// that take the slot position and metadata from different bit ranges of the
// hash rely on this. Hashers declare it by providing a nested
// `using is_avalanching = void;`. For hashers that don't (e.g. identity
// hashes), the table should apply finalize_hash() to the result.

template <typename H, typename = void>
struct is_avalanching : std::false_type
{};

template <typename H>
struct is_avalanching<H, type_traits::void_t<typename H::is_avalanching>> : std::true_type
{};

// Cheap finalizer: multiply, then fold the high half down. The multiply
// pushes entropy upward, and the xor-shift brings it back to the low bits,
// which H2 and the start position both use.
inline hash_t finalize_hash(hash_t h) noexcept
{
    VX_IF_CONSTEXPR (sizeof(hash_t) == 8)
    {
        h *= static_cast<hash_t>(0x9E3779B97F4A7C15ull);
        h ^= h >> 32;
    }
    else
    {
        h *= static_cast<hash_t>(0x9E3779B1u);
        h ^= h >> 16;
    }
    return h;
}

//=========================================================================

template <typename K>
using default_hash = hash<K>;

} // namespace vx
