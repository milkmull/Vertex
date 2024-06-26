#include "vertex/config.h"
#include "vertex/app/video/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/video/win32_window.h"

#endif

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

window::window(const config& config)
    : m_window(std::make_unique<window_impl>(config)) {}

window::~window() {}

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
    return m_window->get_position();
}

void window::set_position(const math::vec2i& position)
{
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
    m_window->set_min_size(size);
}

void window::set_max_size(const math::vec2i& size)
{
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

bool window::is_fullscreen() const
{
    return false;
}

void window::restore()
{
    m_window->restore();
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

}
}