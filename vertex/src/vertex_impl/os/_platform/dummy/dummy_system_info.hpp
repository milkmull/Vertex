#pragma once

#include "vertex/os/system_info.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

static std::string get_model_name_impl()
{
    unsupported("get_model_name");
    return {};
}

static std::string get_system_name_impl()
{
    unsupported("get_system_name");
    return {};
}

std::string get_distribution_name_impl()
{
    return "Dummy";
}

static std::string get_processor_name_impl()
{
    unsupported("get_processor_name");
    return {};
}

static uint32_t get_processor_count_impl()
{
    unsupported("get_processor_count");
    return 0;
}

static bool get_version_impl(uint32_t*, uint32_t*, uint32_t*)
{
    unsupported("get_version");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx