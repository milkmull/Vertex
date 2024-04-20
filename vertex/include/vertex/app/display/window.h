#pragma once

#include <memory>
#include <queue>

#include "../input/event.h"
#include "../input/cursor.h"

namespace vx {
namespace app {

using window_handle = void*;

class window
{
public:

    enum style : int
    {
        TITLEBAR = 1 << 0, // Title bar + fixed border
        RESIZE   = 1 << 1, // Title bar + resizable border + maximize button
        CLOSE    = 1 << 2, // Title bar + close button

        DEFAULT = TITLEBAR | RESIZE | CLOSE // Default window style
    };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    window(const std::string& title, const math::vec2i& size, const math::vec2i& position, style style = style::DEFAULT);
    ~window();

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

    const window_handle get_native_handle() const;

    ///////////////////////////////////////////////////////////////////////////////
    // event
    ///////////////////////////////////////////////////////////////////////////////

    void post_event(const event& e);
    bool pop_event(event& e, bool block = false);

    //inline void set_event_callback(const event_callback_fn callback);
    ////void poll_events();
    //
    ///////////////////////////////////////////////////////////////////////////////
    // open
    ///////////////////////////////////////////////////////////////////////////////
    
    bool is_open() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // close
    ///////////////////////////////////////////////////////////////////////////////
    //
    //bool should_close() const;
    //void set_should_close(bool should_close);

    ///////////////////////////////////////////////////////////////////////////////
    // window style
    ///////////////////////////////////////////////////////////////////////////////

    void set_resizable(bool resizable);
    bool is_resizable() const;

    ///////////////////////////////////////////////////////////////////////////////
    // title
    ///////////////////////////////////////////////////////////////////////////////
    
    std::string get_title() const;
    void set_title(const std::string& title);
    
    ///////////////////////////////////////////////////////////////////////////////
    // position and size
    ///////////////////////////////////////////////////////////////////////////////
    
    math::vec2i get_position() const;
    void set_position(const math::vec2i& position);

    math::vec2i get_size() const;
    void set_size(const math::vec2i& size);
    
    math::vec2i get_min_size() const;
    math::vec2i get_max_size() const;
    
    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);
    
    ///////////////////////////////////////////////////////////////////////////////
    // bordered
    ///////////////////////////////////////////////////////////////////////////////
    //
    //void set_bordered(bool bordered);
    //bool is_bordered() const;
    //
    ///////////////////////////////////////////////////////////////////////////////
    // always on top
    ///////////////////////////////////////////////////////////////////////////////
    //
    //void set_always_on_top(bool always_on_top);
    //bool is_always_on_top() const;
    //
    ///////////////////////////////////////////////////////////////////////////////
    // window operators
    ///////////////////////////////////////////////////////////////////////////////
    
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
    
    ///////////////////////////////////////////////////////////////////////////////
    // icon
    ///////////////////////////////////////////////////////////////////////////////
    
    bool set_icon(const uint8_t* pixels, const math::vec2i& size);
    void clear_icon();
    
    ///////////////////////////////////////////////////////////////////////////////
    // mouse
    ///////////////////////////////////////////////////////////////////////////////
    
    math::vec2i get_mouse_position() const;
    void set_mouse_position(const math::vec2i& position);

    bool is_hovered() const;

    bool get_cursor_visibility() const;
    void set_cursor_visibility(bool visible);

    cursor get_cursor() const;
    bool set_cursor(cursor cursor);
    
    //void set_cursor_shape(cursor::shape shape);
    //void set_cursor_shape(const img::image& shape, int cursor_id, const math::vec2i& hotspot = math::vec2i(0, 0));
    //void clear_cursor_shape();
    //

private:

    class window_impl;
    std::unique_ptr<window_impl> m_window;

};

}
}