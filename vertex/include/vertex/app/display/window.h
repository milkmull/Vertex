#pragma once

#include <queue>

#include "vertex/app/init.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec2i.h"
#include "vertex/image/image.h"

#include "../input/event.h"
#include "../input/cursor.h"

namespace vx {
namespace app {

class window
{
public:

    // =============== constructors ===============

    window(const std::string& title, const math::vec2i& size, const math::vec2i& position);
    ~window();

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

    // =============== events ===============

    void post_event(const event& e);
    bool pop_event(event& e, bool block = false);

    //inline void set_event_callback(const event_callback_fn callback);
    ////void poll_events();
    //
    // =============== open ===============
    
    bool is_open() const;
    
    //// =============== close ===============
    //
    //bool should_close() const;
    //void set_should_close(bool should_close);
    //
    // =============== title ===============
    
    std::string get_title() const;
    void set_title(const std::string& title);
    
    // =============== position ===============
    
    math::vec2i get_position() const;
    void set_position(const math::vec2i& position);
    
    // =============== size ===============
    
    math::vec2i get_size() const;
    void set_size(const math::vec2i& size);
    
    math::vec2i get_min_size() const;
    math::vec2i get_max_size() const;
    
    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);
    
    //// =============== resizable ===============
    //
    //void set_resizable(bool resizable);
    //bool is_resizable() const;
    //
    //// =============== bordered ===============
    //
    //void set_bordered(bool bordered);
    //bool is_bordered() const;
    //
    //// =============== always on top ===============
    //
    //void set_always_on_top(bool always_on_top);
    //bool is_always_on_top() const;
    //
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
    
    //// =============== opacity ===============
    //
    //float get_opacity() const;
    //void set_opacity(float opacity);
    //
    //// =============== hovered ===============
    //
    //bool is_hovered() const;
    //
    // =============== icon ===============
    
    void set_icon(const img::image& icon);
    void clear_icon();
    
    //// =============== cursor ===============
    //
    //math::vec2 get_cursor_position() const;
    //void set_cursor_position(const math::vec2& cursor_position);
    //
    //void set_cursor_shape(cursor::shape shape);
    //void set_cursor_shape(const img::image& shape, int cursor_id, const math::vec2i& hotspot = math::vec2i(0, 0));
    //void clear_cursor_shape();
    //
    //cursor::mode get_cursor_mode() const;
    //void set_cursor_mode(cursor::mode mode);
    //
    //// =============== mouse ===============
    //
    //bool is_mouse_button_pressed(mouse::button button) const;
    //
    //// =============== key ===============
    //
    //bool is_key_pressed(keyboard::key key) const;

private:

    class window_impl;
    std::unique_ptr<window_impl> m_window;

};

}
}