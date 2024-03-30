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

    static std::unique_ptr<window> create(const std::string& title, const math::vec2i& size, const math::vec2i& position);

    window() = default;
    virtual ~window() = default;

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

    // =============== events ===============

protected:

    virtual void process_events() = 0;

public:

    virtual void post_event(const event& e);
    bool pop_event(event& e, bool block = false);

    //virtual inline void set_event_callback(const event_callback_fn callback) = 0;
    ////void poll_events();
    //
    // =============== open ===============
    
    virtual bool is_open() const = 0;
    
    //// =============== close ===============
    //
    //virtual bool should_close() const = 0;
    //virtual void set_should_close(bool should_close) = 0;
    //
    // =============== title ===============
    
    virtual std::string get_title() const = 0;
    virtual void set_title(const std::string& title) = 0;
    
    // =============== position ===============
    
    virtual math::vec2i get_position() const = 0;
    virtual void set_position(const math::vec2i& position) = 0;
    
    // =============== size ===============
    
    virtual math::vec2i get_size() const = 0;
    virtual void set_size(const math::vec2i& size) = 0;
    
    //virtual math::vec2i get_min_size() const = 0;
    //virtual math::vec2i get_max_size() const = 0;
    //
    //virtual void set_min_size(const math::vec2i& size) = 0;
    //virtual void set_max_size(const math::vec2i& size) = 0;
    
    //// =============== resizable ===============
    //
    //virtual void set_resizable(bool resizable) = 0;
    //virtual bool is_resizable() const = 0;
    //
    //// =============== bordered ===============
    //
    //virtual void set_bordered(bool bordered) = 0;
    //virtual bool is_bordered() const = 0;
    //
    //// =============== always on top ===============
    //
    //virtual void set_always_on_top(bool always_on_top) = 0;
    //virtual bool is_always_on_top() const = 0;
    //
    //// =============== iconify and maximize ===============
    //
    //virtual void iconify() = 0;
    //virtual bool is_iconified() const = 0;
    //
    //virtual void maximize() = 0;
    //virtual bool is_maximized() const = 0;
    //
    //virtual void restore() = 0;
    //
    //// =============== hide and show ===============
    //
    //virtual void hide() = 0;
    //virtual void show() = 0;
    //
    //virtual bool is_visible() const = 0;
    //
    //// =============== focus ===============
    //
    //virtual void focus() = 0;
    //virtual bool is_focused() const = 0;
    //
    //// =============== attention ===============
    //
    //virtual void request_attention() = 0;
    //
    //// =============== opacity ===============
    //
    //virtual float get_opacity() const = 0;
    //virtual void set_opacity(float opacity) = 0;
    //
    //// =============== hovered ===============
    //
    //virtual bool is_hovered() const = 0;
    //
    //// =============== icon ===============
    //
    //virtual void set_icon(const img::image& icon) = 0;
    //virtual void clear_icon() = 0;
    //
    //// =============== cursor ===============
    //
    //virtual math::vec2 get_cursor_position() const = 0;
    //virtual void set_cursor_position(const math::vec2& cursor_position) = 0;
    //
    //virtual void set_cursor_shape(cursor::shape shape) = 0;
    //virtual void set_cursor_shape(const img::image& shape, int cursor_id, const math::vec2i& hotspot = math::vec2i(0, 0)) = 0;
    //virtual void clear_cursor_shape() = 0;
    //
    //virtual cursor::mode get_cursor_mode() const = 0;
    //virtual void set_cursor_mode(cursor::mode mode) = 0;
    //
    //// =============== mouse ===============
    //
    //virtual bool is_mouse_button_pressed(mouse::button button) const = 0;
    //
    //// =============== key ===============
    //
    //virtual bool is_key_pressed(keyboard::key key) const = 0;

private:

    std::queue<event> m_events;

};

}
}