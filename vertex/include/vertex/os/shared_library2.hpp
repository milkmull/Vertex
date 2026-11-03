#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/native_string.hpp"
#include "vertex/std/error.hpp"

namespace vx {
namespace os {
namespace shared_library {

//=============================================================================
// platform
//=============================================================================

constexpr const char* suffix() noexcept
{
#if defined(VX_OS_WINDOWS)
    return ".dll";
#elif defined(VX_OS_APPLE)
    return ".dylib";
#else
    return ".so";
#endif
}

//=============================================================================
// types
//=============================================================================

using native_handle = void*;
using native_symbol = void*;
using symbol_char = char;
using symbol_cstring = const symbol_char*;

//=============================================================================
// api
//=============================================================================

VX_API success load(native_handle& h, native_cstring lib);
VX_API void free(native_handle h);
VX_API native_symbol get_symbol(native_handle h, symbol_cstring symbol_name);

} // namespace shared_library
} // namespace os
} // namespace vx
