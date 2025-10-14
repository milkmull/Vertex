#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex/app/input/touch.hpp"

namespace vx {
namespace app {
namespace mouse {

///////////////////////////////////////////////////////////////////////////////
// hints
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

class cursor_instance_impl {};

void cursor_instance_impl_deleter::operator()(cursor_instance_impl* ptr) const noexcept
{
    if (ptr) { delete ptr; }
}

///////////////////////////////////////////////////////////////////////////////
// mouse internal
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_mouse.c#L249

bool mouse_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    data.was_touch_mouse_events = false;
    data.cursors.visible = true;

    return true;
}

////////////////////////////////////////

void mouse_instance::quit()
{
    video = nullptr;
}

//-------------------------------------------------------------------------
// Device Management
//-------------------------------------------------------------------------

size_t mouse_instance::get_mouse_index(mouse_id id) const
{
    if (!is_valid_id(id))
    {
        return VX_INVALID_INDEX;
    }

    for (size_t i = 0; i < data.mice.size(); ++i)
    {
        if (data.mice[i].id == id)
        {
            return i;
        }
    }

    return VX_INVALID_INDEX;
}

////////////////////////////////////////

const mouse_info* mouse_instance::get_mouse(mouse_id id) const
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (const mouse_info& m : data.mice)
    {
        if (m.id == id)
        {
            return &m;
        }
    }

    return nullptr;
}

mouse_info* mouse_instance::get_mouse(mouse_id id)
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (mouse_info& m : data.mice)
    {
        if (m.id == id)
        {
            return &m;
        }
    }

    return nullptr;
}

////////////////////////////////////////

void mouse_instance::add_mouse(mouse_id id, const char* name)
{
    const size_t i = get_mouse_index(id);
    if (i == VX_INVALID_INDEX)
    {
        // we already know about this mouse
        return;
    }

    VX_ASSERT(is_valid_id(id));

    mouse_info info{};
    info.id = id;
    info.name = name ? name : "Mouse";

    data.mice.push_back(info);
    send_mouse_added(id);
}

////////////////////////////////////////

void mouse_instance::remove_mouse(mouse_id id)
{
    for (auto it = data.mice.begin(); it != data.mice.end(); ++it)
    {
        if (it->id == id)
        {
            data.mice.erase(it);
            break;
        }
    }

    // Remove any mouse input sources for this id
    for (auto it = data.sources.begin(); it != data.sources.end(); ++it)
    {
        if (it->id == id)
        {
            it = data.sources.erase(it);
        }
    }

    if (!data.quitting)
    {
        send_mouse_removed(id);
    }
}

////////////////////////////////////////

bool mouse_instance::any_connected() const
{
    return !data.mice.empty();
}

////////////////////////////////////////

std::vector<mouse_id> mouse_instance::list_mice() const
{
    std::vector<mouse_id> mice(data.mice.size());

    for (size_t i = 0; i < data.mice.size(); ++i)
    {
        mice[i] = data.mice[i].id;
    }
    
    return mice;
}

////////////////////////////////////////

const char* mouse_instance::get_name(mouse_id id) const
{
    const mouse_info* m = get_mouse(id);
    return m ? m->name.c_str() : nullptr;
}

//-------------------------------------------------------------------------
// Focus (window association)
//-------------------------------------------------------------------------

video::window_id mouse_instance::get_focus() const
{
    return data.focus;
}

////////////////////////////////////////

void mouse_instance::set_focus(video::window_id w)
{
    if (data.focus == w)
    {
        return;
    }

    // See if the current window has lost focus
    if (is_valid_id(data.focus))
    {
        //send_mouse_leave_window;
    }

    data.focus = w;
    data.has_position = false;

    if (is_valid_id(data.focus))
    {
        //send_mouse_enter_window
    }

    // update cursor visibility
    redraw_cursor();
}

//-------------------------------------------------------------------------
// State (buttons + position)
//-------------------------------------------------------------------------

buttons mouse_instance::get_button_state(mouse_id id, bool include_touch) const
{
    buttons state = buttons::NONE;

    for (const input_source& source : data.sources)
    {
        if (id == GLOBAL_MOUSE_ID || id == TOUCH_MOUSE_ID)
        {
            if (include_touch || source.id == TOUCH_MOUSE_ID)
            {
                state = static_cast<buttons>(state | source.button_state);
            }
        }
        else
        {
            if (id == source.id)
            {
                state = static_cast<buttons>(state | source.button_state);
                break;
            }
        }
    }

    return state;
}

////////////////////////////////////////

buttons mouse_instance::get_state(float* x, float* y) const
{
    if (x)
    {
        *x = data.x;
    }
    if (y)
    {
        *y = data.y;
    }

    return get_button_state(GLOBAL_MOUSE_ID, true);
}

////////////////////////////////////////

buttons mouse_instance::get_relative_state(float* x, float* y)
{
    if (x)
    {
        *x = data.x_accu;
    }
    if (y)
    {
        *y = data.y_accu;
    }

    data.x_accu = 0.0f;
    data.y_accu = 0.0f;

    return get_button_state(GLOBAL_MOUSE_ID, true);
}

////////////////////////////////////////

buttons mouse_instance::get_global_state(float* x, float* y) const
{
#if VX_MOUSE_HAVE_GET_GLOBAL_MOUSE_STATE

    float tmpx, tmpy;

    // make sure these are never NULL for the backend implementations...
    if (!x)
    {
        x = &tmpx;
    }
    if (!y)
    {
        y = &tmpy;
    }

    *x = *y = 0.0f;

    //return get_button_state(GLOBAL_MOUSE_ID, true);
    return {};

#else

    return get_state(x, y);

#endif // VX_MOUSE_HAVE_GET_GLOBAL_MOUSE_STATE
}

} // namespace mouse
} // namespace app
} // namespace vx