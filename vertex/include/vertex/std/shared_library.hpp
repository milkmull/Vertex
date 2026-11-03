#pragma once

#include "vertex/os/shared_library2.hpp"
#include "vertex/std/native_string.hpp"

namespace vx {

class shared_library
{
public:

    //=============================================================================
    // handle
    //=============================================================================

    using native_handle = os::shared_library::native_handle;

    struct handle
    {
    public:

        // construction / assignment

        handle() noexcept = default;

        handle(const handle&) = default;
        handle& operator=(const handle&) = default;

        handle(handle&& h) noexcept
            : m_handle(vx::exchange(h.m_handle, native_handle{}))
        {
        }

        handle& operator=(handle&& h) noexcept
        {
            if (this != &h)
            {
                m_handle = vx::exchange(h.m_handle, native_handle{});
            }

            return *this;
        }

        explicit handle(native_handle h) noexcept
            : m_handle(h)
        {}

        handle& operator=(native_handle h) noexcept
        {
            m_handle = h;
            return *this;
        }

        // comparison

        bool operator==(const handle& h) const noexcept
        {
            return m_handle == h.m_handle;
        }

        bool operator!=(const handle& h) const noexcept
        {
            return m_handle != h.m_handle;
        }

        // observers

        bool is_valid() const noexcept
        {
            return m_handle != native_handle{};
        }

        native_handle& native() noexcept
        {
            return m_handle;
        }

        const native_handle& native() const noexcept
        {
            return m_handle;
        }

        // modifiers

        void reset() noexcept
        {
            m_handle = native_handle{};
        }

        native_handle release() noexcept
        {
            return vx::exchange(m_handle, native_handle{});
        }

    private:

        native_handle m_handle = native_handle{};
    };

public:

    shared_library() = default;

    explicit shared_library(const native_string& name)
    {
        load(name);
    }

    ~shared_library()
    {
        free();
    }

    shared_library(const shared_library&) = delete;
    shared_library& operator=(const shared_library&) = delete;

    shared_library(shared_library&& other) noexcept
        : m_handle(other.m_handle.release())
    {}

    shared_library& operator=(shared_library&& other) noexcept
    {
        if (this != &other)
        {
            free();
            m_handle = other.m_handle.release();
        }

        return *this;
    }

    void swap(shared_library& other) noexcept
    {
        vx::swap(m_handle, other.m_handle);
    }

public:

    bool is_loaded() const noexcept
    {
        return m_handle.is_valid();
    }

    success load(const native_string& lib)
    {
        VX_ASSERT(!is_loaded());
        VX_RET_ERR_IF(!lib.is_valid(), err::out_of_memory);
        return os::shared_library::load(m_handle.native(), lib.c_str());
    }

    void free() noexcept
    {
        if (is_loaded())
        {
            os::shared_library::free(m_handle.native());
            m_handle.reset();
        }
    }

public:

    using symbol_string = str::temporary_string<os::shared_library::symbol_char>;

    expected<bool, error> has(const symbol_string& symbol_name) const noexcept
    {
        VX_ASSERT(is_loaded());
        VX_RET_UNEXPECTED_ERR_IF(!symbol_name.is_valid(), err::out_of_memory);
        const bool found = get_symbol_impl(symbol_name.c_str()) != nullptr;
        return found;
    }

    template <typename Func>
    expected<Func, error> get(const symbol_string& symbol_name) const noexcept
    {
        VX_STATIC_ASSERT(sizeof(Func) == sizeof(os::shared_library::native_symbol));
        VX_ASSERT(is_loaded());
        VX_RET_UNEXPECTED_ERR_IF(!symbol_name.is_valid(), err::out_of_memory);
        const auto symbol = get_symbol_impl(symbol_name.c_str());
        VX_RET_UNEXPECTED_ERR_IF(!symbol, err::resource_not_found);
        return reinterpret_cast<Func>(symbol);
    }

    template <typename Func>
    expected<Func, error> get_assume_exists(const symbol_string& symbol_name) const noexcept
    {
        VX_STATIC_ASSERT(sizeof(Func) == sizeof(os::shared_library::native_symbol));
        VX_ASSERT(is_loaded());
        VX_RET_UNEXPECTED_ERR_IF(!symbol_name.is_valid(), err::out_of_memory);
        const auto symbol = get_symbol_impl(symbol_name.c_str());
        return reinterpret_cast<Func>(symbol);
    }

    expected<os::shared_library::native_symbol, error> get_symbol(const symbol_string& name) const noexcept
    {
        VX_ASSERT(is_loaded());
        VX_RET_UNEXPECTED_ERR_IF(!name.is_valid(), err::out_of_memory);
        const auto symbol = get_symbol_impl(name.c_str());
        VX_RET_UNEXPECTED_ERR_IF(!symbol, err::resource_not_found);
        return symbol;
    }

    expected<os::shared_library::native_symbol, error> get_symbol_assume_exists(const symbol_string& name) const noexcept
    {
        VX_ASSERT(is_loaded());
        VX_RET_UNEXPECTED_ERR_IF(!name.is_valid(), err::out_of_memory);
        return get_symbol_impl(name.c_str());
    }

private:

    os::shared_library::native_symbol get_symbol_impl(os::shared_library::symbol_cstring symbol_name) const noexcept
    {
        VX_ASSERT(is_loaded());
        return os::shared_library::get_symbol(m_handle.native(), symbol_name);
    }

private:

    handle m_handle;
};

} // namespace vx
