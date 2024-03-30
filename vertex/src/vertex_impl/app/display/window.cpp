#include <chrono>
#include <thread>

#include "vertex/config.h"
#include "vertex/app/display/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/app/display/win32_window.h"

#endif

namespace vx {
namespace app {

window::window(const std::string& title, const math::vec2i& size, const math::vec2i& position)
    : m_window(std::make_unique<window_impl>(title, size, position)) {}

window::~window()
{

}

// =============== events ===============

void window::post_event(const event& e)
{
    m_window->post_event(e);
}

bool window::pop_event(event& e, bool block)
{
    return m_window->pop_event(e, block);
}

// =============== open ===============

bool window::is_open() const
{
    return m_window->is_open();
}

// =============== title ===============

std::string window::get_title() const
{
    return m_window->get_title();
}

void window::set_title(const std::string& title)
{
    m_window->set_title(title);
}

// =============== position ===============

math::vec2i window::get_position() const
{
    return m_window->get_position();
}

void window::set_position(const math::vec2i& position)
{
    m_window->set_position(position);
}

// =============== size ===============

math::vec2i window::get_size() const
{
    return m_window->get_size();
}

void window::set_size(const math::vec2i& size)
{
    m_window->set_size(size);
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

// =============== window ops ===============

void window::show()
{
    m_window->show();
}

void window::hide()
{
    m_window->hide();
}

bool window::is_visible() const
{
    return m_window->is_visible();
}

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

// =============== focus ===============

void window::focus()
{
}

bool window::is_focused() const
{
    return true;
}

}
}