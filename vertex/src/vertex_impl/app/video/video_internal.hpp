 #pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {

class app_instance;

namespace video {

///////////////////////////////////////////////////////////////////////////////
// video hint cache
///////////////////////////////////////////////////////////////////////////////

struct video_hints
{
    bool sync_window_operations = false;
};

///////////////////////////////////////////////////////////////////////////////
// video data
///////////////////////////////////////////////////////////////////////////////

struct video_instance_impl;

struct video_data
{
    owner_ptr<mouse::mouse_instance> mouse_ptr;
    owner_ptr<keyboard::keyboard_instance> keyboard_ptr;

    id_generator display_id_generator;
    std::vector<display_instance> displays;
    bool setting_display_mode = false;

    std::vector<owner_ptr<window>> windows;
    os::atomic<const window*> wakeup_window = nullptr;

    bool suspend_screen_saver = false;
    video_hints hints;
};

///////////////////////////////////////////////////////////////////////////////
// forward declares
///////////////////////////////////////////////////////////////////////////////

class display_mode_instance;
class display_instance;

///////////////////////////////////////////////////////////////////////////////
// video_instance
///////////////////////////////////////////////////////////////////////////////

class video_instance
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // video
    ///////////////////////////////////////////////////////////////////////////////

    bool init(app_instance* owner);
    void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // dpi
    ///////////////////////////////////////////////////////////////////////////////

    process_dpi_awareness get_dpi_awareness() const;

    ///////////////////////////////////////////////////////////////////////////////
    // system theme
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/libsdl-org/SDL/blob/a34d31322ce7423b54db29ca6da5883c189b82ee/src/video/SDL_video.c#L773C6-L773C24

    system_theme get_system_theme() const;
    void set_system_theme(system_theme theme);

    ///////////////////////////////////////////////////////////////////////////////
    // display mode
    ///////////////////////////////////////////////////////////////////////////////

    void clear_display_mode_display_id(display_mode_instance& dm);

    ///////////////////////////////////////////////////////////////////////////////
    // displays
    ///////////////////////////////////////////////////////////////////////////////

    static display create_display_reference(display_id id) { return display(id); }
#   define invalid_display display(INVALID_ID)

    void update_displays();

    std::vector<display_id> list_displays() const;
    size_t get_display_index(display_id id) const;
    bool is_display_connected(display_id id) const;

    display_instance* get_display_instance(display_id id);
    const display_instance* get_display_instance(display_id id) const;

    display_id get_primary_display() const;

    display_id get_display_for_point(const math::vec2i& p) const;
    display_id get_display_for_rect(const math::recti& rect) const;
    display_id get_display_at_origin(const math::vec2i& o) const;
    display_id get_display_for_window(const window& w) const;

    math::recti get_desktop_area() const;

    ///////////////////////////////////////

    std::string get_display_name(display_id id) const;
    display_orientation get_display_orientation(display_id id) const;
    math::vec2 get_display_content_scale(display_id id) const;

    math::recti get_display_bounds(display_id id) const;
    math::recti get_display_work_area(display_id id) const;

    bool get_display_desktop_mode(display_id id, display_mode& mode) const;
    bool get_display_current_mode(display_id id, display_mode& mode) const;

    bool set_display_current_mode(display_id id, const display_mode& mode);
    void reset_display_mode(display_id id);

    std::vector<display_mode> list_display_modes_for_display(display_id id) const;
    bool display_has_mode(display_id id, const display_mode& mode) const;
    bool find_closest_display_mode_for_display(display_id id, const display_mode& mode, display_mode& closest) const;

    ///////////////////////////////////////////////////////////////////////////////
    // screen saver
    ///////////////////////////////////////////////////////////////////////////////

    bool enable_screen_saver();
    bool screen_saver_enabled() const;
    bool disable_screen_saver();

    ///////////////////////////////////////////////////////////////////////////////
    // windows
    ///////////////////////////////////////////////////////////////////////////////

    window* create_window(const window_config& config);
    void destroy_window(window& w);
    void destroy_windows();

    std::vector<window_id> get_window_ids();
    window* get_window(window_id id);
    window_id get_window_id(const window& w);

    size_t window_count();
    window* enum_windows(size_t i);

    void check_window_display_changed(window& w);

    window* get_active_window();
    void set_wakeup_window(window* w);

    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen helpers
    ///////////////////////////////////////////////////////////////////////////////

    window_id get_display_fullscreen_window_id(const display& d);
    void set_display_fullscreen_window_id(display& d, window_id id);
    void clear_display_fullscreen_window_id(display& d);
    display* find_display_with_fullscreen_window(const window& w);

    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsevents.c#L2432

    void pump_events();
    bool wait_event_timeout(const window* w, time::time_point t);
    void send_wakeup_event();

    bool post_system_theme_changed(system_theme theme);

    bool post_display_added(const display_instance& d);
    void on_display_added();

    bool post_display_removed(const display_instance& d);
    bool post_display_moved(const display_instance& d);
    bool post_display_orientation_changed(display_instance& d, display_orientation orientation);
    bool post_display_desktop_mode_changed(display_instance& d, const display_mode& mode);
    bool post_display_current_mode_changed(display_instance& d, const display_mode& mode);
    bool post_display_content_scale_changed(display_instance& d, const math::vec2& content_scale);

    ///////////////////////////////////////////////////////////////////////////////
    // impl helpers
    ///////////////////////////////////////////////////////////////////////////////

    template <typename T>
    T* create() { return new T; }

    template <typename T>
    T construct() { return T{}; }

    template <typename T>
    auto create_impl() { return std::make_unique<T>(); }

    template <typename T>
    auto* get_impl(T& x) { return x.m_impl.get(); }

    template <typename T>
    const auto* get_impl(const T& x) { return x.m_impl.get(); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    app_instance* app = nullptr;
    video_data data;
    owner_ptr<video_instance_impl> impl_ptr;

};

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

struct display_mode_data
{
    display_mode mode;
    display_id display_id = INVALID_ID;
};

struct display_mode_instance_impl;

class display_mode_instance
{
public:

    display_mode_data data;
    owner_ptr<display_mode_instance_impl> impl_ptr;
};

///////////////////////////////////////////////////////////////////////////////
// display
///////////////////////////////////////////////////////////////////////////////

struct display_data
{
    display_id id;
    std::string name;

    display_mode current_mode;
    display_mode_instance desktop_mode;
    mutable std::vector<display_mode_instance> modes;
    bool setting_display_mode = false;

    display_orientation orientation;
    math::vec2 content_scale;

    window_id fullscreen_window_id;
};

struct display_instance_impl;

class display_instance
{
public:

    display as_display() const { return video_instance::create_display_reference(data.id); }

    const std::string& get_name() const;
    display_orientation get_orientation() const;
    math::vec2 get_content_scale() const;

    math::recti get_bounds() const;
    math::recti get_work_area() const;

    bool get_desktop_mode(display_mode& mode) const;
    bool get_current_mode(display_mode& mode) const;

    bool set_current_mode(const display_mode& mode);
    void reset_mode();

    std::vector<display_mode> list_modes() const;
    bool has_mode(const display_mode& mode) const;
    const display_mode_instance* find_mode(const display_mode& mode) const;
    bool find_closest_mode(const display_mode& mode, display_mode& closest) const;

public:

    void init_modes() const;

public:

    video_instance* video = nullptr;
    display_data data;
    owner_ptr<display_instance_impl> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx