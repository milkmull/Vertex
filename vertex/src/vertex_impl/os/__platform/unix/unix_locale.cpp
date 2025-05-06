#include <locale.h>
#include <langinfo.h>

#include "vertex_impl/os/__platform/windows/windows_locale.hpp"
#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {
namespace locale {

bool get_country_code_impl(std::string& country_code)
{
    const char* locale = ::setlocale(LC_ALL, "");
    if (!locale)
    {
        unix_::error_message("setlocale()");
        return false;
    }

    // Usually LANG is in the form "en_US.UTF-8"
    const char* lang_env = std::getenv("LANG");
    if (!lang_env)
    {
        err::set(err::SYSTEM_ERROR, "LANG not defined in environment");
        return false;
    }

    const std::string lang_str(lang_env);

    const size_t underscore = lang_str.find('_');
    if (underscore == std::string::npos)
    {
        return false;
    }

    size_t dot = lang_str.find('.', underscore);
    if (dot == std::string::npos)
    {
        dot = lang_str.size();
    }

    country_code = lang_str.substr(underscore, dot - underscore);
    return true;
}

bool get_language_impl(std::string& language)
{
    const char* locale = ::setlocale(LC_ALL, "");
    if (!locale)
    {
        unix_::error_message("setlocale()");
        return false;
    }

    // Usually LANG is in the form "en_US.UTF-8"
    const char* lang_env = std::getenv("LANG");
    if (!lang_env)
    {
        err::set(err::SYSTEM_ERROR, "LANG not defined in environment");
        return false;
    }

    const std::string lang_str(lang_env);

    const size_t underscore = lang_str.find('_');
    if (underscore != std::string::npos)
    {
        language = lang_str.substr(0, underscore);
    }
    else
    {
        // LANG may just be "en" or similar
        const size_t dot = lang_str.find('.');
        language = lang_str.substr(0, dot);
    }

    return true;
}

} // namespace locale
} // namespace os
} // namespace vx