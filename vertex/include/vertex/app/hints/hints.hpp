#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/app/hints/hint_defs.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace app {
namespace hint {

///////////////////////////////////////////////////////////////////////////////
// parsers
///////////////////////////////////////////////////////////////////////////////

inline const char* parse_string(const char* value, const char* default_value) noexcept
{
    return value ? value : default_value;
}

inline bool parse_boolean(const char* value, bool default_value)
{
    if (!value) return default_value;
    return !((std::strcmp(value, "0") == 0) || str::casecmp(value, "false"));
}

inline int64_t parse_integer(const char* value, int64_t default_value)
{
    if (!value) return default_value;

    char* end = nullptr;
    const auto result = std::strtoll(value, &end, 10);
    return (end != value) ? static_cast<int64_t>(result) : default_value;
}

inline uint64_t parse_hint_unsigned_integer(const char* value, uint64_t default_value)
{
    if (!value) return default_value;

    char* end = nullptr;
    const auto result = std::strtoull(value, &end, 10);
    return (end != value) ? static_cast<uint64_t>(result) : default_value;
}

inline float parse_hint_float(const char* value, float default_value)
{
    if (!value) return default_value;

    char* end = nullptr;
    const auto result = std::strtof(value, &end);
    return (end != value) ? result : default_value;
}

///////////////////////////////////////////////////////////////////////////////
// checkers
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_hint(hint_t name);

///////////////////////////////////////////////////////////////////////////////
// getters
///////////////////////////////////////////////////////////////////////////////

VX_API const char* get_hint(hint_t name);

inline const char* get_hint_string(hint_t name, const char* default_value)
{
    return parse_string(get_hint(name), default_value);
}

inline bool get_hint_boolean(hint_t name, bool default_value)
{
    return parse_boolean(get_hint(name), default_value);
}

inline int64_t get_hint_integer(hint_t name, int64_t default_value)
{
    return parse_integer(get_hint(name), default_value);
}

inline uint64_t get_hint_unsigned_integer(hint_t name, uint64_t default_value)
{
    return parse_hint_unsigned_integer(get_hint(name), default_value);
}

inline float get_hint_float(hint_t name, float default_value)
{
    return parse_hint_float(get_hint(name), default_value);
}

///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////

VX_API bool set_hint(hint_t name, const char* value);
VX_API void reset_hint(hint_t name);

///////////////////////////////////////////////////////////////////////////////
// callbacks
///////////////////////////////////////////////////////////////////////////////

using hint_callback = void(*)(const hint_t name, const char* old_value, const char* new_value, void* user_data);

VX_API void add_hint_callback(hint_t name, hint_callback callback, void* user_data);
VX_API void remove_hint_callback(hint_t name, hint_callback callback, void* user_data);

} // namespace hint
} // namespace app
} // namespace vx