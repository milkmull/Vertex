#pragma once

#include <vector>
#include <unordered_map>

#include "vertex/app/hints/hints.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {
namespace hint {

///////////////////////////////////////////////////////////////////////////////
// hint data
///////////////////////////////////////////////////////////////////////////////

struct hint_callback_data
{
    hint_callback callback;
    void* user_data;
};

struct hint_entry
{
    std::vector<hint_callback_data> callbacks;

    const char* user_value = nullptr;
    const char* default_value = nullptr;

    void update(hint_t name, const char* old_value, const char* new_value);
};

struct hint_data
{
    std::unordered_map<hint_t, hint_entry> hints;
    mutable os::mutex mutex;
};

///////////////////////////////////////////////////////////////////////////////
// internal
///////////////////////////////////////////////////////////////////////////////

class hint_internal
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    static bool init();
    static void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // checkers
    ///////////////////////////////////////////////////////////////////////////////

    static bool has_hint(hint_t name);

    ///////////////////////////////////////////////////////////////////////////////
    // getters
    ///////////////////////////////////////////////////////////////////////////////

    static const char* get_hint(hint_t name);

    ///////////////////////////////////////////////////////////////////////////////
    // setters
    ///////////////////////////////////////////////////////////////////////////////

    static bool set_hint(hint_t name, const char* value);
    static void reset_hint(hint_t name);

    static bool set_hint_default_value(hint_t name, const char* value, bool override_user_value);

    ///////////////////////////////////////////////////////////////////////////////
    // callbacks
    ///////////////////////////////////////////////////////////////////////////////

    static void add_hint_callback(hint_t name, hint_callback callback, void* user_data);
    static void remove_hint_callback(hint_t name, hint_callback callback, void* user_data);

    static void add_hint_callback_and_default_value(hint_t name, hint_callback callback, void* user_data, const char* default_value);
};

///////////////////////////////////////////////////////////////////////////////
// backend
///////////////////////////////////////////////////////////////////////////////

inline bool set_hint_default_value(hint_t name, const char* value, bool override_user_value)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND(false);
    return hint_internal::set_hint_default_value(name, value, override_user_value);
}

inline void add_hint_callback_and_default_value(hint_t name, hint_callback callback, void* user_data, const char* default_value)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND_VOID();
    return hint_internal::add_hint_callback_and_default_value(name, callback, user_data, default_value);
}

} // namespace hint
} // namespace app
} // namespace vx