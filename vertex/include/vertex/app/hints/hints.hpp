#pragma once

#include "vertex/app/hints/hint_defs.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace app {
namespace hint {

//=============================================================================
// parsers
//=============================================================================

/**
 * @brief Parses a string hint value, falling back to a default if null.
 *
 * @param value Pointer to the hint string value.
 * @param default_value Fallback string if value is null.
 * @return The hint value or the default.
 */
inline const char* parse_string(const char* value, const char* default_value) noexcept
{
    return value ? value : default_value;
}

/**
 * @brief Parses a boolean hint from a string value.
 *
 * Accepts "0" or "false" (case-insensitive) as false, anything else true.
 *
 * @param value String to parse.
 * @param default_value Default boolean if value is null or empty.
 * @return Parsed boolean or default.
 */
inline bool parse_boolean(const char* value, bool default_value)
{
    if (!value || !*value)
    {
        return default_value;
    }
    return !((std::strcmp(value, "0") == 0) || str::casecmp(value, "false"));
}

/**
 * @brief Parses a signed integer from a string hint.
 *
 * @param value String to parse.
 * @param default_value Default integer if parsing fails.
 * @return Parsed integer or default.
 */
inline int64_t parse_integer(const char* value, int64_t default_value)
{
    if (!value || !*value)
    {
        return default_value;
    }

    char* end = nullptr;
    const auto result = std::strtoll(value, &end, 10);
    return (end != value) ? static_cast<int64_t>(result) : default_value;
}

/**
 * @brief Parses an unsigned integer from a string hint.
 *
 * @param value String to parse.
 * @param default_value Default unsigned integer if parsing fails.
 * @return Parsed unsigned integer or default.
 */
inline uint64_t parse_unsigned_integer(const char* value, uint64_t default_value)
{
    if (!value || !*value)
    {
        return default_value;
    }

    char* end = nullptr;
    const auto result = std::strtoull(value, &end, 10);
    return (end != value) ? static_cast<uint64_t>(result) : default_value;
}

/**
 * @brief Parses a floating-point number from a string hint.
 *
 * @param value String to parse.
 * @param default_value Default float if parsing fails.
 * @return Parsed float or default.
 */
inline float parse_float(const char* value, float default_value)
{
    if (!value || !*value)
    {
        return default_value;
    }

    char* end = nullptr;
    const auto result = std::strtof(value, &end);
    return (end != value) ? result : default_value;
}

//=============================================================================
// checkers
//=============================================================================

/**
 * @brief Checks whether a hint with the specified name is set.
 *
 * @param name The hint identifier.
 * @return true if the hint exists and has a value, false otherwise.
 */
VX_API bool has_hint(hint_t name);

//=============================================================================
// getters
//=============================================================================

/**
 * @brief Retrieves the raw string value of a hint.
 *
 * @param name The hint identifier.
 * @return Pointer to the hint string value, or nullptr if not set.
 */
VX_API const char* get_hint(hint_t name);

/**
 * @brief Retrieves a hint as a string, falling back to a default if unset.
 *
 * @param name The hint identifier.
 * @param default_value The default string to return if the hint is unset.
 * @return The hint value or the default.
 */
inline const char* get_hint_string(hint_t name, const char* default_value)
{
    return parse_string(get_hint(name), default_value);
}

/**
 * @brief Retrieves a hint as a boolean value.
 *
 * @param name The hint identifier.
 * @param default_value Default boolean if the hint is unset or invalid.
 * @return Parsed boolean value of the hint.
 */
inline bool get_hint_boolean(hint_t name, bool default_value)
{
    return parse_boolean(get_hint(name), default_value);
}

/**
 * @brief Retrieves a hint as a signed integer.
 *
 * @param name The hint identifier.
 * @param default_value Default integer if the hint is unset or invalid.
 * @return Parsed integer value of the hint.
 */
inline int64_t get_hint_integer(hint_t name, int64_t default_value)
{
    return parse_integer(get_hint(name), default_value);
}

/**
 * @brief Retrieves a hint as an unsigned integer.
 *
 * @param name The hint identifier.
 * @param default_value Default unsigned integer if the hint is unset or invalid.
 * @return Parsed unsigned integer value of the hint.
 */
inline uint64_t get_hint_unsigned_integer(hint_t name, uint64_t default_value)
{
    return parse_unsigned_integer(get_hint(name), default_value);
}

/**
 * @brief Retrieves a hint as a floating-point number.
 *
 * @param name The hint identifier.
 * @param default_value Default float if the hint is unset or invalid.
 * @return Parsed float value of the hint.
 */
inline float get_hint_float(hint_t name, float default_value)
{
    return parse_float(get_hint(name), default_value);
}

//=============================================================================
// setters
//=============================================================================

/**
 * @brief Sets the value of a hint.
 *
 * If the hint already exists, its value will be updated.
 *
 * @param name The hint identifier.
 * @param value New string value to set.
 * @return true if the hint was set successfully, false otherwise.
 */
VX_API bool set_hint(hint_t name, const char* value);

/**
 * @brief Resets the value of a hint to its default (unset) state.
 *
 * @param name The hint identifier.
 */
VX_API void reset_hint(hint_t name);

//=============================================================================
// callbacks
//=============================================================================

/**
 * @brief Function pointer type for hint change callbacks.
 *
 * Called whenever a hint value changes.
 *
 * @param name The hint identifier.
 * @param old_value Previous string value of the hint (may be nullptr).
 * @param new_value New string value of the hint (may be nullptr).
 * @param user_data User-defined data passed to the callback.
 */
using hint_callback = void (*)(const hint_t name, const char* old_value, const char* new_value, void* user_data);

/**
 * @brief Adds a callback for when a specific hint changes.
 *
 * The callback is invoked **immediately upon registration** with the current hint value.
 *
 * @param name The hint identifier to watch.
 * @param callback The function to call on hint changes.
 * @param user_data User data passed to the callback.
 */
VX_API void add_hint_callback(hint_t name, hint_callback callback, void* user_data);

/**
 * @brief Removes a previously added hint change callback.
 *
 * @param name The hint identifier.
 * @param callback The callback function to remove.
 * @param user_data The user data pointer associated with the callback.
 */
VX_API void remove_hint_callback(hint_t name, hint_callback callback, void* user_data);

} // namespace hint
} // namespace app
} // namespace vx
