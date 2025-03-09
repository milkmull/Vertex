#pragma once

namespace vx {
namespace os {
namespace windows {

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/efda8314-6e41-4837-8299-38ba0ee04b92

typedef void* HANDLE_;

typedef unsigned long DWORD_, *PDWORD_, *LPDWORD_;

///////////////////////////////////////////////////////////////////////////////
// HANDLE wrapper
///////////////////////////////////////////////////////////////////////////////

class handle
{
public:

    inline handle() noexcept;
    inline ~handle() noexcept;

    inline handle(handle&& h) noexcept;
    inline handle& operator=(handle&&) noexcept;

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

    inline handle(const HANDLE_ h) noexcept;
    inline handle& operator=(const HANDLE_ h) noexcept;

public:

    inline bool is_valid() const noexcept;
    inline HANDLE_ get() const noexcept;
    inline void reset() noexcept;
    inline void close() noexcept;

private:

    HANDLE_ m_handle;
};

} // namespace windows
} // namespace os
} // namespace vx
