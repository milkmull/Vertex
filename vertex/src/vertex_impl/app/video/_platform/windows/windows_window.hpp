#pragma once

#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex_impl/app/video/_platform/windows/windows_video.hpp"

namespace vx {
namespace app {
namespace video {

#define IS_RECT_EMPTY(rect) ((rect.right <= rect.left) || (rect.bottom <= rect.top))

class window_impl
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // window proc
    ///////////////////////////////////////////////////////////////////////////////

    static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

public:

    ///////////////////////////////////////////////////////////////////////////////
    // creation
    ///////////////////////////////////////////////////////////////////////////////

    window_impl();
    ~window_impl();

    window_impl(const window_impl&) = delete;
    window_impl(window_impl&&) = delete;

    window_impl& operator=(const window_impl&) = delete;
    window_impl& operator=(window_impl&&) = delete;

    void sync();

private:

    bool create(window* w);
    void destroy();
    bool validate() const;

private:

    ///////////////////////////////////////////////////////////////////////////////
    // style
    ///////////////////////////////////////////////////////////////////////////////

    DWORD get_window_style() const;
    DWORD get_window_ex_style() const;
    void sync_window_style();

public:

    ///////////////////////////////////////////////////////////////////////////////
    // title
    ///////////////////////////////////////////////////////////////////////////////

    std::string get_title() const;
    void set_title(const std::string& title);

    ///////////////////////////////////////////////////////////////////////////////
    // position and size
    ///////////////////////////////////////////////////////////////////////////////

    bool adjust_rect_with_style(RECT& rect, window::rect_type rect_type, DWORD style, DWORD ex_style) const;
    bool adjust_rect(RECT& rect, window::rect_type rect_type) const;
    void set_position_internal(UINT flags, window::rect_type rect_type);

    void set_position();
    bool set_size();

    void get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const;

    void set_resizable();
    void set_bordered();
    void set_always_on_top();

    ///////////////////////////////////////////////////////////////////////////////
    // window operators
    ///////////////////////////////////////////////////////////////////////////////

    void show();
    void hide();

    void minimize();
    void maximize();

    void restore();

    bool set_fullscreen(window::fullscreen_op fullscreen, const display* d);

    void focus();
    bool is_focused() const;

    void flash(window::flash_op operation);

    void set_topmost(bool enabled);
    bool is_topmost() const;

    ///////////////////////////////////////////////////////////////////////////////
    // icon
    ///////////////////////////////////////////////////////////////////////////////

    bool set_icon(const pixel::surface_rgba8& surf);
    void clear_icon();

    ///////////////////////////////////////////////////////////////////////////////
    // mouse
    ///////////////////////////////////////////////////////////////////////////////

private:

    void set_mouse_tracking(bool enabled);
    void update_mouse_tracking();

public:

    math::vec2i get_mouse_position() const;
    void set_mouse_position(const math::vec2i& position);

    bool is_hovered() const;

    bool get_cursor_visibility() const;
    void set_cursor_visibility(bool visible);

    //cursor get_cursor() const;
    //bool set_cursor(cursor cursor);

    bool is_cursor_grabbed() const;
    void set_cursor_grabbed(bool grabbed);

private:

    ///////////////////////////////////////////////////////////////////////////////
    // member data
    ///////////////////////////////////////////////////////////////////////////////

    friend window;
    window* m_owner;

    HWND m_handle;
    HICON m_icon;

    // True when we are expecting to receive a resize event
    bool m_expected_resize;

    // Used to indicate when the position or size of the window should
    // change to match that of the floating rect after exiting a special
    // state such as fullscreen or maximized.
    bool m_floating_rect_pending;

    // Used to indicate when the window was maximized before entering a new
    // state such as fullscreen. When the window is restored, if this flag
    // is set, it will return to a maximized state.
    bool m_windowed_mode_was_maximized;

    // True when losing focus
    bool m_losing_focus;

    // True after clearing the contents of the window
    bool m_cleared;

    DWM_WINDOW_CORNER_PREFERENCE m_windowed_mode_corner_rounding;
    COLORREF m_dwma_border_color;
};

} // namespace video
} // namespace app
} // namespace vx