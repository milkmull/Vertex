#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_system_info.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_system_info.hpp"
#endif

namespace vx {
namespace os {

VX_API const char* get_name()
{
    return get_name_impl();
}

VX_API std::string get_model_name()
{
    return get_model_name_impl();
}

VX_API std::string get_system_name()
{
    return get_system_name_impl();
}

VX_API std::string get_processor_name()
{
    return get_processor_name_impl();
}

VX_API uint32_t get_processor_count()
{
    return get_processor_count_impl();
}

VX_API bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    return get_version_impl(major, minor, patch);
}

} // namespace os
} // namespace vx