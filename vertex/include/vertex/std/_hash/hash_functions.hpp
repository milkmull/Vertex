#pragma once

// Hashers. A hasher that declares `using is_avalanching = void;` is trusted by
// the table. Everything else goes through finalize_hash (hash_common.hpp).

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <type_traits>

#include "vertex/config/language_config.hpp"
#include "vertex/std/crypto/fnv1a.hpp" // path is a guess

namespace vx {

//=========================================================================
// bit mixer
//=========================================================================

// murmur3 finalizer, both bits of the result are well mixed
inline size_t mix_bits(size_t x) noexcept
{
    VX_IF_CONSTEXPR (sizeof(size_t) == 8)
    {
        x ^= x >> 33;
        x *= static_cast<size_t>(0xff51afd7ed558ccdull);
        x ^= x >> 33;
        x *= static_cast<size_t>(0xc4ceb9fe1a85ec53ull);
        x ^= x >> 33;
    }
    else
    {
        x ^= x >> 16;
        x *= static_cast<size_t>(0x85ebca6bu);
        x ^= x >> 13;
        x *= static_cast<size_t>(0xc2b2ae35u);
        x ^= x >> 16;
    }
    return x;
}

namespace _hash_priv {

template <typename K>
size_t to_size(const K& k, std::true_type /*is_pointer*/) noexcept
{
    return static_cast<size_t>(reinterpret_cast<uintptr_t>(k));
}

template <typename K>
size_t to_size(const K& k, std::false_type) noexcept
{
    const unsigned long long u = static_cast<unsigned long long>(k);
    VX_IF_CONSTEXPR (sizeof(size_t) < sizeof(unsigned long long))
    {
        return static_cast<size_t>(u ^ (u >> 32)); // fold, so the high half still counts
    }
    else
    {
        return static_cast<size_t>(u);
    }
}

} // namespace _hash_priv

//=========================================================================
// default_hash
//=========================================================================

// Integers, enums and pointers: one-shot mixer, avalanching.
// Everything else: std::hash<K>, which is not trusted (the table mixes it).
template <typename K, typename = void>
struct default_hash : std::hash<K>
{};

template <typename K>
struct default_hash<K, typename std::enable_if<std::is_integral<K>::value || std::is_enum<K>::value || std::is_pointer<K>::value>::type>
{
    using is_avalanching = void;

    size_t operator()(K k) const noexcept
    {
        return mix_bits(_hash_priv::to_size(k, std::is_pointer<K>{}));
    }
};

//=========================================================================
// FNV-1a
//=========================================================================

// Built on vx::fnv1a (the incremental hasher). Contiguous string-likes (anything
// with data() and size() whose value_type is trivially copyable) and C strings.
// Transparent: pair with std::equal_to<void> to look up string_view/const char*
// in a string-keyed container without a temporary. NOT avalanching, so the
// table mixes the result.
struct fnv1a_hash
{
    using is_transparent = void;

    template <typename S>
    size_t operator()(const S& s) const noexcept
    {
        fnv1a h;
        h.update(s.data(), s.data() + s.size());
        return h.result();
    }

    size_t operator()(const char* s) const noexcept
    {
        fnv1a h;
        h.update(s, s + std::strlen(s));
        return h.result();
    }
};

//=========================================================================
// seeded
//=========================================================================

// Runtime seed on top of any hasher (flood resistance, different order per
// table). Stateful, so it is not empty-base-optimized.
template <typename K, typename Inner = default_hash<K>>
struct seeded_hash
{
    using is_avalanching = void;

    size_t seed;

    explicit seeded_hash(size_t s = 0) noexcept
        : seed(s)
    {}

    size_t operator()(const K& k) const noexcept(noexcept(Inner()(k)))
    {
        return mix_bits(Inner()(k) + seed * static_cast<size_t>(0x9E3779B97F4A7C15ull));
    }
};

} // namespace vx
