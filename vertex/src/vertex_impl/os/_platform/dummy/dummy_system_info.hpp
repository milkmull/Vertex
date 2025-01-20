#pragma once

#include "vertex/os/system_info.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline const char* get_name_impl()
{
    return "Dummy";
}

inline std::string get_model_name_impl()
{
    unsupported("get_model_name");
    return {};
}

inline std::string get_system_name_impl()
{
    unsupported("get_system_name");
    return {};
}

inline std::string get_processor_name_impl()
{
    unsupported("get_processor_name");
    return {};
}

inline uint32_t get_processor_count_impl()
{
    unsupported("get_processor_count");
    return 0;
}

inline bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    unsupported("get_version");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx