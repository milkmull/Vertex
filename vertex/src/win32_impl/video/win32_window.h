#pragma once

#include "win32_video.h"
#include "vertex/app/video/window.h"

namespace vx {
namespace app {

class window::window_impl
{
public:

    // =============== constructors ===============

    window_impl(const config& config);
    ~window_impl();

    window_impl(const window_impl&) = delete;
    window_impl(window_impl&&) = delete;

    window_impl& operator=(const window_impl&) = delete;
    window_impl& operator=(window_impl&&) = delete;

private:

    bool create_window(const config& config);
    void on_destroy();

public:

    bool is_open() const;

private:

    // =============== style ===============

    DWORD get_window_style() const;
    DWORD get_window_ex_style() const;

public:

    void update_style(int flags, bool enable);
    bool has_style(int flag) const;

private:

    // =============== events ===============

    static LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    bool process_event(UINT Msg, WPARAM wParam, LPARAM lParam);
    void process_events();

public:

    void post_event(const event& e);
    bool pop_event(event& e, bool block);

    // =============== title ===============

    std::string get_title() const;
    void set_title(const std::string& title);

    // =============== position and size ===============

private:

    math::vec2i content_position_to_window_position(const math::vec2i& position) const;
    math::vec2i content_size_to_window_size(const math::vec2i& size) const;

public:

    math::vec2i get_position() const;
    void set_position(const math::vec2i& position);

    math::vec2i get_size() const;
    void set_size(const math::vec2i& size);

private:

    static constexpr math::vec2i s_default_min_size = math::vec2i(0);
    static constexpr math::vec2i s_default_max_size = math::vec2i(50000);

public:

    math::vec2i get_min_size() const;
    math::vec2i get_max_size() const;

    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);

    void set_resizable(bool resizable);
    bool is_resizable() const;

    // =============== window ops ===============

    void show();

    void minimize();
    bool is_minimized() const;

    void maximize();
    bool is_maximized() const;

    void restore();

    void focus();
    bool is_focused() const;

    void request_attention();

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

    HWND m_handle;

    std::queue<event> m_events;

    bool m_borderless;
    bool m_visible;
    bool m_focussed;
    bool m_floating;

    bool m_resizable;
    bool m_minimized;
    bool m_maximized;
    bool m_fullscreen;

    bool m_scale_to_monitor;
    bool m_scale_framebuffer;

    bool m_mouse_passthrough;

    bool m_resizing_or_moving;
    math::vec2i m_last_size;
    math::vec2i m_last_position;

    math::vec2i m_min_size;
    math::vec2i m_max_size;

    math::vec2i m_last_mouse_position;
    bool m_mouse_inside_window;

    //cursor m_last_cursor_object;
    HCURSOR m_last_cursor;
    bool m_cursor_visible;
    bool m_cursor_grabbed;

    HICON m_icon;

};

}
}