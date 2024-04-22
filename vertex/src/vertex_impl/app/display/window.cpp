#include "vertex/config.h"
#include "vertex/app/display/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/app/display/win32_window.h"

#endif

/*
    State Transition Matrix
    ~~~~~~~~~~~~~~~~~~~~~~~

      A->B      |   Nutral   | Fullscreen | Minimized  | Maximized  |   Hidden   |   Shown    |  Focussed  | Unfocussed |   Moving   |  Resizing  |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Nutral     |     xx     |     A1     |     A2     |     A3     |     A4     |     A5     |     A6     |     A7     |     A8     |     A9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Fullscreen |     B0     |     xx     |     B2     |     B3     |     B4     |     B5     |     B6     |     B7     |     B8     |     B9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Minimized  |     C0     |     C1     |     xx     |     C3     |     C4     |     C5     |     C6     |     C7     |     C8     |     C9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Maximized  |     D0     |     D1     |     A2     |     xx     |     A4     |     A5     |     A6     |     A7     |     A8     |     A9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Hidden     |     E0     |     E1     |     E2     |     E3     |     xx     |     E5     |     E6     |     E7     |     E8     |     E9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Shown      |     xx     |     F1     |     A2     |     A3     |     A4     |     xx     |     A6     |     A7     |     A8     |     A9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Focussed   |     xx     |     G1     |     A2     |     A3     |     A4     |     A5     |     xx     |     A7     |     A8     |     A9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Unfocussed |     xx     |     H1     |     H2     |     H3     |     H4     |     H5     |     H6     |     xx     |     H8     |     H9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Moving     |     I0     |     I1     |     I2     |     I3     |     I4     |     I5     |     I6     |     I7     |     xx     |     I9     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+
     Resizing   |     J0     |     J1     |     J2     |     J3     |     J4     |     J5     |     J6     |     J7     |     J8     |     xx     |
    +-----------+------------+------------+------------+------------+------------+------------+------------+------------+------------+------------+

    ===========================================================================
     Ax: Nutral -> x
    ===========================================================================

    A1: Nutral -> Fullscreen:
    ----------------------------------------
    A2: Nutral -> Minimized:
        - WINDOW_FOCUS is generated, as the window has lost focus.
        - WINDOW_MINIMIZE is generated.
        - WINDOW_RESIZE is generated with the size set to 0.
        - NO WINDOW_MOVE is generated, it is assumed that the
          window is in the same position.
        - MOUSE_HOVER is generated if the cursor was inside the window
          when minimized.
    ----------------------------------------
    A3: Nutral -> Maximized
        - WINDOW_FOCUS is generated if the window was not in focus
          when maximized.
        - WINDOW_MOVE is generated with the new position.
        - MOUSE_HOVER is generated if the cursor enters the window as a
          result of the move OR resize.
        - WINDOW_MAXIMIZE is generated.
        - WINDOW_RESIZE is generated with the new size.
        - MOUSE_HOVER is generated if the cursor enters the window as a
          result of the move OR resize. This is only if the first MOUSE_HOVER
          was not generated.
        - MOUSE_MOVE is generated if the position of the mouse
          changed relative to the new window position.
    ----------------------------------------
    A4: Nutral -> Hidden
        - WINDOW_SHOW is generated.
        - MOUSE_HOVER is generated if the cursor was in the window area when
          the window was hidden.
        - WINDOW_FOCUS is generated if the window was in focus when the window
          was hidden.
    ----------------------------------------
    A5: Nutral -> Shown
        - Nothing should happen.
    ----------------------------------------
    A6: Nutral -> Focussed
        - Nothing should happen.
    ----------------------------------------
    A7: Nutral -> Unfocussed
        - WINDOW_FOCUS is generated, the wndow lost focus.
    ----------------------------------------
    A8: Nutral -> Moving
        - WINDOW_MOVE is generated with the new position.
        - MOUSE_HOVER is generated if the cursor enters or exits the window as
          a result of the move.
        - MOUSE_MOVE is generated if the position of the mouse
          changed relative to the new window position.
    ----------------------------------------
    A9: Nutral -> Resizing
        - WINDOW_SIZE is generated with the new size.
        - WINDOW_MOVE is generated if the left or top sides of the window changed.
        - MOUSE_HOVER is generated if the cursor enters or exits the window as
          a result of the size.
        - MOUSE_MOVE is generated if the position of the mouse
          changed relative to the new window position.
        
    ===========================================================================
     Bx: Fullscreen -> x
    ===========================================================================

    ===========================================================================
     Cx: Minimized -> x
    ===========================================================================

    C0: Minimized -> Nutral:
        - WINDOW_FOCUS is generated, as the window has gained focus.
        - WINDOW_RESIZE is generated ith the size set to the original size
          (before minimizing).
        - NO WINDOW_MOVE is generated, it is assumed that the
          window is in the same position.
        - MOUSE_HOVER is generated if the cursor enters the window.
    ----------------------------------------
    C1: Minimized -> Fullscreen:
    ----------------------------------------
    C3: Minimized -> Maximized:

*/

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

window::window(const std::string& title, const math::vec2i& size, const math::vec2i& position, style style)
    : m_window(std::make_unique<window_impl>(title, size, position, style)) {}

window::~window() {}

const window_handle window::get_native_handle() const
{
    return m_window->get_native_handle();
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

bool window::request_focus()
{
    return m_window->request_focus();
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

cursor window::get_cursor() const
{
    return m_window->get_cursor();
}

bool window::set_cursor(cursor cursor)
{
    return m_window->set_cursor(cursor);
}

}
}