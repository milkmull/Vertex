#pragma once

#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex_impl/app/video/_platform/windows/windows_video.hpp"

namespace vx {
namespace app {
namespace video {

#define IS_RECT_EMPTY(rect) ((rect.right <= rect.left) || (rect.bottom <= rect.top))

//=============================================================================
// data
//=============================================================================

struct window_impl_data
{
    HWND handle;
    HICON icon;

    // True when we are expecting to receive a resize event
    bool expected_resize;

    // Used to indicate when the position or size of the window should
    // change to match that of the floating rect after exiting a special
    // state such as fullscreen or maximized.
    bool floating_rect_pending;

    // Used to indicate when the window was maximized before entering a new
    // state such as fullscreen. When the window is restored, if this flag
    // is set, it will return to a maximized state.
    bool windowed_mode_was_maximized;

    // True when losing focus
    bool losing_focus;

    // True after clearing the contents of the window
    bool cleared;

    DWM_WINDOW_CORNER_PREFERENCE windowed_mode_corner_rounding;
    COLORREF dwma_border_color;
};

//=============================================================================
// impl
//=============================================================================

class window_instance_impl
{
public:

    //=============================================================================
    // window proc
    //=============================================================================

    static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    //=============================================================================
    // creation
    //=============================================================================

    bool create(window_instance* owner) { return true; }
    void destroy() {}

    //=============================================================================
    // sync
    //=============================================================================

    void sync() {}

    //=============================================================================
    // style
    //=============================================================================

    DWORD get_window_style() const;
    DWORD get_window_ex_style() const;
    void sync_window_style();

    //=============================================================================
    // title
    //=============================================================================

    std::string get_title() const { return ""; }
    bool set_title(const std::string& title) { return false; }

    //=============================================================================
    // position and size
    //=============================================================================

    bool adjust_rect_with_style(RECT& rect, window_rect_type rect_type, DWORD style, DWORD ex_style) const;
    bool adjust_rect(RECT& rect, window_rect_type rect_type) const;
    void set_position_internal(UINT flags, window_rect_type rect_type);

    bool set_position() { return false; }
    bool set_size() { return false; }

    bool get_size_in_pixels(int* w, int* h) const { return false; }

    bool get_border_size(int* left, int* right, int* bottom, int* top) const { return false; }

    bool set_resizable(bool resizable) { return false; }
    bool set_bordered() { return false; }
    bool set_always_on_top() { return false; }

    //=============================================================================
    // window operators
    //=============================================================================

    void show() {}
    void hide() {}

    void minimize() {}
    void maximize() {}

    void restore() {}
    bool raise() { return false; }

    typename window_instance::fullscreen_result set_fullscreen(typename window_instance::fullscreen_op::type fullscreen, display_instance& display) { return (typename window_instance::fullscreen_result)0; }

    void focus() {}
    bool is_focused() const { return false; }

    bool flash(window_flash_op operation) { return false; }

    void set_topmost(bool enabled) {}
    bool is_topmost() const { return false; }

    //=============================================================================
    // icon
    //=============================================================================

    bool set_icon(const argb_surface& surf) { return false; }
    void clear_icon() {}

    //=============================================================================
    // shape
    //=============================================================================

    bool update_shape(const argb_surface& shape) { return false; }
    bool set_opacity(float opacity) { return false; }

    //=============================================================================
    // icc profile
    //=============================================================================

    std::vector<uint8_t> get_icc_profile() const { return {}; }

    //=============================================================================
    // grab
    //=============================================================================

    bool set_mouse_grab(bool grabbed) { return false; }
    bool set_keyboard_grab(bool grabbed) { return false; }

    //=============================================================================
    // mouse
    //=============================================================================

    bool set_focusable(bool focusable) { return false; }
    bool set_mouse_rect(const math::recti& rect) { return false; }

    void set_mouse_tracking(bool enabled) {}
    void update_mouse_tracking() {}

    math::vec2i get_mouse_position() const { return {}; }
    void set_mouse_position(const math::vec2i& position) {}

    bool is_hovered() const {}

    bool get_cursor_visibility() const { return false; }
    void set_cursor_visibility(bool visible) {}

    //cursor get_cursor() const;
    //bool set_cursor(cursor cursor);

    bool is_cursor_grabbed() const { return false; }
    void set_cursor_grabbed(bool grabbed) {}

    //=============================================================================
    // drag and drop
    //=============================================================================

    bool toggle_drag_and_drop(bool enabled) { return false; }

    //=============================================================================
    // events
    //=============================================================================

    void send_wakeup_event() {}
    void on_window_mouse_enter() {}

    //=============================================================================
    // data
    //=============================================================================

    window_instance* window;
    window_impl_data data;
};

} // namespace video
} // namespace app
} // namespace vx