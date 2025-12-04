#include "vertex/config/language_config.hpp"
#include "vertex_impl/os/_platform/platform_system_info.hpp"

namespace vx {
namespace os {

std::string get_model_name()
{
    return get_model_name_impl();
}

std::string get_system_name()
{
    return get_system_name_impl();
}

std::string get_distribution_name()
{
    return get_distribution_name_impl();
}

std::string get_processor_name()
{
    return get_processor_name_impl();
}

uint32_t get_processor_count()
{
    return get_processor_count_impl();
}

bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    return get_version_impl(major, minor, patch);
}

} // namespace os
} // namespace vx