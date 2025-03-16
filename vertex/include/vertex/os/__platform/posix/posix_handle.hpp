#pragma once

namespace vx {
namespace os {
namespace posix {

class handle
{
public:

    inline handle() noexcept;
    inline ~handle() noexcept;

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

    inline handle(handle&& h) noexcept;
    inline handle& operator=(handle&& h) noexcept;

    inline handle(const int h) noexcept;
    inline handle& operator=(const int h) noexcept;

public:

    inline bool is_valid() const noexcept;
    inline int get() const noexcept;
    inline void reset() noexcept;
    inline void close() noexcept;

private:

    int m_handle;
};


} // namespace posix
} // namespace os
} // namespace vx