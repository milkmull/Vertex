#include "vertex/config.h"
#include "vertex/app/video/window.h"
#include "vertex/app/event/event.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/video/win32_window.h"

#endif

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

window window::create(const config& window_config)
{
    state_data state{};

    state.title = window_config.title;
    state.position = window_config.position;
    state.size = window_config.size;

    // Set the windowed size and position for the window

    if (state.size.x <= 0)
    {
        state.size.x = 1;
    }
    if (state.size.y <= 0)
    {
        state.size.y = 1;
    }

    const video::display* display = nullptr;
    math::recti bounds;

    video::update_displays();

    // If auto position is true, we center the window on the display

    if (window_config.auto_position)
    {
        display = video::get_display_for_rect(math::recti(state.position, state.size));

        if (!display)
        {
            display = video::get_primary_display();
        }

        bounds = display->get_work_area();

        // Center on display
        state.position = bounds.position + (bounds.size - state.size) / 2;
    }

    state.floating.position = state.windowed.position = state.position;
    state.floating.size = state.windowed.size = state.size;

    // Set fullscreen size and position if desired

    //if (window_config.fullscreen)
    //{
    //    display = video::get_display_for_point(window_config.position);
    //
    //    if (!display)
    //    {
    //        display = video::get_primary_display();
    //    }
    //
    //    display->get_work_area(bounds);
    //
    //    state.position = bounds.position;
    //    state.size = bounds.size;
    //}

    // Creation flags
    flags::type creation_flags = flags::HIDDEN;

    // Mask out some flags to be applied after creation

    if (window_config.borderless)
    {
        creation_flags |= flags::BORDERLESS;
    }
    if (window_config.resizable)
    {
        creation_flags |= flags::RESIZABLE;
    }
    if (window_config.topmost)
    {
        creation_flags |= flags::TOPMOST;
    }
    if (window_config.fullscreen)
    {
        creation_flags |= flags::FULLSCREEN;
    }

    state.flags = creation_flags;

    window w(state);



    return w;
}

window::window() {}
window::~window() {}

window::window(const state_data& state) : m_window(std::make_unique<window_impl>(this, state)) {}

window::window(window&& w) noexcept : m_window(std::move(w.m_window)) {}

