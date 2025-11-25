#pragma once

#include <string>
#include <vector>
#include <optional>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace app {
namespace clipboard {

//=============================================================================
// callbacks
//=============================================================================

using data_callback = const void* (*)(const char* mime_type, size_t& size, void* user_data);
using cleanup_callback = void (*)(void* user_data);

//=============================================================================
// api
//=============================================================================

constexpr auto z = sizeof(std::optional<std::vector<std::string>>);

VX_API bool has_text();
VX_API std::string get_text();
VX_API bool set_text(const std::string& text);

VX_API bool has_primary_selection_text();
VX_API std::string get_primary_selection_text();
VX_API bool set_primary_selection_text(const std::string& text);

VX_API bool has_data(const std::string& mime_type);
VX_API std::vector<uint8_t> get_data(const std::string& mime_type);
VX_API bool set_data(data_callback callback, cleanup_callback cleanup, void* user_data, const std::vector<std::string>& mime_types);
VX_API bool clear_data();

VX_API std::vector<std::string> get_mime_types(size_t* count);

} // namespace clipboard
} // namespace app
} // namespace vx