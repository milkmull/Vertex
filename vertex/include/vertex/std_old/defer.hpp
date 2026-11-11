#pragma once

#include <utility>

#include "vertex/config/language_config.hpp"

namespace vx {

template <typename F>
class defer;

namespace _defer_priv {

enum class defer_tag
{
};

template <typename F>
defer<F> operator+(defer_tag, F&& f)
{
    return defer<F>(std::forward<F>(f));
}

template <typename F>
class defer_base
{
public:

    explicit defer_base(F&& f) noexcept
        : m_fn(std::forward<F>(f))
    {}

    void call() noexcept
    {
        m_fn();
    }

private:

    F m_fn;
};

} // namespace _defer_priv

template <typename F>
class defer : private _defer_priv::defer_base<F>
{
    using base = _defer_priv::defer_base<F>;

public:

    using base::defer_base;

    ~defer() noexcept
    {
        base::call();
    }

    defer(const defer&) = delete;
    defer& operator=(const defer&) = delete;
    defer(defer&&) = delete;
    defer& operator=(defer&&) = delete;
};

#define VX_DEFER \
    auto VX_CONCAT(_vx_defer_, __COUNTER__) = ::vx::_defer_priv::defer_tag{} + [&]() noexcept

template <typename F>
class defer_guard : private _defer_priv::defer_base<F>
{
    using base = _defer_priv::defer_base<F>;

public:

    using base::defer_base;

    ~defer_guard() noexcept
    {
        if (m_active)
        {
            base::call();
        }
    }

    defer_guard(const defer_guard&) = delete;
    defer_guard& operator=(const defer_guard&) = delete;
    defer_guard(defer_guard&&) = delete;
    defer_guard& operator=(defer_guard&&) = delete;

    void dismiss() noexcept
    {
        m_active = false;
    }
    void rearm() noexcept
    {
        m_active = true;
    }
    bool active() const noexcept
    {
        return m_active;
    }

private:

    bool m_active;
};

template <typename F>
defer_guard(F&&) -> defer_guard<std::decay_t<F>>;

} // namespace vx
