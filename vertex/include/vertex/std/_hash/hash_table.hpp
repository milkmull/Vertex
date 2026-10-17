#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/expected.hpp"

#include "hash_common.hpp" // make_void

namespace vx {
namespace _hash_priv {

template <typename T, typename = void>
struct is_transparent_fn : std::false_type
{};

template <typename T>
struct is_transparent_fn<T, type_traits::void_t<typename T::is_transparent>> : std::true_type
{};

template <typename P, typename = void>
struct policy_mapped : std::false_type
{
    using type = void;
};

template <typename P>
struct policy_mapped<P, type_traits::void_t<typename P::mapped_type>> : std::true_type
{
    using type = typename P::mapped_type;
};

template <typename Backend>
class hash_table
{
    using policy = typename Backend::slot_policy;
    using mapped_info = policy_mapped<policy>;
    using map_tag = std::integral_constant<bool, mapped_info::value>;

public:

    using backend_type = Backend;
    using key_type = typename Backend::key_type;
    using mapped_type = typename mapped_info::type; // void for sets
    using value_type = typename Backend::value_type;
    using size_type = size_t;
    using hasher_type = typename Backend::hasher_type;
    using eq_type = typename Backend::eq_type;
    using allocator_type = typename Backend::allocator_type;
    using iterator = typename Backend::iterator;
    using const_iterator = typename Backend::const_iterator;

    struct insert_result
    {
        iterator it;
        bool inserted;
    };

private:

    Backend m_b;

    //=========================================================================
    // construction of a new slot (the only place that knows set vs map)
    //=========================================================================

    template <typename Q, typename... Args>
    expected<iterator, error> construct_new(std::false_type, size_t hash, Q&& key, Args&&...)
    {
        static_assert(sizeof...(Args) == 0, "a set takes only the key");
        return m_b.emplace_new(hash, std::forward<Q>(key));
    }

