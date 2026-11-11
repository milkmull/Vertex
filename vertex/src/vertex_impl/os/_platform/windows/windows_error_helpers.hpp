#pragma once

#include "vertex/std/expected.hpp"
#include "vertex/std/error/error_code.hpp"
#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {

//=============================================================================
// Adapter section: the only place that knows how errors/handles are built
//=============================================================================

static DWORD win32_from_hresult(HRESULT hr) noexcept
{
    if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
    {
        return HRESULT_CODE(hr);
    }

    switch (hr)
    {
        case E_OUTOFMEMORY:
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        case E_ACCESSDENIED:
        {
            return ERROR_ACCESS_DENIED;
        }
        case E_INVALIDARG:
        {
            return ERROR_INVALID_PARAMETER;
        }
        case E_NOTIMPL:
        {
            return ERROR_NOT_SUPPORTED;
        }
        default:
        {
            return ERROR_INTERNAL_ERROR;
        }
    }
}

// Result of fail(): converts to success or to expected<T, error>, so
// `return fail("api");` works in every function of this file.
struct failure
{
    error value;

    operator success() const noexcept
    {
        return success(value);
    }

    template <typename T>
    operator expected<T, error>() const
    {
        return expected<T, error>(unexpected<error>(value)); // ADAPT to your expected/unexpected API
    }
};

// Captures GetLastError() at the call site.
inline failure fail(const char* api)
{
    err::code c = map_win32_error(::GetLastError());

    if (c == err::system_error)
    {
        // The code carries no information here, so keep the raw OS error and message.
        // Expected failures (not found, already exists, denied, ...) skip this on purpose:
        // formatting an OS message is far more expensive than the failing syscall's
        // error path, and those cases are routine (probing paths, mkdir on an existing dir).
        // Nothing may call into Win32 between the failing API and this point.
        err::set_last_os_error(api);
    }

    if (c == err::none)
    {
        c = err::failed; // an API failed without setting a last error
    }

    return failure{ error(c) };
}

} // namespace os
} // namespace vx
