#pragma once

#if defined(VX_OS_LOCALE_IMPL)
#   error "locale implementation has already been included!"
#endif
#define VX_OS_LOCALE_IMPL

#include "vertex/os/locale.hpp"
#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {
namespace locale {

static bool get_country_code_impl(std::string& country_code)
{
    WCHAR winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        windows::error_message("GetLocaleInfoEx()");
        return false;
    }

    country_code = str::string_cast<char>(winfo);
    return true;
}

static bool get_language_impl(std::string& language)
{
    WCHAR winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        windows::error_message("GetLocaleInfoEx()");
        return false;
    }

    language = str::string_cast<char>(winfo);
    return true;
}

} // namespace locale
} // namespace os
} // namespace vx