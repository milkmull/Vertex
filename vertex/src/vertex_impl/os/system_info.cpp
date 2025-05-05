#include "vertex/config/language_config.hpp"
#include "vertex_impl/os/__platform/system_info.hpp"

namespace vx {
namespace os {

VX_API std::string get_model_name()
{
    return get_model_name_impl();
}

VX_API std::string get_system_name()
{
    return get_system_name_impl();
}

VX_API std::string get_distribution_name()
{
    return get_distribution_name_impl();
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