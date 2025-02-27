#pragma once

#include <vector>

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/os/shared_library.hpp"

namespace vx {
namespace os {

const char* get_name_impl();
std::string get_model_name_impl();
std::string get_system_name_impl();
std::string get_processor_name_impl();
uint32_t get_processor_count_impl();
bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx