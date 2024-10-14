#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"

#include "vertex/stdlib/system/locale.hpp"
#include "vertex/stdlib/string/string.hpp"

namespace vx {
namespace locale {

std::string get_country_code()
{
    wchar_t winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        VX_ERROR(error::error_code::PLATFORM_ERROR) << "GetLocaleInfoEx() failed";
    }

    return str::wstring_to_string(winfo);
}

std::string get_language()
{
    wchar_t winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        VX_ERROR(error::error_code::PLATFORM_ERROR) << "GetLocaleInfoEx() failed";
    }

    return str::wstring_to_string(winfo);
}

}
}

#endif