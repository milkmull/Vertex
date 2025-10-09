#pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace app {
namespace video {

class window
{
//private:
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // helper defs
//    ///////////////////////////////////////////////////////////////////////////////
//
//    struct flags
//    {
//        using type = uint64_t;
//
//        enum : type
//        {
//            NONE            = 0,
//
//            FULLSCREEN      = (1 << 0),
//            MINIMIZED       = (1 << 1),
//            MAXIMIZED       = (1 << 2),
//
//            BORDERLESS      = (1 << 3),
//            RESIZABLE       = (1 << 4),
//            TOPMOST         = (1 << 5),
//
//            HIDDEN          = (1 << 6),
//            FOCUSSED        = (1 << 7),
//
//            MOUSE_GRABBED   = (1 << 8),
//            MOUSE_CAPTURE   = (1 << 9)
//        };
//    };
//
//    enum rect_type
//    {
//        INPUT = 0,
//        CURRENT,
//        WINDOWED,
//        FLOATING
//    };
//
//private:
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // creation
//    ///////////////////////////////////////////////////////////////////////////////
//
//    window();
//    ~window();
//
//    window(const window&) = delete;
//    window(window&&) noexcept;
//
//    window& operator=(const window&) = delete;
//    window& operator=(window&&) noexcept;
//
//private:
//
//    bool create(const window_config& config);
//    void finish_creation(flags::type new_flags);
//    void destroy();
//
//    bool validate() const;
//    void apply_flags(flags::type new_flags);
//
//public:
//
//    window_id id() const;
//
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // sync
//    ///////////////////////////////////////////////////////////////////////////////
//
//    void sync();
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // title
//    ///////////////////////////////////////////////////////////////////////////////
//
//    const std::string& get_title() const;
//    void set_title(const std::string& title);
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // position and size
//    ///////////////////////////////////////////////////////////////////////////////
//
//    void set_resizable(bool resizable);
//    bool is_resizable() const;
//
//    math::vec2i get_position() const;
//    void set_position(const math::vec2i& position);
//
//    const math::vec2i& get_size() const;
//    void set_size(const math::vec2i& size);
//
//    math::vec2i get_center() const;
//    math::recti get_rect() const;
//    void get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const;
//
//    const math::vec2i& get_min_size() const;
//    const math::vec2i& get_max_size() const;
//
//    bool set_min_size(const math::vec2i& size);
//    bool set_max_size(const math::vec2i& size);
//
//    float get_aspect_ratio() const;
//    void lock_aspect_ratio(float aspect_ratio);
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // bordered
//    ///////////////////////////////////////////////////////////////////////////////
//
//    void set_bordered(bool bordered);
//    bool is_bordered() const;
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // always on top
//    ///////////////////////////////////////////////////////////////////////////////
//
//    void set_always_on_top(bool always_on_top);
//    bool is_always_on_top() const;
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // window operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    void show();
//    void hide();
//    bool is_visible() const;
//    bool is_hidden() const;
//
//    void minimize();
//    bool is_minimized() const;
//
//    void maximize();
//    bool is_maximized() const;
//
//    void restore();
//
//    void focus();
//    bool is_focused() const;
//
//    enum class flash_op
//    {
//        CANCEL,
//        BRIEF,
//        UNTIL_FOCUSED
//    };
//
//    void flash(flash_op operation);
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // fullscreen
//    ///////////////////////////////////////////////////////////////////////////////
//
//    bool is_fullscreen() const;
//    bool set_fullscreen(bool fullscreen);
//
//    const display_mode& get_fullscreen_mode() const;
//    bool set_fullscreen_mode(const display_mode& mode);
//
//private:
//
//    enum fullscreen_op
//    {
//        LEAVE = 0,
//        ENTER = 1,
//        UPDATE
//    };
//
//    bool update_fullscreen_mode(fullscreen_op fullscreen, bool commit);
//
//public:
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // icon
//    ///////////////////////////////////////////////////////////////////////////////
//
//    bool set_icon(const pixel::surface_rgba8& surf);
//    void clear_icon();
//
//private:
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // member data
//    ///////////////////////////////////////////////////////////////////////////////
//
//    window_id m_id;
//
//    std::string m_title;
//    math::vec2i m_position;
//    math::vec2i m_size;
//
//    math::vec2i m_min_size, m_max_size;
//    float m_locked_aspect;
//
//    // Position and size of the window in any windowed state.
//    // This includes maximized or tiled states, but excludes fullscreen.
//    // This is the position and size that the window should return to
//    // when leaving fullscreen state.
//    math::recti m_windowed_rect;
//
//    // Position and size of the window in a normal "floating" state
//    // (not maximized, fullscreen, or tiled). This is the position and
//    // size that the window should return to when restored.
//    math::recti m_floating_rect;
//
//    // The most recently requested position and size of the window.
//    // This is a temporary holding value used after the front end requests
//    // a move or resize, but before the backend has applied the change.
//    // Once the backend confirms the new position and size, this rect is
//    // replaced by the actual window geometry.
//    math::recti m_pending_rect;
//
//    //  True if the frontend requested a move that has not yet been confirmed.
//    //  When true, GetPosition() should return m_pending_rect.x/y, because
//    //  the system may never explicitly confirm the new position.
//    //  (Some window managers do not reliably send move notifications.)
//    //
//    //  Why only for position:
//    //  - Position requests are best effort. Some systems ignore or adjust
//    //    them silently. Without exposing the pending value, the app would
//    //    have no way to know what it asked for.
//    bool m_last_position_pending;
//
//    //  True if the frontend requested a resize that has not yet been confirmed.
//    //  Unlike position, GetSize() should not return the pending size. It should
//    //  continue to return the true, confirmed size from the system.
//    //
//    //  Why the difference from position:
//    //  - Rendering backends (GL, Vulkan, D3D) require the true size of the
//    //    drawable surface. Returning a pending size here would desync the
//    //    rendering pipeline and cause invalid buffers or crashes.
//    //  - All major systems (Win32, X11, Wayland, Cocoa) send explicit resize
//    //    notifications, so we can trust the authoritative size will arrive.
//    //
//    //  Summary:
//    //  - Pending position may be returned, since confirmation is unreliable.
//    //  - Pending size is never returned, since confirmation is guaranteed
//    //    and rendering correctness depends on the real value.
//    bool m_last_size_pending;
//
//    flags::type m_flags, m_pending_flags;
//
//    display_id m_pending_display_id;
//    display_id m_last_display_id;
//    display_id m_last_fullscreen_exclusive_display_id;
//
//    display_mode m_requested_fullscreen_mode;
//    display_mode m_current_fullscreen_mode;
//    bool m_fullscreen_exclusive;
//
//    float m_display_scale;
//    float m_opacity;
//
//    bool m_initializing;
//    bool m_destroying;
//    bool m_hiding;
//
//    bool m_tiled;
//
//private:
//
//    //friend _priv::video_internal;
//    friend owner_ptr<window>;
//
//    //friend _priv::window_impl;
//    //std::shared_ptr<_priv::window_impl> m_impl;
//
//private:
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // window events
//    ///////////////////////////////////////////////////////////////////////////////
//
//    bool post_window_shown();
//    void on_window_shown();
//
//    bool post_window_hidden();
//    void on_window_hidden();
//
//    bool post_window_moved(int32_t x, int32_t y);
//    void on_window_moved();
//
//    bool post_window_resized(int32_t w, int32_t h);
//    void on_window_resized();
//
//    bool post_window_minimized();
//    void on_window_minimized();
//
//    bool post_window_maximized();
//    void on_window_maximized();
//
//    bool post_window_restored();
//    void on_window_restored();
//
//    bool post_window_enter_fullscreen();
//    bool post_window_leave_fullscreen();
//
//    //bool post_window_gained_focus();
//    //bool post_window_lost_focus();
//    //bool post_window_mouse_enter();
//    //bool post_window_mouse_leave();
//
//    bool post_window_display_changed(const display& d);
//    void on_window_display_changed(const display& d);
//
//    //bool post_window_display_scale_changed();
//
//    bool post_window_close_requested();
//    bool post_window_destroyed();
};

} // namespace video
} // namespace app
} // namespace vx