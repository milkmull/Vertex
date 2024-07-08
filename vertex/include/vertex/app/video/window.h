#pragma once

#include <memory>
#include <queue>
#include <optional>

#include "video.h"
#include "../event/event.h"
#include "../event/cursor.h"

namespace vx {
namespace app {
namespace video {

class window
{
public:

    struct config
    {
        std::string title;
        math::vec2i size;
        math::vec2i position;

        bool auto_position = false;

        bool fullscreen = false;
        bool minimized = false;
        bool maximized = false;

        bool borderless = false;
        bool resizable = true;
        bool topmost = false;

        bool hidden = false;
        bool focussed = true;

        bool mouse_grabbed = false;
        bool mouse_capture = false;
    };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    static window create(const config& window_config);

    window();
    ~window();

private:

    struct state_data;
    window(const state_data& state);

    window(const window&) = delete;
    window(window&&) noexcept;

    window& operator=(const window&) = delete;
    window& operator=(window&&) noexcept;

public:

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

    void restore();

    bool is_fullscreen() const;
    bool set_fullscreen(bool fullscreen);
    bool set_fullscreen_mode(const video::display_mode& mode);

    const video::display* get_display() const;
    bool get_display_mode(video::display_mode& mode) const;

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

    struct flags
    {
        using type = uint64_t;

        static constexpr type NONE = 0;

        static constexpr type FULLSCREEN = (1 << 0);
        static constexpr type MINIMIZED = (1 << 1);
        static constexpr type MAXIMIZED = (1 << 2);

        static constexpr type BORDERLESS = (1 << 3);
        static constexpr type RESIZABLE = (1 << 4);
        static constexpr type TOPMOST = (1 << 5);

        static constexpr type HIDDEN = (1 << 6);
        static constexpr type FOCUSSED = (1 << 7);

        static constexpr type MOUSE_GRABBED = (1 << 8);
        static constexpr type MOUSE_CAPTURE = (1 << 9);
    };

    struct state_data
    {
        device_id id;

        std::string title;
        math::vec2i position;
        math::vec2i size;

        math::vec2i min_size;
        math::vec2i max_size;

        float min_aspect = 0.0f;
        float maz_aspect = 0.0f;

        // Position and size for a non-fullscreen window, including when
        // maximized or tiled. Window should return to this size and position
        // when leaving fullscreen.
        math::recti windowed;

        // Position and size for a base window in 'floating' state. Floating
        // state excludes maximized and fullscreen windows. Window should
        // return to this size and position when being restored from a
        // maximized state.
        math::recti floating;

        // Used to indicate when the position or size of the window should
        // change to match that of the floating rect after exiting a special
        // state such as fullscreen or maximized.
        bool adjust_floating_rect = false;

        flags::type flags = 0;
        flags::type pending_flags = 0;

        const video::display* last_display;

        video::display_mode requested_fullscreen_mode;
        video::display_mode current_fullscreen_mode;

        float opacity;

        bool moving_or_resizing = false;
    };

    enum class window_rect_type
    {
        CURRENT,
        WINDOWED,
        FLOATING
    };

    class window_impl;
    std::unique_ptr<window_impl> m_window;

private:

    void apply_flags(flags::type new_flags);

    void on_hide();
    void on_show();

    void on_minimize();
    void on_maximize();
    void on_restore();

};

}
}
}