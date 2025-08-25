#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"

namespace vx {
namespace app {
namespace hint {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

bool hints_instance::init()
{
    return true;
}

void hints_instance::quit()
{

}

///////////////////////////////////////////////////////////////////////////////
// checkers
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(false);
    return s_hints->has_hint(name);
}

bool hints_instance::has_hint(hint_t name) const
{
    os::lock_guard lock(data.mutex);

    if (!hint_entry_exists(name))
    {
        return false;
    }

    const hint_entry& h = data.hints.at(name);
    return h.user_value || h.default_value;
}

///////////////////////////////////////////////////////////////////////////////
// getters
///////////////////////////////////////////////////////////////////////////////

VX_API const char* get_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(nullptr);
    return s_hints->get_hint(name);
}

const char* hints_instance::get_hint(hint_t name) const
{
    os::lock_guard lock(data.mutex);
    const hint_entry& h = data.hints.at(name);

    if (h.user_value)
    {
        return h.user_value;
    }
    if (h.default_value)
    {
        return h.default_value;
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////

void hints_instance::hint_entry::update(hint_t name, const char* old_value, const char* new_value)
{
    user_value = new_value;

    for (const auto& cb : callbacks)
    {
        cb.callback(name, old_value, new_value, cb.user_data);
    }
}

////////////////////////////////////////

VX_API bool set_hint(hint_t name, const char* value)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(false);
    return s_hints->set_hint(name, value);
}

bool hints_instance::set_hint(hint_t name, const char* value)
{
    if (!value)
    {
        return false;
    }

    os::lock_guard lock(data.mutex);
    hint_entry& h = data.hints[name];

    if (h.user_value || h.default_value)
    {
        const char* old_value = h.user_value ? h.user_value : h.default_value;

        if (std::strcmp(value, old_value) != 0)
        {
            h.update(name, old_value, h.user_value);
        }
    }
    else
    {
        h.update(name, nullptr, h.user_value);
    }

    return true;
}

////////////////////////////////////////

VX_API void reset_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    return s_hints->reset_hint(name);
}

void hints_instance::reset_hint(hint_t name)
{
    os::lock_guard lock(data.mutex);

    if (!hint_entry_exists(name))
    {
        return;
    }

    hint_entry& h = data.hints[name];

    // if we are returning to a default value, notify the callbacks
    if (h.user_value)
    {
        const char* old_value = h.user_value;
        const char* new_value = h.default_value ? h.default_value : nullptr;
        h.update(name, old_value, new_value);
    }

    // if this hint has no default value or callbacks we can remove the entry
    if (!h.default_value && h.callbacks.empty())
    {
        data.hints.erase(name);
    }
}

////////////////////////////////////////

bool set_hint_default_value(hint_t name, const char* value, bool override_user_value)
{
    return s_hints->set_hint_default_value(name, value, override_user_value);
}

bool hints_instance::set_hint_default_value(hint_t name, const char* value, bool override_user_value)
{
    if (!value)
    {
        return false;
    }

    os::lock_guard lock(data.mutex);
    hint_entry& h = data.hints[name];

    if (!h.user_value || override_user_value)
    {
        const char* old_value = h.user_value ? h.user_value : h.default_value;

        if (std::strcmp(value, old_value) != 0)
        {
            if (override_user_value)
            {
                h.user_value = nullptr;
            }

            h.update(name, old_value, h.default_value);
        }
    }
    else
    {
        h.default_value = value;
    }
}

///////////////////////////////////////////////////////////////////////////////
// callbacks
///////////////////////////////////////////////////////////////////////////////

VX_API void add_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    return s_hints->add_hint_callback(name, callback, user_data);
}

void hints_instance::add_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    if (!callback)
    {
        return;
    }

    os::lock_guard lock(data.mutex);
    hint_entry* hint = nullptr;

    if (!hint_entry_exists(name))
    {
        hint = &data.hints[name];
    }
    else
    {
        hint = &data.hints.at(name);

        // make sure callback does not already exist
        for (const auto& cb : hint->callbacks)
        {
            if (cb.callback == callback && cb.user_data == user_data)
            {
                return;
            }
        }
    }

    hint->callbacks.push_back({ callback, user_data });
}

////////////////////////////////////////

VX_API void remove_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    s_hints->remove_hint_callback(name, callback, user_data);
}

void hints_instance::remove_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    if (!callback)
    {
        return;
    }

    os::lock_guard lock(data.mutex);

    if (!hint_entry_exists(name))
    {
        return;
    }

    hint_entry* hint = &data.hints.at(name);

    for (auto it = hint->callbacks.begin(); it != hint->callbacks.end(); ++it)
    {
        if (it->callback == callback && it->user_data == user_data)
        {
            hint->callbacks.erase(it);
            break;
        }
    }
}

////////////////////////////////////////

void add_hint_callback_and_default_value(hint_t name, hint_callback callback, void* user_data, const char* default_value)
{
    s_hints->add_hint_callback_and_default_value(name, callback, user_data, default_value);
}

void hints_instance::add_hint_callback_and_default_value(hint_t name, hint_callback callback, void* user_data, const char* default_value)
{
    // set the default value first so the callback is not triggered
    set_hint_default_value(name, default_value, false);
    add_hint_callback(name, callback, user_data);
}

} // namespace hint
} // namespace app
} // namespace vx
