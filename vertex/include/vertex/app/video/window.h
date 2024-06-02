#pragma once

#include <memory>
#include <queue>

#include "video.h"
#include "../event/event.h"
#include "../event/cursor.h"

namespace vx {
namespace app {

class window
{
public:

    enum style : int
    {
        RESIZABLE         = 1 << 0, // Title bar + resizable border + maximize button
        VISIBLE           = 1 << 1,
        DECORATED         = 1 << 2,
        FOCUSED           = 1 << 3,
        AUTO_ICONIFY      = 1 << 4,
        FLOATING          = 1 << 5,
        MAXIMIZED         = 1 << 6,
        CENTER_CURSOR     = 1 << 7,
        FOCUS_ON_SHOW     = 1 << 8,
        MOUSE_PASSTHROUGH = 1 << 9,
        SCALE_TO_MONITOR  = 1 << 10,
        SCALE_FRAMEBUFFER = 1 << 11,

        DEFAULT           = RESIZABLE | VISIBLE | DECORATED | FOCUSED // Default window style
    };

    struct config
    {
        std::string title;
        math::vec2i size;
        math::vec2i position = math::vec2i(VX_DEFAULT_INT);

        struct
        {            
            bool borderless = false;
            bool visible = true;
            bool focused = true;
            bool floating = false;

            bool resizable = true;
            bool maximized = false;
            bool fullscreen = false;

            bool scale_to_monitor = true;
            bool scale_framebuffer = false;

            bool center_cursor = false;
            bool mouse_passthrough = false;
        } hint;


    };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    window(const config& config);
    ~window();

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

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

    math::recti get_rect() const;
    
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
    
    void minimize();
    bool is_minimized() const;
    
    void maximize();
    bool is_maximized() const;

    bool is_fullscreen() const;
    
    void restore();

    void focus();
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

    //cursor get_cursor() const;
    //bool set_cursor(cursor cursor);

    bool is_cursor_grabbed() const;
    void set_cursor_grabbed(bool grabbed);

private:

    class window_impl;
    std::unique_ptr<window_impl> m_window;

};

}
}