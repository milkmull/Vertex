#pragma once

#include <string>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {
namespace locale {

VX_API bool get_country_code(std::string& country_code);
VX_API bool get_language(std::string& language);

} // namespace locale
} // namespace os
} // namespace vx