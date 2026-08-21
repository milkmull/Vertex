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

} // namespace _expected_priv

//=============================================================================
// expected
//=============================================================================

template <typename T, typename E>
class expected
{
    using value_traits = _expected_priv::storage_traits<T>;
    using error_traits = _expected_priv::storage_traits<E>;
    using storage_type = typename _expected_priv::expected_storage<T, E>::type;

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
        : m_has_value(true)
    {
        value_traits::construct(m_storage, std::forward<U>(value));
    }

    expected(const unexpected<E>& unex) noexcept
        : m_has_value(false)
    {
        error_traits::construct(m_storage, unex.error);
    }

    expected(unexpected<E>&& unex) noexcept
        : m_has_value(false)
    {
        error_traits::construct(m_storage, std::move(unex.error));
    }

    expected(const expected& other) noexcept
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            value_traits::construct(m_storage, other.value());
        }
        else
        {
            error_traits::construct(m_storage, other.error());
        }
    }

    expected(expected&& other) noexcept
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            value_traits::construct(m_storage, std::move(other).value());
        }
        else
        {
            error_traits::construct(m_storage, std::move(other.error()));
        }
    }

    //=====================================
    // destructor / assignment
    //=====================================

    ~expected()
    { destroy(); }

    expected& operator=(const expected& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        destroy();

        m_has_value = other.m_has_value;
        if (m_has_value)
        {
            value_traits::construct(m_storage, other.value());
        }
        else
        {
            error_traits::construct(m_storage, other.error());
        }

        return *this;
    }

    expected& operator=(expected&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        destroy();

        m_has_value = other.m_has_value;
        if (m_has_value)
        {
            value_traits::construct(m_storage, std::move(other).value());
        }
        else
        {
            error_traits::construct(m_storage, std::move(other.error()));
        }

        return *this;
    }

    //=====================================
    // observers / accessors
    //=====================================

    bool has_value() const noexcept
    {
        return m_has_value;
    }

    explicit operator bool() const noexcept
    {
        return m_has_value;
    }

    T& value() & noexcept
    {
        VX_VERIFY(m_has_value, "");
        return value_traits::get(m_storage);
    }

    const T& value() const& noexcept
    {
        VX_VERIFY(m_has_value, "");
        return value_traits::get(m_storage);
    }

    T&& value() && noexcept
    {
        VX_VERIFY(m_has_value, "");
        return value_traits::get_rv(m_storage);
    }

    E& error() & noexcept
    {
        VX_VERIFY(!m_has_value, "");
        return error_traits::get(m_storage);
    }

    const E& error() const& noexcept
    {
        VX_VERIFY(!m_has_value, "");
        return error_traits::get(m_storage);
    }

    template <typename U>
    T value_or(U&& default_value) const&
    {
        return m_has_value ? value_traits::get(m_storage) : static_cast<T>(std::forward<U>(default_value));
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

//=============================================================================
// expected_error
//=============================================================================

template <typename E, E NoError = E{}>
class expected_error
{
public:

    //=====================================
    // constructors
    //=====================================

    constexpr expected_error() noexcept
        : m_error(NoError)
    {
    }

    constexpr expected_error(const unexpected<E>& unex) noexcept
        : m_error(unex.error)
    {
    }

    constexpr expected_error(unexpected<E>&& unex) noexcept
        : m_error(std::move(unex.error))
    {
    }

    //=====================================

    constexpr expected_error(const expected_error&) noexcept = default;
    constexpr expected_error(expected_error&&) noexcept = default;

    //=====================================
    // destructor
    //=====================================

    ~expected_error() = default;

    //=====================================
    // assignment
    //=====================================

    constexpr expected_error& operator=(const expected_error&) noexcept = default;
    constexpr expected_error& operator=(expected_error&&) noexcept = default;

    //=====================================
    // observers
    //=====================================

    constexpr bool has_value() const noexcept
    {
        return m_error == NoError;
    }

    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    //=====================================
    // accessors
    //=====================================

    constexpr bool value() const noexcept
    {
        return has_value();
    }

    constexpr E& error() & noexcept
    {
        return m_error;
    }

    constexpr const E& error() const& noexcept
    {
        return m_error;
    }

private:

    E m_error;
};

} // namespace vx
