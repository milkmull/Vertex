#pragma once

#include "vertex/os/system_info.hpp"

namespace vx {
namespace os {

std::string get_model_name_impl();
std::string get_system_name_impl();
std::string get_distribution_name_impl();
std::string get_processor_name_impl();
uint32_t get_processor_count_impl();
bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx