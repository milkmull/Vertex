#pragma once

#if defined(VX_APPLICATION)

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

    struct window_specs
    {
        const char* title = "Vertex";

        math::vec2i position;
        math::vec2i size;
    };

public:

    // =============== constructors ===============

    window(const window_specs& specs);
    ~window();

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

private:

    void open(const window_specs& specs);
    void close();

public:

    inline void set_event_callback(const event_callback callback) { m_event_callback = callback; }
    void poll_events();

    // =============== open ===============

    inline bool is_open() const { return m_window != nullptr; }

    // =============== close ===============

    bool should_close() const;
    void set_should_close(bool should_close);

    // =============== title ===============

    const char* get_title() const;
    void set_title(const char* title);

    // =============== position ===============

    math::vec2i get_position() const;
    void set_position(const math::vec2i& pos);

    // =============== size ===============

    math::vec2i get_size() const;
    void set_size(const math::vec2i& size);

    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);

    // =============== resizable ===============

    void set_resizable(bool resizable);
    bool is_resizable() const;

    // =============== bordered ===============

    void set_bordered(bool bordered);
    bool is_bordered() const;

    // =============== always on top ===============

    void set_always_on_top(bool always_on_top);
    bool is_always_on_top() const;

    // =============== iconify and maximize ===============

    void iconify();
    bool is_iconified() const;

    void maximize();
    bool is_maximized() const;

    void restore();

    // =============== hide and show ===============

    void hide();
    void show();

    bool is_visible() const;

    // =============== focus ===============

    void focus();
    bool is_focused() const;

    // =============== attention ===============

    void request_attention();

    // =============== opacity ===============

    float get_opacity() const;
    void set_opacity(float opacity);

    // =============== hovered ===============

    bool is_hovered() const;

    // =============== icon ===============

    void set_icon(const img::image& icon);
    void clear_icon();

    // =============== cursor ===============

    math::vec2 get_cursor_position() const;
    void set_cursor_position(const math::vec2& cursor_position);

    void set_cursor_shape(cursor::shape shape);
    void set_cursor_shape(const img::image& shape, int cursor_id, const math::vec2i& hotspot = math::vec2i(0, 0));
    void clear_cursor_shape();

    cursor::mode get_cursor_mode() const;
    void set_cursor_mode(cursor::mode mode);

    // =============== mouse ===============

    bool is_mouse_button_pressed(mouse::button button) const;

    // =============== key ===============

    bool is_key_pressed(keyboard::key key) const;

private:

    static int s_window_count;

private:

    void* m_window = nullptr;
    event_callback_fn m_event_callback = nullptr;

};

}
}

#endif