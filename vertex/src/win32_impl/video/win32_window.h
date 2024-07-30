#pragma once

#include "win32_video.h"
#include "vertex/app/video/window.h"

namespace vx {
namespace app {

class video::window::window_impl
{
    friend video;
    friend window;

public:

    // =============== constructors ===============

    static void create(window& w);

    window_impl(window* w);
    ~window_impl();

    window_impl(const window_impl&) = delete;
    window_impl(window_impl&&) = delete;

    window_impl& operator=(const window_impl&) = delete;
    window_impl& operator=(window_impl&&) = delete;

private:

    // =============== style ===============

    DWORD get_window_style() const;
    DWORD get_window_ex_style() const;
    void sync_window_style();

public:

    void update_style(int flags, bool enable);
    bool has_style(int flag) const;

public:

    // =============== events ===============

    static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

public:

    // =============== sync ===============

    void sync();

    // =============== title ===============

    std::string get_title() const;
    void set_title(const std::string& title);

    // =============== position and size ===============

private:

    math::vec2i content_position_to_window_position(const math::vec2i& position) const;
    math::vec2i content_size_to_window_size(const math::vec2i& size) const;

public:

    void adjust_rect_with_style(RECT& rect, window_rect_type rect_type, DWORD style, DWORD ex_style) const;
    void adjust_rect(RECT& rect, window_rect_type rect_type) const;
    void set_position_internal(UINT flags, window_rect_type rect_type);

    math::vec2i get_position() const;
    void set_position();

    math::vec2i get_size() const;
    void set_size();

public:

    math::vec2i get_min_size() const;
    math::vec2i get_max_size() const;

    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);

    void set_resizable(bool resizable);

    // =============== window ops ===============

    void show();

    void minimize();
    bool is_minimized() const;

    void maximize();
    bool is_maximized() const;

    void restore();

    bool set_fullscreen(bool fullscreen, const display* d);

    void focus();
    bool is_focused() const;

    void request_attention();

    void set_topmost(bool enabled);
    bool is_topmost() const;

    // =============== icon ===============

    bool set_icon(const uint8_t* pixels, const math::vec2i& size);
    void clear_icon();

    // =============== mouse ===============

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

    window* m_owner;
    HWND m_handle;
    HICON m_icon;

    // Used to indicate when the position or size of the window should
    // change to match that of the floating rect after exiting a special
    // state such as fullscreen or maximized.
    bool m_floating_rect_pending;

    // Used to indicate when the window was maximized before entering a new
    // state such as fullscreen. When the window is restored, if this flag
    // is set, it will return to a maximized state.
    bool m_windowed_mode_was_maximized;

    // True when losing focus
    bool m_in_window_deactivation;

    // True after clearing the contents of the window
    bool m_cleared;

    //bool m_windowed_mode_corner_rounding;
    //COLORREF m_dwma_border_color;

};

}
}