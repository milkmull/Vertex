#pragma once

// Traits pieces that are not tied to one backend: the load policy for chaining,
// the default traits of both backends, traits validation, and the names users
// put in their own Traits.

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "vertex/config/language_config.hpp"

#include "hash_common.hpp"

namespace vx {
namespace _hash_priv {

//=========================================================================
// load policy for closed addressing
//=========================================================================

// Elements per bucket may exceed 1 (Num >= Den). The bucket count is cap + 1,
// which is the size of the ring the growth policy wraps into. There are no
// tombstones, so there is no cleanup_limit.
template <size_t Num, size_t Den>
struct bucket_load_ratio
{
    VX_STATIC_ASSERT_MSG(Num > 0 && Den > 0, "load ratio must be positive");

    // floor((cap + 1) * Num / Den), saturating. max_size(0) == 0.
    static constexpr size_t max_size(size_t cap) noexcept
    {
        if (cap == 0)
        {
            return 0;
        }

        const size_t buckets = cap + 1;
        const size_t q = buckets / Den;
        const size_t r = buckets % Den;
        const size_t hi = q > SIZE_MAX / Num ? SIZE_MAX : q * Num;
        const size_t lo = (r * Num) / Den;
        return hi > SIZE_MAX - lo ? SIZE_MAX : hi + lo;
    }
};

using load_1_1 = bucket_load_ratio<1, 1>;

//=========================================================================
// default traits
//=========================================================================

using default_hash_traits = default_table_traits; // open addressing: pow2, 7/8, no cached hash

// chaining: pow2 bucket counts, about one element per bucket
using default_chain_traits = hash_traits<pow2_growth, load_1_1, false>;

//=========================================================================
// validation
//=========================================================================

// Detects a cleanup_limit member of ANY signature. has_cleanup_limit (hash_common)
// only detects the callable one, so a wrong signature would silently fall back
// to the default. A misspelled NAME cannot be detected from here.
template <typename L, typename = void>
struct has_any_cleanup_limit : std::false_type
{};

template <typename L>
struct has_any_cleanup_limit<L, typename make_void<decltype(&L::cleanup_limit)>::type> : std::true_type
{};

template <typename Traits>
struct validate_traits
{
    using growth = typename Traits::growth_policy;
    using load = typename Traits::load_policy;

    VX_STATIC_ASSERT_MSG((std::is_same<decltype(Traits::cache_hash), const bool>::value),
        "Traits::cache_hash must be a static constexpr bool");

    VX_STATIC_ASSERT_MSG((growth::min_capacity >= 1), "growth policy min_capacity must be >= 1");

    VX_STATIC_ASSERT_MSG((!has_any_cleanup_limit<load>::value || has_cleanup_limit<load>::value),
        "load policy declares cleanup_limit, but it is not callable as size_t(size_t)");

    static constexpr bool value = true;
};

} // namespace _hash_priv

//=========================================================================
// user-facing names (what goes into Traits)
//=========================================================================

using _hash_priv::bucket_load_ratio;
using _hash_priv::factor_growth;
using _hash_priv::hash_traits;
using _hash_priv::max_load_ratio;
using _hash_priv::pow2_growth;

} // namespace vx
