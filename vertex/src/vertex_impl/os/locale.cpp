#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_locale.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_locale.hpp"
#endif

namespace vx {
namespace os {
namespace locale {

VX_API bool get_country_code(std::string& country_code)
{
    return get_country_code_impl(country_code);
}

VX_API bool get_language(std::string& language)
{
    return get_language_impl(language);
}

} // namespace locale
} // namespace os
} // namespace vx