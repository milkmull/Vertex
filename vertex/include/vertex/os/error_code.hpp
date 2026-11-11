#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/native_error.hpp"
#include "vertex/std/error/error_code.hpp"

namespace vx {
namespace os {

VX_API err::code to_error_code(native_error e) noexcept;

VX_API native_error get_last_error();
VX_API void set_last_error(native_error error);

inline constexpr native_error no_error() noexcept
{
    return static_cast<native_error>(0);
}

inline void clear_last_error()
{
    set_last_error(no_error());
}

} // namespace os
} // namespace vx