window& window::operator=(window&& w) noexcept
{
    m_window = std::move(w.m_window);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// event
///////////////////////////////////////////////////////////////////////////////

void window::post_event(const event& e)
{
    m_window->post_event(e);
}

bool window::pop_event(event& e, bool block)
{
    return m_window->pop_event(e, block);
}

///////////////////////////////////////////////////////////////////////////////
// open
///////////////////////////////////////////////////////////////////////////////

bool window::is_open() const
{
    return m_window->is_open();
}

///////////////////////////////////////////////////////////////////////////////
// window style
///////////////////////////////////////////////////////////////////////////////

void window::set_resizable(bool resizable)
{
    m_window->set_resizable(resizable);
}

bool window::is_resizable() const
{
    return m_window->is_resizable();
}

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

std::string window::get_title() const
{
    return m_window->get_title();
}

void window::set_title(const std::string& title)
{
    m_window->set_title(title);
}

///////////////////////////////////////////////////////////////////////////////
// position and size
///////////////////////////////////////////////////////////////////////////////

math::vec2i window::get_position() const
{
    // test what happens when window display fullscreen mode changes
    return m_window->get_position();
}

void window::set_position(const math::vec2i& position)
{
    m_window->m_state.floating.position = position;
    m_window->set_position(position);
}

math::vec2i window::get_size() const
{
    return m_window->get_size();
}

void window::set_size(const math::vec2i& size)
{
    m_window->set_size(size);
}

math::recti window::get_rect() const
{
    return math::recti(get_position(), get_size());
}

math::vec2i window::get_min_size() const
{
    return m_window->get_min_size();
}

math::vec2i window::get_max_size() const
{
    return m_window->get_max_size();
}

void window::set_min_size(const math::vec2i& size)
{
    math::vec2i min_size = size;

    if (min_size.x < 0 || min_size.y < 0)
    {
        // error
        return;
    }

    const math::vec2i max_size = get_max_size();

    if ((min_size.x && min_size.x > max_size.x) || (min_size.y && min_size.y > max_size.y))
    {
        // error
        return;
    }

    m_window->set_min_size(min_size);
}

void window::set_max_size(const math::vec2i& size)
{
    math::vec2i max_size = size;

    if (max_size.x < 0 || max_size.y < 0)
    {
        // error
        return;
    }

    const math::vec2i min_size = get_min_size();

    if ((max_size.x && max_size.x < min_size.x) || (max_size.y && max_size.y < min_size.y))
    {
        // error
        return;
    }

    m_window->set_max_size(size);
}

///////////////////////////////////////////////////////////////////////////////
// window ops
///////////////////////////////////////////////////////////////////////////////

void window::minimize()
{
    m_window->minimize();
}

bool window::is_minimized() const
{
    return m_window->is_minimized();
}

void window::maximize()
{
    m_window->maximize();
}

bool window::is_maximized() const
{
    return m_window->is_maximized();
}

void window::restore()
{
    m_window->restore();
}

bool window::is_fullscreen() const
{
    return m_window->is_fullscreen();
}

bool window::set_fullscreen(bool fullscreen)
{
    return true;
}

bool window::set_fullscreen_mode(const video::display_mode& mode)
{
    //if (!mode.display || !mode.display->has_mode(mode))
    //{
    //    return false;
    //}
    //
    //m_window->m_state.requested_fullscreen_mode = mode;



    return true;
}

const video::display* window::get_display() const
{
    return video::get_display_for_window(this);
}

bool window::get_display_mode(video::display_mode& mode) const
{
    //const video::display* display = get_display();
    //if (!display)
    //{
    //    return false;
    //}
    //
    //return display->get_mode(mode);
    return true;
}

void window::focus()
{
    m_window->focus();
}

bool window::is_focused() const
{
    return m_window->is_focused();
}

void window::request_attention()
{
    m_window->request_attention();
}

///////////////////////////////////////////////////////////////////////////////
// icon
///////////////////////////////////////////////////////////////////////////////

bool window::set_icon(const uint8_t* pixels, const math::vec2i& size)
{
    return m_window->set_icon(pixels, size);
}

void window::clear_icon()
{
    m_window->clear_icon();
}

///////////////////////////////////////////////////////////////////////////////
// mouse
///////////////////////////////////////////////////////////////////////////////

math::vec2i window::get_mouse_position() const
{
    return m_window->get_mouse_position();
}

void window::set_mouse_position(const math::vec2i& position)
{
    m_window->set_mouse_position(position);
}

bool window::is_hovered() const
{
    return m_window->is_hovered();
}

bool window::get_cursor_visibility() const
{
    return m_window->get_cursor_visibility();
}

void window::set_cursor_visibility(bool visible)
{
    m_window->set_cursor_visibility(visible);
}

//cursor window::get_cursor() const
//{
//    return m_window->get_cursor();
//}
//
//bool window::set_cursor(cursor cursor)
//{
//    return true;// m_window->set_cursor(cursor);
//}

void window::apply_flags(flags::type new_flags)
{
    if (!(new_flags & (flags::MINIMIZED | flags::MAXIMIZED)))
    {
        restore();
    }
    if (new_flags & flags::MAXIMIZED)
    {
        maximize();
    }

    //SDL_SetWindowFullscreen(window, (flags & SDL_WINDOW_FULLSCREEN) != 0);

    if (new_flags & flags::MINIMIZED)
    {
        minimize();
    }
}

void window::on_hide()
{

}

void window::on_show()
{
    if (!(m_window->m_state.flags & flags::HIDDEN))
    {
        return;
    }

    m_window->m_state.flags &= ~(flags::HIDDEN | flags::MINIMIZED);

    {
        event e {};
        e.type = event_type::WINDOW_SHOWN;
        e.window_event.window_id = m_window->m_state.id;
        event::poll_event(e);
    }

    apply_flags(m_window->m_state.pending_flags);
    m_window->m_state.pending_flags = 0;
}

void window::on_minimize()
{
    if (!(m_window->m_state.flags & flags::MINIMIZED))
    {
        return;
    }

    m_window->m_state.flags &= ~flags::MAXIMIZED;
    m_window->m_state.flags |= flags::MINIMIZED;

    {
        event e {};
        e.type = event_type::WINDOW_MINIMIZED;
        e.window_event.window_id = m_window->m_state.id;
        event::poll_event(e);
    }

    //if (window->flags & SDL_WINDOW_FULLSCREEN) {
    //    SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, SDL_FALSE);
    //}
}

void window::on_maximize()
{
    if (!(m_window->m_state.flags & flags::MAXIMIZED))
    {
        return;
    }

    m_window->m_state.flags &= ~flags::MINIMIZED;
    m_window->m_state.flags |= flags::MAXIMIZED;

    {
        event e {};
        e.type = event_type::WINDOW_MAXIMIZED;
        e.window_event.window_id = m_window->m_state.id;
        event::poll_event(e);
    }
}

void window::on_restore()
{
}

}
}
}