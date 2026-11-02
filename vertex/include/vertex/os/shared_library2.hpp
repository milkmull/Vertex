#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/native_string.hpp"
#include "vertex/std/error.hpp"

namespace vx {
namespace os {

constexpr const char* shared_library_suffix() noexcept
{
#if defined(VX_OS_WINDOWS)
    return ".dll";
#elif defined(VX_OS_APPLE)
    return ".dylib";
#else
    return ".so";
#endif
}

using shared_library_handle = void*;
using shared_library_symbol_cstring = const char*;

VX_API success load_shared_library(shared_library_handle& h, native_cstring lib);
VX_API void free_shared_library(shared_library_handle& h);
VX_API void* get_shared_library_symbol(shared_library_handle& h, shared_library_symbol_cstring symbol_name);

} // namespace os
} // namespace vx
