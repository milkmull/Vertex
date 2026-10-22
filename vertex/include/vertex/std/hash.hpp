#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/crypto/fnv1a.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

using hash_t = size_t;

//=========================================================================
// internals
//=========================================================================

namespace _hash_priv {

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

template <typename F>
uint64_t float_bits(F f) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(F) <= sizeof(uint64_t), "unexpected floating-point size");

    if (f == F(0))
    {
        f = F(0);
    }

    uint64_t b = 0;
    mem::copy(&b, &f, sizeof(F));
    return b;
}

template <typename K>
struct is_int_hashable
{
    static constexpr bool value = std::is_integral<K>::value || std::is_enum<K>::value;
};

template <typename K>
struct is_float_hashable
{
    static constexpr bool value = std::is_same<K, float>::value || std::is_same<K, double>::value;
};

} // namespace _hash_priv

//=========================================================================
// hash
//=========================================================================

template <typename K, typename = void>
struct hash
{
    VX_STATIC_ASSERT_MSG(type_traits::always_false<K>::value, "vx::hash<K> is not defined for this type; specialize it or provide a hasher");
};

// integers, enums
template <typename K>
struct hash<K, VX_REQUIRES_TYPE(_hash_priv::is_int_hashable<K>::value)>
{
    using is_avalanching = void;

    hash_t operator()(K k) const noexcept
    {
        return _hash_priv::mix_bits(_hash_priv::fold(static_cast<uint64_t>(k)));
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

// nullptr
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

template <typename K, typename Inner = hash<K>>
struct seeded_hash
{
    using is_avalanching = void;

    hash_t seed;

    explicit seeded_hash(hash_t s = 0) noexcept
        : seed(s)
    {}

    hash_t operator()(const K& k) const noexcept(noexcept(Inner()(k)))
    {
        return _hash_priv::mix_bits(Inner()(k) + seed * static_cast<hash_t>(0x9E3779B97F4A7C15ull));
    }
};

} // namespace vx
