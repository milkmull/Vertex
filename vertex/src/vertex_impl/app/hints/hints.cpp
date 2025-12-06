#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace app {
namespace hint {

//=============================================================================
// initialization
//=============================================================================

hint_manager::hint_manager() = default;
hint_manager::~hint_manager() { quit(); }

bool hint_manager::init(app_instance* owner)
{
    if (app)
    {
        quit();
    }
    app = owner;
    return true;
}

void hint_manager::quit()
{
    app = nullptr;
}

//=============================================================================
// checkers
//=============================================================================

bool has_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(false);
    return s_hints_ptr->has_hint(name);
}

bool hint_manager::has_hint(hint_t name) const
{
    os::lock_guard lock(data.mutex);

    if (!hint_entry_exists(name))
    {
        return false;
    }

    const hint_entry& h = data.hints.at(name);
    return h.value != nullptr;
}

//=============================================================================
// getters
//=============================================================================

const char* get_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(nullptr);
    return s_hints_ptr->get_hint(name);
}

const char* hint_manager::get_hint(hint_t name) const
{
    os::lock_guard lock(data.mutex);

    if (hint_entry_exists(name))
    {
        const hint_entry& h = data.hints.at(name);

        if (h.value)
        {
            return h.value;
        }
    }

    return nullptr;
}

//=============================================================================
// setters
//=============================================================================

void hint_entry::update(hint_t name, const char* old_value, const char* new_value)
{
    value = new_value;

    for (const auto& cb : callbacks)
    {
        cb.callback(name, old_value, new_value, cb.user_data);
    }
}

//=============================================================================

bool set_hint(hint_t name, const char* value)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT(false);
    return s_hints_ptr->set_hint(name, value);
}

bool hint_manager::set_hint(hint_t name, const char* value)
{
    if (!value)
    {
        return false;
    }

    os::lock_guard lock(data.mutex);
    hint_entry& h = data.hints[name];

    if (!str::cstrcmp(h.value, value))
    {
        h.update(name, h.value, value);
    }

    return true;
}

//=============================================================================

void reset_hint(hint_t name)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    return s_hints_ptr->reset_hint(name);
}

void hint_manager::reset_hint(hint_t name)
{
    os::lock_guard lock(data.mutex);

    if (!hint_entry_exists(name))
    {
        return;
    }

    hint_entry& h = data.hints[name];

    if (h.value)
    {
        h.update(name, h.value, nullptr);
    }

    // if this hint has no callbacks we can remove the entry
    if (h.callbacks.empty())
    {
        data.hints.erase(name);
    }
}

//=============================================================================
// callbacks
//=============================================================================

void add_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    s_hints_ptr->add_hint_callback(name, callback, user_data);
}

void hint_manager::add_hint_callback(hint_t name, hint_callback callback, void* user_data)
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

    // add the callback
    hint->callbacks.push_back({ callback, user_data });
    // call it with the current value
    callback(name, hint->value, hint->value, user_data);
}

//=============================================================================

void remove_hint_callback(hint_t name, hint_callback callback, void* user_data)
{
    VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID();
    s_hints_ptr->remove_hint_callback(name, callback, user_data);
}

void hint_manager::remove_hint_callback(hint_t name, hint_callback callback, void* user_data)
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

} // namespace hint
} // namespace app
} // namespace vx
