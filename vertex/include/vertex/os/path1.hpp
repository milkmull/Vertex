#pragma once

namespace vx {
namespace os {

#if defined(VX_OS_WINDOWS)

using path_char = wchar_t;

#else

using path_char = char;

#endif

using path_ptr = const path_char*;

} // namespace os
} // namespace vx
