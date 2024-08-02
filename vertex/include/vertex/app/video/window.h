#pragma once

#include "video.h"

namespace vx {
namespace app {

class video::window
{
private:

    class window_impl;

private:

    friend video;
    friend window_impl;

private:

private:

    struct flags
    {
        using type = uint64_t;

        enum : type
        {
            NONE = 0,

            FULLSCREEN = (1 << 0),
            MINIMIZED = (1 << 1),
            MAXIMIZED = (1 << 2),

            BORDERLESS = (1 << 3),
            RESIZABLE = (1 << 4),
            TOPMOST = (1 << 5),

            HIDDEN = (1 << 6),
            FOCUSSED = (1 << 7),

            MOUSE_GRABBED = (1 << 8),
            MOUSE_CAPTURE = (1 << 9)
        };
    };

    struct window_rect_type
    {
        using type = uint32_t;

        enum : type
        {
            INPUT = 0,
            CURRENT,
            WINDOWED,
            FLOATING
        };
    };

public:

    window(const window_config& config);
    ~window();
    
    window(const window&) = delete;
    window(window&&) noexcept;

    window& operator=(const window&) = delete;
    window& operator=(window&&) noexcept;

private:

    bool validate() const;

    void finish_creation(flags::type new_flags);
    void apply_flags(flags::type new_flags);

public:

    device_id id() const;
    void sync();

public:

    ///////////////////////////////////////////////////////////////////////////////
    // title
    ///////////////////////////////////////////////////////////////////////////////

    const std::string& get_title() const;
    void set_title(const std::string& title);

    ///////////////////////////////////////////////////////////////////////////////
    // position and size
    ///////////////////////////////////////////////////////////////////////////////

    void set_resizable(bool resizable);
    bool is_resizable() const;

    math::vec2i get_position() const;
    void set_position(const math::vec2i& position);

    const math::vec2i& get_size() const;
    void set_size(const math::vec2i& size);

    math::vec2i get_center() const;
    math::recti get_rect() const;
    void get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const;

    const math::vec2i& get_min_size() const;
    const math::vec2i& get_max_size() const;

    void set_min_size(const math::vec2i& size);
    void set_max_size(const math::vec2i& size);

    float get_aspect_ratio() const;
    void lock_aspect_ratio(float aspect_ratio);

    ///////////////////////////////////////////////////////////////////////////////
    // bordered
    ///////////////////////////////////////////////////////////////////////////////
    
    void set_bordered(bool bordered);
    bool is_bordered() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // always on top
    ///////////////////////////////////////////////////////////////////////////////
    
    void set_always_on_top(bool always_on_top);
    bool is_always_on_top() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // window operators
    ///////////////////////////////////////////////////////////////////////////////

    void show();
    void hide();
    bool is_visible() const;
    bool is_hidden() const;

    void minimize();
    bool is_minimized() const;

    void maximize();
    bool is_maximized() const;

    void restore();

    void focus();
    bool is_focused() const;

    enum class flash_op
    {
        CANCEL,
        BRIEF,
        UNTIL_FOCUSED
    };

    void flash(flash_op operation);

public:

    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen
    ///////////////////////////////////////////////////////////////////////////////

    const display_mode* get_fullscreen_mode() const;
    bool set_fullscreen_mode(const display_mode& mode);

private:

    struct fullscreen_op
    {
        using type = uint32_t;

        enum : type
        {
            LEAVE = 0,
            ENTER = 1,
            UPDATE
        };
    };

    bool update_fullscreen_mode(fullscreen_op::type fullscreen, bool commit);

public:

    bool is_fullscreen() const;
    bool set_fullscreen(bool fullscreen);

    ///////////////////////////////////////////////////////////////////////////////
    // icon
    ///////////////////////////////////////////////////////////////////////////////

    bool set_icon(const uint8_t* pixels, const math::vec2i& size);
    void clear_icon();

private:

    device_id m_id;

    std::string m_title;
    math::vec2i m_position;
    math::vec2i m_size;

    math::vec2i m_min_size, m_max_size;
    float m_locked_aspect;

    // Position and size for a non-fullscreen window, including when
    // maximized or tiled. Window should return to this size and position
    // when leaving fullscreen.
    math::recti m_windowed_rect;

    // Position and size for a base window in 'floating' state. Floating
    // state excludes maximized and fullscreen windows. Window should
    // return to this size and position when being restored from a
    // maximized state.
    math::recti m_floating_rect;

    flags::type m_flags, m_pending_flags;

    device_id m_last_display_id;
    display_mode m_requested_fullscreen_mode, m_current_fullscreen_mode;
    bool m_requested_fullscreen_mode_set;

    bool m_fullscreen_exclusive;
    device_id m_last_fullscreen_exclusive_display_id;

    //math::vec2 m_display_scale;
    float m_opacity;

    bool m_initializing;
    bool m_destroying;
    bool m_repositioning;
    bool m_hiding;

    bool m_tiled;

    bool m_sync_requested;

private:

    std::unique_ptr<window_impl> m_impl;

private:

    ///////////////////////////////////////////////////////////////////////////////
    // window events
    ///////////////////////////////////////////////////////////////////////////////

    bool post_window_shown();
    void on_window_shown();

    bool post_window_hidden();
    void on_window_hidden();

    bool post_window_moved(int32_t x, int32_t y);
    void on_window_moved();

    bool post_window_resized(int32_t w, int32_t h);
    void on_window_resized();

    bool post_window_minimized();
    void on_window_minimized();

    bool post_window_maximized();
    void on_window_maximized();

    bool post_window_restored();
    void on_window_restored();

    bool post_window_enter_fullscreen();
    bool post_window_leave_fullscreen();

    //bool post_window_gained_focus();
    //bool post_window_lost_focus();
    //bool post_window_mouse_enter();
    //bool post_window_mouse_leave();
    bool post_window_display_changed(const display& d);
    void on_window_display_changed(const display& d);

    //bool post_window_display_scale_changed();
    bool post_window_close_requested();
    bool post_window_destroyed();

};

}
}