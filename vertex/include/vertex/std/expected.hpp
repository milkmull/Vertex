#pragma once

#include "vertex/std/variant_storage.hpp"

namespace vx {

//=============================================================================
// unexpected
//=============================================================================

template <typename E>
struct unexpected
{
    E error;
};

template <typename E>
auto make_unexpected(E&& error)
{
    return unexpected<std::decay_t<E>>{ std::forward<E>(error) };
}

namespace _expected_priv {

template <typename U>
struct is_unexpected : std::false_type
{};

template <typename E2>
struct is_unexpected<unexpected<E2>> : std::true_type
{};

// Maps a type to what actually gets placed in the sink.
// Ordinary T -> T. Reference T& -> T*, since aligned_storage (like any
// object-based sink) can only construct/hold actual objects.
template <typename T>
struct storage_traits
{
    using stored_type = T;

    template <typename Storage, typename... Args>
    static void construct(Storage& s, Args&&... args)
    {
        s.template construct<stored_type>(std::forward<Args>(args)...);
    }

    template <typename Storage>
    static void destroy(Storage& s) noexcept
    {
        s.template destroy<stored_type>();
    }

    template <typename Storage>
    static T& get(Storage& s) noexcept
    { return s.template get<stored_type>(); }

    template <typename Storage>
    static const T& get(const Storage& s) noexcept
    { return s.template get<stored_type>(); }

    template <typename Storage>
    static T&& get_rv(Storage& s) noexcept
    { return std::move(s.template get<stored_type>()); }
};

template <typename T>
struct storage_traits<T&>
{
    using stored_type = T*;

    template <typename Storage>
    static void construct(Storage& s, T& ref) noexcept
    {
        s.template construct<stored_type>(std::addressof(ref));
    }

    template <typename Storage>
    static void destroy(Storage&) noexcept
    {
        // trivial: nothing owned, nothing to run
    }

    template <typename Storage>
    static T& get(Storage& s) noexcept
    { return *s.template get<stored_type>(); }

    template <typename Storage>
    static T& get(const Storage& s) noexcept
    { return *s.template get<stored_type>(); }

    template <typename Storage>
    static T& get_rv(Storage& s) noexcept
    { return get(s); } // nothing to "move" for a reference
};

template <typename T, typename E>
struct expected_storage
{
    using value_traits = storage_traits<T>;
    using error_traits = storage_traits<E>;

    using value_stored = typename value_traits::stored_type;
    using error_stored = typename error_traits::stored_type;

    using type = variant_storage<value_stored, error_stored>;
};

//=============================================================================
// expected_storage_base
//=============================================================================

template <typename T, typename E>
struct expected_storage_traits
{
    using value_traits = storage_traits<T>;
    using error_traits = storage_traits<E>;
    using storage_type = typename expected_storage<T, E>::type;

    static constexpr bool is_trivial =
        std::is_trivially_destructible<typename value_traits::stored_type>::value &&
        std::is_trivially_destructible<typename error_traits::stored_type>::value;
};

template <typename T, typename E, bool = expected_storage_traits<T, E>::is_trivial>
struct expected_storage_base
{
    using value_traits = typename expected_storage_traits<T, E>::value_traits;
    using error_traits = typename expected_storage_traits<T, E>::error_traits;
    using storage_type = typename expected_storage_traits<T, E>::storage_type;

    storage_type m_storage;
    bool m_has_value;

    expected_storage_base() noexcept : m_has_value(false)
    {}

    explicit expected_storage_base(bool has_value) noexcept
        : m_has_value(has_value)
    {}

    // non-trivial: must actually tear down whichever alternative is active
    ~expected_storage_base()
    {
        destroy();
    }

    void destroy() noexcept
    {
        if (m_has_value)
        {
            value_traits::destroy(m_storage);
        }
        else
        {
            error_traits::destroy(m_storage);
        }
    }
};

template <typename T, typename E>
struct expected_storage_base<T, E, true>
{
    using value_traits = typename expected_storage_traits<T, E>::value_traits;
    using error_traits = typename expected_storage_traits<T, E>::error_traits;
    using storage_type = typename expected_storage_traits<T, E>::storage_type;

    storage_type m_storage;
    bool m_has_value;

    expected_storage_base() noexcept : m_has_value(false)
    {}

    explicit expected_storage_base(bool has_value) noexcept
        : m_has_value(has_value)
    {}

