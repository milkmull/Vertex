#include "vertex/os/error_format.hpp"
#include "vertex/std/memory/_priv/memory_base.hpp"
#include "vertex/std/utf/utf.hpp"
#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {

size_t format_error(native_error error, native_buffer buffer, size_t size)
{
    if (!buffer || size == 0)
    {
        return 0;
    }

    // FormatMessageW fails with ERROR_INSUFFICIENT_BUFFER instead of
    // truncating, so format into a scratch buffer and truncate ourselves.
    WCHAR scratch[1024];

    DWORD count = ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        static_cast<DWORD>(error),
        0, // language: system default search order
        scratch,
        static_cast<DWORD>(mem::array_size(scratch)),
        NULL);

    if (count == 0)
    {
        // no system text for this code (or the call failed)
        return 0;
    }

    // Strip the trailing "\r\n" (and any trailing space/period padding
    // FormatMessage leaves) from the end.
    while (count > 0)
    {
        const WCHAR c = scratch[count - 1];
        if (c == L'\r' || c == L'\n' || c == L' ')
        {
            --count;
        }
        else
        {
            break;
        }
    }

    // Truncate to the caller's capacity
    size_t out = (count < size) ? count : size;

    // Don't leave a dangling high surrogate if we cut mid-pair
    if (out > 0 && out < count)
    {
        const WCHAR last = scratch[out - 1];
        if (utf::utf_traits<WCHAR>::is_first_surrogate(last))
        {
            --out;
        }
    }

    mem::copy(buffer, scratch, out * sizeof(WCHAR));
    return out;
}

} // namespace os
} // namespace vx
