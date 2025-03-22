#pragma once

#include "vertex/os/locale.hpp"

namespace vx {
namespace os {
namespace locale {

bool get_country_code_impl(std::string& country_code);
bool get_language_impl(std::string& language);

} // namespace locale
} // namespace os
} // namespace vx