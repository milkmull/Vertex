#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/expected.hpp"

namespace vx {
namespace _hash_priv {

//=============================================================================
// heterogeneous lookup support
//
// key_arg<Q> is Q when BOTH hasher and eq are transparent, key_type otherwise.
// This stops a non-transparent table from silently building a temporary key
// from an unrelated Q.
//=============================================================================

template <typename T, typename = void>
struct has_is_transparent : std::false_type
{};

template <typename T>
struct has_is_transparent<T, std::void_t<typename T::is_transparent>> : std::true_type
{};

template <bool Transparent>
struct key_arg_impl
{
    template <typename Q, typename K>
    using type = K;
};

template <>
struct key_arg_impl<true>
{
    template <typename Q, typename K>
    using type = Q;
};

template <typename Backend>
class basic_hash_set
{
    //=========================================================================
    // member types
    //=========================================================================

public:

    using backend_type = Backend;
    using key_type = typename Backend::key_type;
    using value_type = typename Backend::value_type;
    using size_type = size_t;
    using hasher_type = typename Backend::hasher_type;
    using eq_type = typename Backend::eq_type;
    using allocator_type = typename Backend::allocator_type;
    using iterator = typename Backend::iterator;
    using const_iterator = typename Backend::const_iterator;

    using insert_result = typename Backend::insert_result;

private:

    static constexpr bool is_transparent_v =
        has_is_transparent<hasher_type>::value && has_is_transparent<eq_type>::value;

    template <typename Q>
    using key_arg = typename key_arg_impl<is_transparent_v>::template type<Q, key_type>;

    Backend m_b;

    explicit basic_hash_set(Backend&& b) noexcept
        : m_b(std::move(b))
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    basic_hash_set() = default;

