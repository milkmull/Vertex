#pragma once

#include <string>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

VX_API const char* get_name();
VX_API std::string get_model_name();
VX_API std::string get_system_name();

VX_API std::string get_processor_name();
VX_API uint32_t get_processor_count();

VX_API bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx