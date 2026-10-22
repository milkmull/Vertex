#pragma once

#include <initializer_list>
#include <iterator>
#include <type_traits>

#include "vertex/std/_hash/hash_common.hpp"
#include "vertex/std/expected.hpp"

namespace vx {
namespace _hash_priv {

//=========================================================================

template <typename T, typename = void>
struct has_is_transparent : std::false_type
{};

template <typename T>
struct has_is_transparent<T, type_traits::void_t<typename T::is_transparent>> : std::true_type
{};

//=========================================================================

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

template <typename Derived, typename Backend>
class hash_table_base
{
    using backend_type = Backend;
    using slot_traits = typename Backend::slot_traits;

public:

    //=========================================================================
    // member types
    //=========================================================================

    using key_type = typename Backend::key_type;
    using value_type = typename Backend::value_type;

    using hasher_type = typename Backend::hasher_type;
    using eq_type = typename Backend::eq_type;
    using allocator_type = typename Backend::allocator_type;

    using size_type = size_t;
    using insert_result = typename Backend::insert_result;

    using iterator = typename Backend::iterator;
    using const_iterator = typename Backend::const_iterator;

protected:

    static constexpr bool is_transparent =
        has_is_transparent<hasher_type>::value && has_is_transparent<eq_type>::value;

    template <typename Q>
    using key_arg = typename key_arg_impl<is_transparent>::template type<Q, key_type>;

    Backend m_b;

public:

    //=========================================================================
    // constructors
    //=========================================================================

private:

    explicit hash_table_base(const Backend& b) noexcept
        : m_b(b)
    {}

    explicit hash_table_base(Backend&& b) noexcept
        : m_b(std::move(b))
    {}

public:

    hash_table_base() = default;

