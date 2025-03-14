#pragma once

#include "vertex/os/__platform/windows/windows_types.hpp"

namespace vx {
namespace os {
namespace windows {

class handle
{
public:

    inline handle() noexcept;
    inline ~handle() noexcept;

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

    inline handle(handle&& h) noexcept;
    inline handle& operator=(handle&& h) noexcept;

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