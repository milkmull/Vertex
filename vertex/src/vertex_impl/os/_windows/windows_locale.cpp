#include "vertex_impl/system/platform_config.hpp"

#if defined(VX_OS_WINDOWS_LOCALE)

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include "vertex/system/error.hpp"
#include "vertex/os/locale.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace locale {

std::string get_country_code()
{
    wchar_t winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        VX_ERR(err::PLATFORM_ERROR) << "GetLocaleInfoEx()";
    }

    return str::wstring_to_string(winfo);
}

std::string get_language()
{
    wchar_t winfo[LOCALE_NAME_MAX_LENGTH]{};

    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, winfo, LOCALE_NAME_MAX_LENGTH))
    {
        VX_ERR(err::PLATFORM_ERROR) << "GetLocaleInfoEx()";
    }

    return str::wstring_to_string(winfo);
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_LOCALE