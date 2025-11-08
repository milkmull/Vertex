#define _CRT_RAND_S
#include <stdlib.h>

#include "vertex_impl/os/_platform/windows/windows_random.hpp"
#include "vertex_impl/os/_platform/windows/windows_header.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rand-s?view=msvc-170
// https://github.com/microsoft/STL/blob/25dc2b72b6f594326253cf3a05543f7a5750f802/stl/src/xrngdev.cpp#L10

bool get_entropy_impl(uint8_t* bytes, size_t count)
{
    if (bytes == nullptr)
    {
        return false;
    }
    if (count == 0)
    {
        return true;
    }

    size_t off = 0;
    while (off != count)
    {
        unsigned int r;
        const errno_t err = ::rand_s(&r);

        if (err != 0)
        {
            VX_ERR(err::system_error) << "rand_s(): " << err;
            return false;
        }

        const size_t copy_count = std::min(count - off, sizeof(r));
        std::memcpy(bytes + off, &r, copy_count);
        off += copy_count;
    }

    return true;
}

} // namespace os
} // namespace vx