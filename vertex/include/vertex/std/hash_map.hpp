#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/_hash/hash_functions.hpp"
#include "vertex/std/_hash/hash_open.hpp"
#include "vertex/std/_hash/hash_chain.hpp"
#include "vertex/std/_hash/hash_table.hpp"
#include "vertex/std/expected.hpp"

namespace vx {
namespace _hash_priv {

template <typename Backend>
class basic_hash_map : public hash_table_base<basic_hash_map<Backend>, Backend>
{
    using base = hash_table_base<basic_hash_map, Backend>;
    using base::m_b;

public:

    using typename base::const_iterator;
    using typename base::insert_result;
    using typename base::iterator;
    using typename base::key_type;
    using typename base::size_type;
    using typename base::value_type;
    using mapped_type = typename Backend::slot_traits::mapped_type;

    // constructors (including range / initializer_list) and insert(value / range / list) come from the base
    using base::base;
    using base::operator=; // operator=(initializer_list)

    basic_hash_map() = default;
    basic_hash_map(const basic_hash_map&) = default;
    basic_hash_map(basic_hash_map&&) noexcept = default;
    basic_hash_map& operator=(const basic_hash_map&) = default;
    basic_hash_map& operator=(basic_hash_map&&) noexcept = default;
    ~basic_hash_map() = default;

    //=========================================================================
    // get
    //=========================================================================

    template <typename Q = key_type>
    mapped_type* get(const typename base::template key_arg<Q>& key) noexcept
    {
        const iterator it = this->find(key);
        return it != this->end() ? &it->second : nullptr;
    }

    template <typename Q = key_type>
    const mapped_type* get(const typename base::template key_arg<Q>& key) const noexcept
    {
        const const_iterator it = this->find(key);
        return it != this->end() ? &it->second : nullptr;
    }

    //=========================================================================
    // at
    //=========================================================================

    template <typename Q = key_type>
    expected<mapped_type&, error> at(const typename base::template key_arg<Q>& key) noexcept
    {
        mapped_type* p = get(key);
        VX_RET_UNEXPECTED_ERR_IF(!p, err::out_of_range);
        return *p;
    }

    template <typename Q = key_type>
    expected<const mapped_type&, error> at(const typename base::template key_arg<Q>& key) const noexcept
    {
        const mapped_type* p = get(key);
        VX_RET_UNEXPECTED_ERR_IF(!p, err::out_of_range);
        return *p;
    }

    //=========================================================================
    // get_or_insert
    //=========================================================================

private:

    template <typename K>
    expected<mapped_type*, error> get_or_insert_impl(K&& key)
    {
        const size_t hash = this->hash_of(key);

        const iterator found = this->find_with_hash(key, hash);
        if (found != this->end())
        {
            return &found->second;
        }

        const auto ok = m_b.emplace_new(hash, std::move(key));
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());

        const iterator it = ok.value();
        return &it->second;
    }

public:

    expected<mapped_type*, error> get_or_insert(const key_type& key)
    {
        return get_or_insert_impl(key);
    }

    expected<mapped_type*, error> get_or_insert(key_type&& key)
    {
        return get_or_insert_impl(std::move(key));
    }

    //=========================================================================
    // operator[]
    //=========================================================================

    mapped_type& operator[](const key_type& key)
    {
        const auto r = get_or_insert_impl(key);
        VX_ASSERT(static_cast<bool>(r));
        return *r.value();
    }

    mapped_type& operator[](key_type&& key)
    {
        const auto r = get_or_insert_impl(std::move(key));
        VX_ASSERT(static_cast<bool>(r));
        return *r.value();
    }

    //=========================================================================
    // try_emplace
    //=========================================================================

private:

    template <typename K, typename... Args>
    expected<insert_result, error> try_emplace_impl(K&& key, Args&&... args)
    {
        const size_t hash = this->hash_of(key);

        const iterator it = this->find_with_hash(key, hash);
        if (it != this->end())
        {
            return insert_result{ it, false };
        }

        const auto ok = m_b.emplace_new(hash, std::move(key), std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());

        return insert_result{ ok.value(), true };
    }

public:

    template <typename... Args>
    expected<insert_result, error> try_emplace(const key_type& key, Args&&... args)
    {
        return try_emplace_impl(key, std::forward<Args>(args)...);
    }

    template <typename... Args>
    expected<insert_result, error> try_emplace(key_type&& key, Args&&... args)
    {
        return try_emplace_impl(std::move(key), std::forward<Args>(args)...);
    }

    //=========================================================================
    // insert_or_assign
    //=========================================================================

private:

    template <typename K, typename M>
    expected<insert_result, error>
        insert_or_assign_impl(K&& key, M&& obj)
    {
        const size_t hash = this->hash_of(key);

        const iterator it = this->find_with_hash(key, hash);
        if (it != this->end())
        {
            it->second = std::forward<M>(obj);
            return insert_result{ it, false };
        }

        const auto ok = m_b.emplace_new(hash, std::move(key), std::move(obj));
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());

        return insert_result{ ok.value(), true };
    }

public:

    template <typename M>
    expected<insert_result, error> insert_or_assign(const key_type& key, M&& obj)
    {
        return insert_or_assign_impl(key, std::forward<M>(obj));
    }

    template <typename M>
    expected<insert_result, error> insert_or_assign(key_type&& key, M&& obj)
    {
        return insert_or_assign_impl(std::move(key), std::forward<M>(obj));
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const basic_hash_map& a, const basic_hash_map& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (const_iterator it = a.begin(); it != a.end(); ++it)
        {
            const const_iterator other = b.find(it->first);
            if (other == b.end() || !(other->second == it->second))
            {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const basic_hash_map& a, const basic_hash_map& b)
    {
        return !(a == b);
    }
};

} // namespace _hash_priv
} // namespace vx

namespace vx {

template <typename K, typename V, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<vx::pair<const K, V>>, typename Growth = _hash_priv::pow2_growth>
using flat_map = _hash_priv::basic_hash_map<_hash_priv::open_table<_hash_priv::map_policy<K, V>, Hash, Eq, Alloc, Growth>>;

template <typename K, typename V, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<vx::pair<const K, V>>, typename Growth = _hash_priv::pow2_growth>
using chain_map = _hash_priv::basic_hash_map<_hash_priv::chain_table<_hash_priv::map_policy<K, V>, Hash, Eq, Alloc, Growth>>;

template <typename K, typename V, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<vx::pair<const K, V>>, typename Growth = _hash_priv::pow2_growth>
using unordered_map = flat_map<K, V, Hash, Eq, Alloc, Growth>;

} // namespace vx