    // trivial: both alternatives are trivially destructible, so leaving
    // this defaulted makes expected_storage_base (and expected) trivially
    // destructible in turn
    ~expected_storage_base() = default;

    void destroy() noexcept
    {
        if (m_has_value)
        {
            value_traits::destroy(m_storage);
        }
        else
        {
            error_traits::destroy(m_storage);
        }
    }
};

} // namespace _expected_priv

//=============================================================================
// expected
//=============================================================================

template <typename T, typename E>
class expected : private _expected_priv::expected_storage_base<T, E>
{
    using base = _expected_priv::expected_storage_base<T, E>;
    using value_traits = typename base::value_traits;
    using error_traits = typename base::error_traits;
    using storage_type = typename base::storage_type;

public:

    //=====================================
    // constructors
    //=====================================

    template <
        typename U = T,
        VX_REQUIRES(!std::is_same<expected, typename std::decay<U>::type>::value &&
            !_expected_priv::is_unexpected<typename std::decay<U>::type>::value &&
            std::is_constructible<T, U&&>::value)>
    expected(U&& value) noexcept
        : base(true)
    {
        value_traits::construct(base::m_storage, std::forward<U>(value));
    }

    expected(const unexpected<E>& unex) noexcept
        : base(false)
    {
        error_traits::construct(base::m_storage, unex.error);
    }

    expected(unexpected<E>&& unex) noexcept
        : base(false)
    {
        error_traits::construct(base::m_storage, std::move(unex.error));
    }

    expected(const expected& other) noexcept
        : base(other.m_has_value)
    {
        if (base::m_has_value)
        {
            value_traits::construct(base::m_storage, other.value());
        }
        else
        {
            error_traits::construct(base::m_storage, other.error());
        }
    }

    expected(expected&& other) noexcept
        : base(other.m_has_value)
    {
        if (base::m_has_value)
        {
            value_traits::construct(base::m_storage, std::move(other).value());
        }
        else
        {
            error_traits::construct(base::m_storage, std::move(other.error()));
        }
    }

    //=====================================
    // destructor / assignment
    //=====================================

    // defaulted so triviality is inherited from expected_storage_base:
    // when both T and E are trivially destructible, this destructor (and
    // therefore expected itself) is trivial too.
    ~expected() = default;

    expected& operator=(const expected& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        base::destroy();

        base::m_has_value = other.m_has_value;
        if (base::m_has_value)
        {
            value_traits::construct(base::m_storage, other.value());
        }
        else
        {
            error_traits::construct(base::m_storage, other.error());
        }

        return *this;
    }

    expected& operator=(expected&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        base::destroy();

        base::m_has_value = other.m_has_value;
        if (base::m_has_value)
        {
            value_traits::construct(base::m_storage, std::move(other).value());
        }
        else
        {
            error_traits::construct(base::m_storage, std::move(other.error()));
        }

        return *this;
    }

    //=====================================
    // observers / accessors
    //=====================================

    bool has_value() const noexcept
    {
        return base::m_has_value;
    }

    explicit operator bool() const noexcept
    {
        return base::m_has_value;
    }

    T& value() & noexcept
    {
        VX_VERIFY(base::m_has_value);
        return value_traits::get(base::m_storage);
    }

    const T& value() const& noexcept
    {
        VX_VERIFY(base::m_has_value);
        return value_traits::get(base::m_storage);
    }

    T&& value() && noexcept
    {
        VX_VERIFY(base::m_has_value);
        return value_traits::get_rv(base::m_storage);
    }

    E& error() & noexcept
    {
        VX_VERIFY(!base::m_has_value);
        return error_traits::get(base::m_storage);
    }

    const E& error() const& noexcept
    {
        VX_VERIFY(!base::m_has_value);
        return error_traits::get(base::m_storage);
    }

    template <typename U>
    T value_or(U&& default_value) const&
    {
        return base::m_has_value ? value_traits::get(base::m_storage) : static_cast<T>(std::forward<U>(default_value));
    }

    template <typename U>
    E error_or(U&& default_error) const&
    {
        return !base::m_has_value ? error_traits::get(base::m_storage) : static_cast<E>(std::forward<U>(default_error));
    }

private:

    void destroy() noexcept
    {
        if (m_has_value)
        {
            value_traits::destroy(m_storage);
        }
        else
        {
            error_traits::destroy(m_storage);
        }
    }

    storage_type m_storage;
    bool m_has_value;
};

} // namespace vx