    template <typename Q, typename... Args>
    expected<iterator, error> construct_new(std::true_type, size_t hash, Q&& key, Args&&... args)
    {
        return m_b.emplace_new(hash, std::piecewise_construct, std::forward_as_tuple(std::forward<Q>(key)), std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename Q, typename... Args>
    iterator construct_new_capacity(std::false_type, size_t hash, Q&& key, Args&&...)
    {
        static_assert(sizeof...(Args) == 0, "a set takes only the key");
        return m_b.emplace_new_capacity(hash, std::forward<Q>(key));
    }

    template <typename Q, typename... Args>
    iterator construct_new_capacity(std::true_type, size_t hash, Q&& key, Args&&... args)
    {
        return m_b.emplace_new_capacity(hash, std::piecewise_construct, std::forward_as_tuple(std::forward<Q>(key)), std::forward_as_tuple(std::forward<Args>(args)...));
    }

    //=========================================================================
    // splitting an inserted value into (key, mapped args...)
    //=========================================================================

    // set: the value is the key
    template <typename F, typename T>
    decltype(auto) visit_value(std::false_type, F&& f, T&& v)
    {
        return f(std::forward<T>(v));
    }

    // map: std::pair or a slot
    template <typename F, typename A, typename B>
    decltype(auto) visit_value(std::true_type, F&& f, const std::pair<A, B>& p)
    {
        return f(p.first, p.second);
    }

    template <typename F, typename A, typename B>
    decltype(auto) visit_value(std::true_type, F&& f, std::pair<A, B>&& p)
    {
        return f(std::move(p.first), std::move(p.second));
    }

    template <typename F>
    decltype(auto) visit_value(std::true_type, F&& f, const value_type& s)
    {
        return f(s.key(), s.value);
    }

    template <typename F>
    decltype(auto) visit_value(std::true_type, F&& f, value_type&& s)
    {
        return f(s.key(), std::move(s.value)); // the key is const: copied
    }

    //=========================================================================
    // equality helper
    //=========================================================================

    static bool slot_equal(std::false_type, const value_type&, const value_type&)
    {
        return true;
    }

    static bool slot_equal(std::true_type, const value_type& a, const value_type& b)
    {
        return a.value == b.value;
    }

    explicit hash_table(Backend&& b) noexcept
        : m_b(std::move(b))
    {}

public:

    //=========================================================================
    // constructors
    //=========================================================================

    hash_table() = default;

    explicit hash_table(const hasher_type& h, const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(h, e, a)
    {}

    explicit hash_table(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(n, h, e, a)
    {}

    hash_table(const hash_table&) = default;
    hash_table(hash_table&&) = default;
    hash_table& operator=(const hash_table&) = default;
    hash_table& operator=(hash_table&&) = default;
    ~hash_table() = default;

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<hash_table, error> create(const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        auto r = Backend::create(h, e, a);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return hash_table(std::move(r.value()));
    }

    static expected<hash_table, error> create(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        auto r = Backend::create(n, h, e, a);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return hash_table(std::move(r.value()));
    }

    static expected<hash_table, error> create(const hash_table& other)
    {
        auto r = Backend::create(other.m_b);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return hash_table(std::move(r.value()));
    }

    success assign(const hash_table& other)
    {
        return m_b.assign(other.m_b);
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

    Backend& backend() noexcept
    {
        return m_b;
    }

    const Backend& backend() const noexcept
    {
        return m_b;
    }

    template <typename Q>
    size_t hash_of(const Q& key) const noexcept
    {
        return m_b.hash_of(key);
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
    // size and memory
    //=========================================================================

    bool empty() const noexcept
    {
        return m_b.size() == 0;
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

    void clear() noexcept
    {
        m_b.clear();
    }

    success reserve(size_type n)
    {
        return m_b.reserve(n);
    }

    success shrink_to_fit()
    {
        return m_b.shrink_to_fit();
    }

    void swap(hash_table& other) noexcept
    {
        m_b.swap(other.m_b);
    }

    //=========================================================================
    // lookup
    //=========================================================================

    template <typename Q>
    iterator find_with_hash(const Q& key, size_t hash) noexcept
    {
        return m_b.find_with_hash(key, hash);
    }

    template <typename Q>
    const_iterator find_with_hash(const Q& key, size_t hash) const noexcept
    {
        return m_b.find_with_hash(key, hash);
    }

    // Q may be any type the hasher and eq accept. With a transparent hasher
    // and eq (is_transparent) no temporary key is built.
    template <typename Q>
    iterator find(const Q& key) noexcept
    {
        return m_b.find_with_hash(key, m_b.hash_of(key));
    }

    template <typename Q>
    const_iterator find(const Q& key) const noexcept
    {
        return m_b.find_with_hash(key, m_b.hash_of(key));
    }

    template <typename Q>
    bool contains(const Q& key) const noexcept
    {
        return find(key) != end();
    }

    template <typename Q>
    bool contains_with_hash(const Q& key, size_t hash) const noexcept
    {
        return find_with_hash(key, hash) != end();
    }

    // 0 or 1 (no multi containers)
    template <typename Q>
    size_type count(const Q& key) const noexcept
    {
        return contains(key) ? 1 : 0;
    }

    //=========================================================================
    // try_emplace: the primitive. hash -> find -> emplace_new.
    // The key (and mapped args) are only consumed on an actual insertion.
    //=========================================================================

    template <typename Q, typename... Args>
    expected<insert_result, error> try_emplace_with_hash(size_t hash, Q&& key, Args&&... args)
    {
        const iterator it = m_b.find_with_hash(key, hash);
        if (it != m_b.end())
        {
            return insert_result{ it, false };
        }

        auto r = construct_new(map_tag{}, hash, std::forward<Q>(key), std::forward<Args>(args)...);
        VX_RET_UNEXPECTED_ERR_IF(!r, r.error());
        return insert_result{ r.value(), true };
    }

    template <typename Q, typename... Args>
    expected<insert_result, error> try_emplace(Q&& key, Args&&... args)
    {
        const size_t hash = m_b.hash_of(key);
        return try_emplace_with_hash(hash, std::forward<Q>(key), std::forward<Args>(args)...);
    }

    // requires capacity() > size() (see reserve)
    template <typename Q, typename... Args>
    insert_result try_emplace_capacity_with_hash(size_t hash, Q&& key, Args&&... args)
    {
        const iterator it = m_b.find_with_hash(key, hash);
        if (it != m_b.end())
        {
            return insert_result{ it, false };
        }
        return insert_result{ construct_new_capacity(map_tag{}, hash, std::forward<Q>(key), std::forward<Args>(args)...), true };
    }

    template <typename Q, typename... Args>
    insert_result try_emplace_capacity(Q&& key, Args&&... args)
    {
        const size_t hash = m_b.hash_of(key);
        return try_emplace_capacity_with_hash(hash, std::forward<Q>(key), std::forward<Args>(args)...);
    }

    // key known absent (asserted by the backend's debug paths only)
    template <typename Q, typename... Args>
    expected<iterator, error> emplace_unique_with_hash(size_t hash, Q&& key, Args&&... args)
    {
        VX_ASSERT(m_b.find_with_hash(key, hash) == m_b.end());
        return construct_new(map_tag{}, hash, std::forward<Q>(key), std::forward<Args>(args)...);
    }

    template <typename Q, typename... Args>
    expected<iterator, error> emplace_unique(Q&& key, Args&&... args)
    {
        const size_t hash = m_b.hash_of(key);
        return emplace_unique_with_hash(hash, std::forward<Q>(key), std::forward<Args>(args)...);
    }

    template <typename Q, typename... Args>
    iterator emplace_unique_capacity(Q&& key, Args&&... args)
    {
        const size_t hash = m_b.hash_of(key);
        VX_ASSERT(m_b.find_with_hash(key, hash) == m_b.end());
        return construct_new_capacity(map_tag{}, hash, std::forward<Q>(key), std::forward<Args>(args)...);
    }

    //=========================================================================
    // insert (set: a key. map: std::pair<K, V> or a value_type)
    //=========================================================================

    template <typename T>
    expected<insert_result, error> insert(T&& v)
    {
        return visit_value(map_tag{}, [this](auto&&... a)
            { return this->try_emplace(std::forward<decltype(a)>(a)...); }, std::forward<T>(v));
    }

    template <typename T>
    insert_result insert_capacity(T&& v)
    {
        return visit_value(map_tag{}, [this](auto&&... a)
            { return this->try_emplace_capacity(std::forward<decltype(a)>(a)...); }, std::forward<T>(v));
    }

    template <typename T>
    expected<iterator, error> insert_unique(T&& v)
    {
        return visit_value(map_tag{}, [this](auto&&... a)
            { return this->emplace_unique(std::forward<decltype(a)>(a)...); }, std::forward<T>(v));
    }

    template <typename T>
    iterator insert_unique_capacity(T&& v)
    {
        return visit_value(map_tag{}, [this](auto&&... a)
            { return this->emplace_unique_capacity(std::forward<decltype(a)>(a)...); }, std::forward<T>(v));
    }

    // stops at the first failure, earlier elements stay inserted
    template <typename It>
    success insert_range(It first, It last)
    {
        for (; first != last; ++first)
        {
            const auto r = insert(*first);
            VX_RET_ERR_IF(!r, r.error());
        }
        return success{};
    }

    // set only: builds the key from args, then inserts it
    template <typename... Args, typename P = policy, VX_REQUIRES(!policy_mapped<P>::value)>
    expected<insert_result, error> emplace(Args&&... args)
    {
        key_type key(std::forward<Args>(args)...);
        return try_emplace(std::move(key));
    }

    //=========================================================================
    // map only
    //=========================================================================

    template <typename Q, typename M, typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    expected<insert_result, error> insert_or_assign(Q&& key, M&& m)
    {
        auto r = try_emplace(std::forward<Q>(key), std::forward<M>(m)); // m is untouched if the key exists
        if (r && !r.value().inserted)
        {
            r.value().it->value = std::forward<M>(m);
        }
        return r;
    }

    // Inserts a default-constructed value if the key is absent. VX_VERIFYs on
    // out-of-memory, like the constructors. Use try_emplace to handle it.
    template <typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    typename P::mapped_type& operator[](const key_type& key)
    {
        auto r = try_emplace(key);
        VX_VERIFY(r);
        return r.value().it->value;
    }

    template <typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    typename P::mapped_type& operator[](key_type&& key)
    {
        auto r = try_emplace(std::move(key));
        VX_VERIFY(r);
        return r.value().it->value;
    }

    template <typename Q, typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    expected<typename P::mapped_type&, error> at(const Q& key)
    {
        const iterator it = find(key);
        VX_RET_UNEXPECTED_ERR_IF(it == end(), err::out_of_range);
        return it->value;
    }

    template <typename Q, typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    expected<const typename P::mapped_type&, error> at(const Q& key) const
    {
        const const_iterator it = find(key);
        VX_RET_UNEXPECTED_ERR_IF(it == end(), err::out_of_range);
        return it->value;
    }

    template <typename Q, typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    typename P::mapped_type& at_unchecked(const Q& key) noexcept
    {
        const iterator it = find(key);
        VX_ASSERT(it != end());
        return it->value;
    }

    template <typename Q, typename P = policy, VX_REQUIRES(policy_mapped<P>::value)>
    const typename P::mapped_type& at_unchecked(const Q& key) const noexcept
    {
        const const_iterator it = find(key);
        VX_ASSERT(it != end());
        return it->value;
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
        m_b.erase_at(pos);
        return next;
    }

    iterator erase(const_iterator first, const_iterator last) noexcept
    {
        while (first != last)
        {
            first = erase(first);
        }
        return m_b.unconst(last);
    }

    // returns the number of erased elements (0 or 1)
    size_type erase(const key_type& key) noexcept
    {
        return erase_with_hash(key, m_b.hash_of(key));
    }

    // heterogeneous: only with a transparent hasher and eq, and never for iterators
    template <typename Q, typename H = hasher_type, typename E = eq_type, VX_REQUIRES(is_transparent_fn<H>::value&& is_transparent_fn<E>::value && !std::is_convertible<Q, iterator>::value && !std::is_convertible<Q, const_iterator>::value)>
    size_type erase(const Q& key) noexcept
    {
        return erase_with_hash(key, m_b.hash_of(key));
    }

    template <typename Q>
    size_type erase_with_hash(const Q& key, size_t hash) noexcept
    {
        const const_iterator it = m_b.find_with_hash(key, hash);
        if (it == m_b.end())
        {
            return 0;
        }
        m_b.erase_at(it);
        return 1;
    }

    // One linear pass. pred receives the slot (the key for sets, a slot with
    // key() and value for maps). Returns the number of erased elements.
    template <typename Pred>
    size_type erase_if(Pred pred)
    {
        size_type n = 0;
        for (iterator it = m_b.begin(); it != m_b.end();)
        {
            iterator next = it;
            ++next;
            if (pred(*it))
            {
                m_b.erase_at(it);
                ++n;
            }
            it = next;
        }
        return n;
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const hash_table& a, const hash_table& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (const_iterator it = a.begin(); it != a.end(); ++it)
        {
            const const_iterator jt = b.m_b.find(policy::key(*it));
            if (jt == b.end() || !slot_equal(map_tag{}, *it, *jt))
            {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const hash_table& a, const hash_table& b)
    {
        return !(a == b);
    }
};

template <typename B>
void swap(hash_table<B>& a, hash_table<B>& b) noexcept
{
    a.swap(b);
}

} // namespace _hash_priv
} // namespace vx
