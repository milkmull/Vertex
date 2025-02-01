#pragma once

#include <string>

#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {
namespace windows {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void error_message(const char* msg);

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

class com_scoped_initializer
{
public:

    com_scoped_initializer() noexcept;
    ~com_scoped_initializer() noexcept;

    com_scoped_initializer(const com_scoped_initializer&) = delete;
    com_scoped_initializer& operator=(const com_scoped_initializer&) = delete;

    bool succeeded() const noexcept;

private:

    HRESULT m_hr;
};

///////////////////////////////////////////////////////////////////////////////
// HANDLE wrapper
///////////////////////////////////////////////////////////////////////////////

class handle
{
public:

    handle() noexcept : m_handle(INVALID_HANDLE_VALUE) {}

    handle(const HANDLE h) noexcept : m_handle(h) {}

    handle(handle&& h) noexcept : m_handle(INVALID_HANDLE_VALUE)
    {
        if (this != &h)
        {
            m_handle = h.m_handle;
            h.reset();
        }
    }

    ~handle() noexcept { close(); }

    handle& operator=(const HANDLE h) noexcept
    {
        close();
        m_handle = h;
        return *this;
    }

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;
    handle& operator=(handle&&) noexcept = delete;

public:

    bool is_valid() const noexcept
    {
        return m_handle != NULL && m_handle != INVALID_HANDLE_VALUE;
    }

    HANDLE get() const noexcept { return m_handle; }
    void reset() noexcept { m_handle = INVALID_HANDLE_VALUE; }

private:

    void close() noexcept
    {
        if (is_valid())
        {
            CloseHandle(m_handle);
            reset();
        }
    }

private:

    HANDLE m_handle;
};

} // namespace vx
} // namespace os
} // namespace windows