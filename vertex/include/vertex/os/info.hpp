#pragma once

#include <string>

namespace vx {
namespace os {

const char* get_name();
std::string get_model_name();
std::string get_system_name();

std::string get_processor_name();
uint32_t get_processor_count();

bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx