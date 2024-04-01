#pragma once

#include "../../win32_header.h"
#include "vertex/app/display/window.h"
#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {

typedef HWND window_handel;

class window::window_impl
{
public:

    // =============== win32 class stuff ===============

    static constexpr const wchar_t* s_class_name = L"Vertex_Window";
    static size_t s_window_count;

    static void register_window_class();

    // =============== constructors ===============

    window_impl(const std::string& title, const math::vec2i& size, const math::vec2i& position, style style = style::DEFAULT);
    ~window_impl();

    window_impl(const window_impl&) = delete;
    window_impl(window_impl&&) = delete;

    window_impl& operator=(const window_impl&) = delete;
    window_impl& operator=(window_impl&&) = delete;

    const window_handle get_native_handle() const;

private:

    void on_destroy();

public:

    bool is_open() const;

    // =============== style ===============

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
    void hide();

    bool is_visible() const;

    void minimize();
    bool is_minimized() const;

    void maximize();
    bool is_maximized() const;

    void restore();

    bool request_focus();
    bool is_focused() const;

    void request_attention();

    // =============== icon ===============
    
    void set_icon(const img::image& icon);
    void clear_icon();

    // =============== mouse ===============

    math::vec2i get_mouse_position() const;
    void set_mouse_position(const math::vec2i& position);
    

private:

    HWND m_handle;

    std::queue<event> m_events;

    bool m_resizing_or_moving;
    math::vec2i m_last_size;
    math::vec2i m_last_position;

    math::vec2i m_min_size;
    math::vec2i m_max_size;

    math::vec2i m_last_mouse_position;

    HICON m_icon;

};

}
}