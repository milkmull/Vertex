#pragma once

#include <vector>
#include <unordered_map>

#include "vertex/app/hints/hints.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {

class app_instance;

namespace hint {

//=============================================================================
// hint data
//=============================================================================

struct hint_callback_data
{
    hint_callback callback;
    void* user_data;
};

struct hint_entry
{
    std::vector<hint_callback_data> callbacks;

    const char* value = nullptr;
    void update(hint_t name, const char* old_value, const char* new_value);
};

struct hints_data
{
    std::unordered_map<hint_t, hint_entry> hints;
    mutable os::mutex mutex;
};

//=============================================================================
// internal
//=============================================================================

class hints_instance
{
public:

    hints_instance();
    ~hints_instance();

    hints_instance(const hints_instance&) = delete;
    hints_instance& operator=(const hints_instance&) = delete;

    hints_instance(hints_instance&&) noexcept = delete;
    hints_instance& operator=(hints_instance&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(app_instance* owner);
    void quit();

    //=============================================================================
    // checkers
    //=============================================================================

    bool hint_entry_exists(hint_t name) const
    {
        return data.hints.count(name);
    }

    bool has_hint(hint_t name) const;

    //=============================================================================
    // getters
    //=============================================================================

    const char* get_hint(hint_t name) const;

    inline const char* get_hint_string(hint_t name, const char* default_value) const
    {
        return parse_string(get_hint(name), default_value);
    }

    inline bool get_hint_boolean(hint_t name, bool default_value) const
    {
        return parse_boolean(get_hint(name), default_value);
    }

    inline int64_t get_hint_integer(hint_t name, int64_t default_value) const
    {
        return parse_integer(get_hint(name), default_value);
    }

    inline uint64_t get_hint_unsigned_integer(hint_t name, uint64_t default_value) const
    {
        return parse_unsigned_integer(get_hint(name), default_value);
    }

    inline float get_hint_float(hint_t name, float default_value) const
    {
        return parse_float(get_hint(name), default_value);
    }

    //=============================================================================
    // setters
    //=============================================================================

    bool set_hint(hint_t name, const char* value);
    void reset_hint(hint_t name);

    //=============================================================================
    // callbacks
    //=============================================================================

    void add_hint_callback(hint_t name, hint_callback callback, void* user_data);
    void remove_hint_callback(hint_t name, hint_callback callback, void* user_data);

    //=============================================================================
    // data
    //=============================================================================

    app_instance* app = nullptr;
    hints_data data;
};

} // namespace hint
} // namespace app
} // namespace vx