    explicit basic_hash_set(const hasher_type& h, const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(h, e, a)
    {}

    explicit basic_hash_set(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(n, h, e, a)
    {}

    basic_hash_set(const basic_hash_set&) = default;
    basic_hash_set(basic_hash_set&&) noexcept = default;
    basic_hash_set& operator=(const basic_hash_set&) = default;
    basic_hash_set& operator=(basic_hash_set&&) noexcept = default;
    ~basic_hash_set() = default;

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<basic_hash_set, error> create(const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        auto r = Backend::create(h, e, a);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return basic_hash_set(std::move(r.value()));
    }

    static expected<basic_hash_set, error> create(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        auto r = Backend::create(n, h, e, a);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return basic_hash_set(std::move(r.value()));
    }

    static expected<basic_hash_set, error> create(const basic_hash_set& other)
    {
        auto r = Backend::create(other.m_b);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return basic_hash_set(std::move(r.value()));
    }

    static expected<basic_hash_set, error> create(basic_hash_set&& other) noexcept
    {
        return basic_hash_set(std::move(other.m_b));
    }

    //=========================================================================
    // observers
    //=========================================================================

    allocator_type get_allocator() const noexcept
    {
        return m_b.get_allocator();
    }

    hasher_type hash_function() const
    {
        return m_b.hash_function();
    }

    eq_type key_eq() const
    {
        return m_b.key_eq();
    }

    //=========================================================================
    // assign (fallible; operator= is the defaulted one above)
    //=========================================================================

    success assign(const basic_hash_set& other)
    {
        return m_b.assign(other.m_b);
    }

    success assign(basic_hash_set&& other) noexcept
    {
        m_b = std::move(other.m_b);
        return success{};
    }

    success assign(std::initializer_list<value_type> init)
    {
        return m_b.assign(init);
    }

    //=========================================================================
    // hashing
    //=========================================================================

    template <typename Q = key_type>
    size_t hash_of(const key_arg<Q>& key) const noexcept
    {
        return m_b.hash_of(key);
    }

    //=========================================================================
    // lookup
    //
    // Q may be any type the hasher and eq accept, but only when both are
    // transparent. Otherwise the parameter is const key_type&.
    //
    // *_with_hash: the hash MUST equal hash_of(key) (asserted in debug).
    //=========================================================================

    template <typename Q = key_type>
    iterator find(const key_arg<Q>& key) noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q = key_type>
    iterator find_with_hash(const key_arg<Q>& key, size_t hash) noexcept
    {
        VX_ASSERT(hash == hash_of(key));
        return m_b.find_with_hash(key, hash);
    }

    template <typename Q = key_type>
    const_iterator find(const key_arg<Q>& key) const noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q = key_type>
    const_iterator find_with_hash(const key_arg<Q>& key, size_t hash) const noexcept
    {
        VX_ASSERT(hash == hash_of(key));
        return m_b.find_with_hash(key, hash);
    }

    template <typename Q = key_type>
    bool contains(const key_arg<Q>& key) const noexcept
    {
        return find(key) != end();
    }

    template <typename Q = key_type>
    bool contains_with_hash(const key_arg<Q>& key, size_t hash) const noexcept
    {
        return find_with_hash(key, hash) != end();
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return m_b.begin();
    }

    const_iterator begin() const noexcept
    {
        return m_b.begin();
    }

    const_iterator cbegin() const noexcept
    {
        return m_b.begin();
    }

    iterator end() noexcept
    {
        return m_b.end();
    }

    const_iterator end() const noexcept
    {
        return m_b.end();
    }

    const_iterator cend() const noexcept
    {
        return m_b.end();
    }

    //=========================================================================
    // memory
    //=========================================================================

    void clear() noexcept
    {
        m_b.clear();
    }

    void clear_and_deallocate() noexcept
    {
        m_b.clear_and_deallocate();
    }

    success reserve(size_type n)
    {
        return m_b.reserve(n);
    }

    success shrink_to_fit()
    {
        return m_b.shrink_to_fit();
    }

    void swap(basic_hash_set& other) noexcept
    {
        m_b.swap(other.m_b);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type size() const noexcept
    {
        return m_b.size();
    }

    static constexpr size_type max_size() noexcept
    {
        return Backend::max_size();
    }

    // elements that fit before the next growth
    size_type capacity() const noexcept
    {
        return m_b.capacity();
    }

    //=========================================================================
    // emplace
    //
    // Construction always goes through key_type (we need a key to hash).
    // Backend contract assumed:
    //   emplace_new(hash, key)          -> expected<iterator, error>
    //   emplace_new_capacity(hash, key) -> iterator
    //=========================================================================

    template <typename... Args>
    expected<insert_result, error> emplace(Args&&... args)
    {
        key_type key(std::forward<Args>(args)...);
        const size_t hash = hash_of(key);

        const iterator it = find_with_hash(key, hash);
        if (it != end())
        {
            return insert_result{ it, false };
        }

        return m_b.emplace_new(hash, std::move(key));
    }

    template <typename... Args>
    expected<iterator, error> emplace_assume_unique(Args&&... args)
    {
        key_type key(std::forward<Args>(args)...);
        const size_t hash = hash_of(key);
        VX_ASSERT(!contains_with_hash(key, hash));
        return m_b.emplace_new(hash, std::move(key));
    }

    template <typename... Args>
    insert_result emplace_assume_capacity(Args&&... args)
    {
        key_type key(std::forward<Args>(args)...);
        const size_t hash = hash_of(key);

        const iterator it = find_with_hash(key, hash);
        if (it != end())
        {
            return insert_result{ it, false };
        }

        return insert_result{ m_b.emplace_new_capacity(hash, std::move(key)), true };
    }

    template <typename... Args>
    iterator emplace_assume_unique_and_capacity(Args&&... args)
    {
        key_type key(std::forward<Args>(args)...);
        const size_t hash = hash_of(key);
        VX_ASSERT(!contains_with_hash(key, hash));
        return m_b.emplace_new_capacity(hash, std::move(key));
    }

    //=========================================================================
    // insert
    //=========================================================================

    expected<insert_result, error> insert(const value_type& v)
    {
        return emplace(v);
    }

    expected<insert_result, error> insert(value_type&& v)
    {
        return emplace(std::move(v));
    }

    expected<iterator, error> insert_assume_unique(const value_type& v)
    {
        return emplace_assume_unique(v);
    }

    expected<iterator, error> insert_assume_unique(value_type&& v)
    {
        return emplace_assume_unique(std::move(v));
    }

    insert_result insert_assume_capacity(const value_type& v)
    {
        return emplace_assume_capacity(v);
    }

    insert_result insert_assume_capacity(value_type&& v)
    {
        return emplace_assume_capacity(std::move(v));
    }

    iterator insert_assume_unique_and_capacity(const value_type& v)
    {
        return emplace_assume_unique_and_capacity(v);
    }

    iterator insert_assume_unique_and_capacity(value_type&& v)
    {
        return emplace_assume_unique_and_capacity(std::move(v));
    }

    //=========================================================================
    // merge
    //=========================================================================

    success merge(basic_hash_set& source)
    {
        return success{};
    }

    success merge(basic_hash_set&& source)
    {
        return merge(source);
    }

    //=========================================================================
    // erase
    //=========================================================================

    // Void primitive. Invalidates pos only. No other element moves.
    void erase_at(const_iterator pos) noexcept
    {
        m_b.erase_at(pos);
    }

    // Returns the element after pos. The successor is found BEFORE the erase,
    // which is valid for every backend (chain frees the node, open leaves a hole).
    iterator erase(const_iterator pos) noexcept
    {
        iterator next = m_b.unconst(pos);
        ++next;
        erase_at(pos);
        return next;
    }

    // Needed so a transparent erase(key) never hijacks erase(iterator).
    iterator erase(iterator pos) noexcept
    {
        return erase(const_iterator(pos));
    }

    // returns true if an element was erased
    template <typename Q = key_type>
    bool erase(const key_arg<Q>& key) noexcept
    {
        return erase_with_hash(key, hash_of(key));
    }

    template <typename Q = key_type>
    bool erase_with_hash(const key_arg<Q>& key, size_t hash) noexcept
    {
        const iterator it = find_with_hash(key, hash);
        if (it == end())
        {
            return false;
        }

        erase_at(it);
        return true;
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const basic_hash_set& a, const basic_hash_set& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (const_iterator it = a.begin(); it != a.end(); ++it)
        {
            if (!b.contains(*it))
            {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const basic_hash_set& a, const basic_hash_set& b)
    {
        return !(a == b);
    }
};

template <typename B>
void swap(basic_hash_set<B>& a, basic_hash_set<B>& b) noexcept
{
    a.swap(b);
}

} // namespace _hash_priv
} // namespace vx


namespace vx {

template <typename K, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<K>, typename Traits = _hash_priv::default_table_traits>
using unordered_set = _hash_priv::basic_hash_set<_hash_priv::open_table<_hash_priv::set_policy<K>, Hash, Eq, Alloc, Traits>>;

template <typename K, typename Hash = default_hash<K>, typename Eq = std::equal_to<K>, typename Alloc = mem::default_allocator<K>, typename Traits = _hash_priv::default_chain_traits>
using chain_unordered_set = _hash_priv::basic_hash_set<_hash_priv::chain_table<_hash_priv::set_policy<K>, Hash, Eq, Alloc, Traits>>;

} // namespace vx
