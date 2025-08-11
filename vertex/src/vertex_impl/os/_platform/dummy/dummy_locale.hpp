#pragma once

#include "vertex/os/locale.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {
namespace locale {

#define unsupported(op) VX_UNSUPPORTED("os::locale::" op "()")

static bool get_country_code_impl(std::string&)
{
    unsupported("get_country_code");
    return false;
}

static bool get_language_impl(std::string&)
{
    unsupported("get_language");
    return false;
}

#undef unsupported

} // namespace locale
} // namespace os
} // namespace vx