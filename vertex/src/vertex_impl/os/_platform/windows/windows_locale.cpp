#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_LOCALE)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/system/error.hpp"
#include "vertex/os/locale.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {
namespace locale {

VX_API bool get_country_code(std::string& country_code)
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

VX_API bool get_language(std::string& language)
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

#endif // VX_OS_WINDOWS_LOCALE