    explicit hash_table_base(const hasher_type& h, const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(h, e, a)
    {}

    explicit hash_table_base(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(n, h, e, a)
    {}

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    hash_table_base(IT first, IT last, size_type n = 0, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : m_b(h, e, a)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        if (n != 0)
        {
            const auto r = reserve(n);
            VX_VERIFY(r);
        }
        const auto r = insert(first, last);
        VX_VERIFY(r);
    }

    hash_table_base(std::initializer_list<value_type> init, size_type n = 0, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
        : hash_table_base(init.begin(), init.end(), n, h, e, a)
    {}

    hash_table_base(const hash_table_base&) = default;
    hash_table_base(hash_table_base&&) noexcept = default;
    hash_table_base& operator=(const hash_table_base&) = default;
    hash_table_base& operator=(hash_table_base&&) noexcept = default;
    ~hash_table_base() = default;

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<Derived, error> create(const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        return Derived(h, e, a);
    }

    static expected<Derived, error> create(size_type n, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        Derived d(h, e, a);
        const auto ok = d.reserve(n);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return d;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static expected<Derived, error> create(IT first, IT last, size_type n = 0, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        Derived d(h, e, a);

        if (n != 0)
        {
            const auto reserved = d.reserve(n);
            VX_RET_UNEXPECTED_ERR_IF(!reserved, reserved);
        }

        const auto ok = d.insert(first, last);
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return d;
    }

    static expected<Derived, error> create(std::initializer_list<value_type> init, size_type n = 0, const hasher_type& h = hasher_type(), const eq_type& e = eq_type(), const allocator_type& a = allocator_type())
    {
        return create(init.begin(), init.end(), n, h, e, a);
    }

    static expected<Derived, error> create(const Derived& other)
    {
        const auto& base = static_cast<const hash_table_base&>(other);
        auto b = Backend::create(base.m_b);
        VX_RET_UNEXPECTED_ERR_IF(!b, b.error());
        return Derived(std::move(b.value()));
    }

    static expected<Derived, error> create(const Derived& other, const allocator_type& a)
    {
        Derived d(other.hash_function(), other.key_eq(), a);
        const auto ok = d.insert(other.begin(), other.end());
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        return d;
    }

    static expected<Derived, error> create(Derived&& other) noexcept
    {
        return Derived(std::move(other));
    }

public:

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
    // assign
    //=========================================================================

    success assign(const hash_table_base& other)
    {
        return m_b.assign(other.m_b);
    }

    success assign(hash_table_base&& other) noexcept
    {
        m_b = std::move(other.m_b);
        return success{};
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success assign(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
        auto tmp = create(first, last, 0, hash_function(), key_eq(), get_allocator());
        VX_RET_ERR_IF(!tmp, tmp.error());

        swap(tmp.value());
        return success{};
    }

    success assign(std::initializer_list<value_type> init)
    {
        return assign(init.begin(), init.end());
    }

    Derived& operator=(std::initializer_list<value_type> init)
    {
        const auto ok = assign(init);
        VX_VERIFY(ok);
        return static_cast<Derived&>(*this);
    }

protected:

    //=========================================================================
    // hashing
    //=========================================================================

    template <typename Q = key_type>
    size_t hash_of(const key_arg<Q>& key) const noexcept
    {
        return m_b.hash_of(key);
    }

    //=========================================================================
    // lookup primitives (protected so derived front ends can hash once)
    //=========================================================================

    template <typename Q = key_type>
    iterator find_with_hash(const key_arg<Q>& key, size_t hash) noexcept
    {
        VX_ASSERT(hash == hash_of(key));
        return m_b.find_with_hash(key, hash);
    }

    template <typename Q = key_type>
    const_iterator find_with_hash(const key_arg<Q>& key, size_t hash) const noexcept
    {
        VX_ASSERT(hash == hash_of(key));
        return m_b.find_with_hash(key, hash);
    }

    template <typename Q = key_type>
    bool contains_with_hash(const key_arg<Q>& key, size_t hash) const noexcept
    {
        return find_with_hash(key, hash) != end();
    }

public:

    template <typename Q = key_type>
    iterator find(const key_arg<Q>& key) noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q = key_type>
    const_iterator find(const key_arg<Q>& key) const noexcept
    {
        return find_with_hash(key, hash_of(key));
    }

    template <typename Q = key_type>
    bool contains(const key_arg<Q>& key) const noexcept
    {
        return find(key) != end();
    }

    template <typename Q = key_type>
    size_type count(const key_arg<Q>& key) const noexcept
    {
        return contains(key) ? 1 : 0;
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

    void swap(hash_table_base& other) noexcept
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

    constexpr size_type max_size() const noexcept
    {
        return m_b.max_size();
    }

    size_type capacity() const noexcept
    {
        return m_b.capacity();
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename... Args>
    expected<insert_result, error> emplace(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        const key_type& key = slot_traits::key(val);
        const size_t hash = hash_of(key);

        const iterator it = find_with_hash(key, hash);
        if (it != end())
        {
            return insert_result{ it, false };
        }

        const auto ok = m_b.emplace_new(hash, std::move(val));
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());
        return insert_result{ ok.value(), true };
    }

    template <typename... Args>
    expected<iterator, error> emplace_assume_unique(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        const key_type& key = slot_traits::key(val);
        const size_t hash = hash_of(key);

        VX_ASSERT(!contains_with_hash(key, hash));
        return m_b.emplace_new(hash, std::move(val));
    }

    template <typename... Args>
    insert_result emplace_assume_capacity(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        const key_type& key = slot_traits::key(val);
        const size_t hash = hash_of(key);

        const iterator it = find_with_hash(key, hash);
        if (it != end())
        {
            return insert_result{ it, false };
        }

        return insert_result{ m_b.emplace_new_assume_capacity(hash, std::move(val)), true };
    }

    template <typename... Args>
    iterator emplace_assume_unique_and_capacity(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        const key_type& key = slot_traits::key(val);
        const size_t hash = hash_of(key);

        VX_ASSERT(!contains_with_hash(key, hash));
        return m_b.emplace_new_assume_capacity(hash, std::move(val));
    }

    //=========================================================================
    // insert
    //=========================================================================

    expected<insert_result, error> insert(const value_type& val)
    {
        return insert_value(val);
    }

    expected<insert_result, error> insert(value_type&& val)
    {
        return insert_value(std::move(val));
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    success insert(IT first, IT last)
    {
        VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

        VX_IF_CONSTEXPR (type_traits::is_forward_iterator<IT>::value)
        {
            const auto count = vx::distance(first, last);
            const auto ok = reserve(size() + static_cast<size_type>(count));
            VX_RET_ERR_IF(!ok, ok);
        }

        for (; first != last; ++first)
        {
            const auto r = insert_value(value_type(*first));
            VX_RET_ERR_IF(!r, r.error());
        }

        return success{};
    }

    success insert(std::initializer_list<value_type> init)
    {
        return insert(init.begin(), init.end());
    }

    expected<iterator, error> insert_assume_unique(const value_type& val)
    {
        return emplace_assume_unique(val);
    }

    expected<iterator, error> insert_assume_unique(value_type&& val)
    {
        return emplace_assume_unique(std::move(val));
    }

    insert_result insert_assume_capacity(const value_type& val)
    {
        return emplace_assume_capacity(val);
    }

    insert_result insert_assume_capacity(value_type&& val)
    {
        return emplace_assume_capacity(std::move(val));
    }

    iterator insert_assume_unique_and_capacity(const value_type& val)
    {
        return emplace_assume_unique_and_capacity(val);
    }

    iterator insert_assume_unique_and_capacity(value_type&& val)
    {
        return emplace_assume_unique_and_capacity(std::move(val));
    }

private:

    template <typename V>
    expected<insert_result, error> insert_value(V&& val)
    {
        const key_type& key = slot_traits::key(val);
        const size_t hash = hash_of(key);

        const iterator it = find_with_hash(key, hash);
        if (it != end())
        {
            return insert_result{ it, false };
        }

        const auto ok = m_b.emplace_new(hash, value_type(std::forward<V>(val)));
        VX_RET_UNEXPECTED_ERR_IF(!ok, ok.error());
        return insert_result{ ok.value(), true };
    }

    //=========================================================================
    // merge
    //=========================================================================

public:

    template <typename D2, typename B2, VX_REQUIRES(std::is_same<typename B2::value_type, value_type>::value)>
    success merge(hash_table_base<D2, B2>& source)
    {
        if (static_cast<const void*>(&source) == static_cast<const void*>(this))
        {
            return success{};
        }

        const auto ok = reserve(size() + source.size());
        VX_RET_ERR_IF(!ok, ok);

        using src_const_iterator = typename hash_table_base<D2, B2>::const_iterator;

        for (auto it = source.begin(); it != source.end();)
        {
            // key and hash are taken before the element is moved out of source
            const key_type& key = slot_traits::key(*it);
            const size_t hash = hash_of(key);

            if (find_with_hash(key, hash) != end())
            {
                ++it;
                continue;
            }

            m_b.emplace_new_assume_capacity(hash, std::move(m_b.value_at(it)));

            // erase returns the successor, valid for every backend
            it = source.erase(src_const_iterator(it));
        }
        return success{};
    }

    template <typename D2, typename B2, VX_REQUIRES(std::is_same<typename B2::value_type, value_type>::value)>
    success merge(hash_table_base<D2, B2>&& source)
    {
        return merge(source);
    }

    //=========================================================================
    // erase
    //=========================================================================

protected:

    void erase_at(const_iterator pos) noexcept
    {
        m_b.erase_at(pos);
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

public:

    iterator erase(const_iterator pos) noexcept
    {
        iterator next = m_b.unconst(pos);
        ++next;
        erase_at(pos);
        return next;
    }

    template <typename Q = key_type, VX_REQUIRES((!std::is_convertible<const key_arg<Q>&, const_iterator>::value && !std::is_convertible<const key_arg<Q>&, iterator>::value))>
    bool erase(const key_arg<Q>& key) noexcept
    {
        return erase_with_hash(key, hash_of(key));
    }

    //=========================================================================
    // comparison
    //=========================================================================

private:

    bool contains_value(const_iterator it)
    {
    }

public:

    friend bool operator==(const hash_table_base& a, const hash_table_base& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (const_iterator it = a.begin(); it != a.end(); ++it)
        {
            if (!b.contains(slot_traits::key(*it)))
            {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const hash_table_base& a, const hash_table_base& b)
    {
        return !(a == b);
    }
};

} // namespace _hash_priv
} // namespace vx
