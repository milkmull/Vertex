#pragma once

#include "vertex/config/os.hpp"

namespace vx {
namespace os {

#if defined(VX_OS_WINDOWS)

using native_char = wchar_t;
    #define VX_OS_NATIVE_LITERAL(x) L##x

#else

using native_char = char;
    #define VX_OS_NATIVE_LITERAL(x) x

#endif // VX_OS_WINDOWS

using native_cstring = const native_char*;

} // namespace os
} // namespace vx
