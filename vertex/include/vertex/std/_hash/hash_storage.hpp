#pragma once

// Allocator, hasher and eq storage shared by every backend. Stateless members
// are empty-base-optimized through compressed_pair, so they take no space.

#include <cstddef>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/algorithm.hpp" // vx::swap, path is a guess

#include "hash_common.hpp"

namespace vx {
namespace _hash_priv {

//=========================================================================
// table_storage
//=========================================================================

// allocator -> hasher -> eq -> data. Each `first` is empty-base-optimized.
// Data is the backend's own state (pointers, size, growth_left, ...).
template <typename Allocator, typename Hasher, typename Eq, typename Data>
class table_storage
{
    using eq_pair = _priv::compressed_pair<Eq, Data>;
    using hasher_pair = _priv::compressed_pair<Hasher, eq_pair>;

    _priv::compressed_pair<Allocator, hasher_pair> m_pair;

public:

    //=========================================================================
    // constructors
    //=========================================================================

    table_storage() noexcept(noexcept(Allocator()))
        : m_pair(_priv::zero_then_variadic_args_tag{},
              _priv::zero_then_variadic_args_tag{},
              _priv::zero_then_variadic_args_tag{})
    {}

    // clang-format off

    table_storage(const Allocator& a, const Hasher& h, const Eq& e)
        : m_pair(
            _priv::one_then_variadic_args_tag{}, a,
            _priv::one_then_variadic_args_tag{}, h,
            _priv::one_then_variadic_args_tag{}, e)
    {}

    table_storage(Allocator&& a, Hasher&& h, Eq&& e) noexcept
        : m_pair(
            _priv::one_then_variadic_args_tag{}, std::move(a),
            _priv::one_then_variadic_args_tag{}, std::move(h),
            _priv::one_then_variadic_args_tag{}, std::move(e))
    {}

    // clang-format on

    //=========================================================================
    // accessors
    //=========================================================================

    Allocator& alloc() noexcept
    {
        return m_pair.first();
    }

    const Allocator& alloc() const noexcept
    {
        return m_pair.first();
    }

    Hasher& hasher() noexcept
    {
        return m_pair.second.first();
    }

    const Hasher& hasher() const noexcept
    {
        return m_pair.second.first();
    }

    Eq& eq() noexcept
    {
        return m_pair.second.second.first();
    }

    const Eq& eq() const noexcept
    {
        return m_pair.second.second.first();
    }

    Data& data() noexcept
    {
        return m_pair.second.second.second;
    }

    const Data& data() const noexcept
    {
        return m_pair.second.second.second;
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
    // swap
    //=========================================================================

    // swaps allocator, hasher, eq and data together
    void swap(table_storage& other) noexcept
    {
        vx::swap(m_pair, other.m_pair);
    }
};

} // namespace _hash_priv
} // namespace vx
