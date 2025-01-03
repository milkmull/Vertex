#pragma once

#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {
namespace windows {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void error_message(const std::string& msg);

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

class com_scoped_initializer
{
public:

    com_scoped_initializer();
    ~com_scoped_initializer();

    com_scoped_initializer(const com_scoped_initializer&) = delete;
    com_scoped_initializer& operator=(const com_scoped_initializer&) = delete;

    bool succeeded() const;

private:

    HRESULT m_hr;
};

///////////////////////////////////////////////////////////////////////////////
// HANDLE wrapper
///////////////////////////////////////////////////////////////////////////////

class handle
{
public:

    handle() : m_handle(INVALID_HANDLE_VALUE) {}
    handle(HANDLE handle) : m_handle(handle) {}

    ~handle() { close(); }

    bool is_valid() const
    {
        return m_handle != NULL && m_handle != INVALID_HANDLE_VALUE;
    }

    HANDLE get() const { return m_handle; }

private:

    void close()
    {
        if (is_valid())
        {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }

private:

    HANDLE m_handle;
};

} // namespace vx
} // namespace os
} // namespace windows