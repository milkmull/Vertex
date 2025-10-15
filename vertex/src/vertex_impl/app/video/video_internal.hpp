 #pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex_impl/app/video/window_internal.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {

class app_instance;

namespace video {

///////////////////////////////////////////////////////////////////////////////
// forward declares
///////////////////////////////////////////////////////////////////////////////

class display_mode_instance;
class display_instance;

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

struct video_data
{
    owner_ptr<mouse::mouse_instance> mouse_ptr;
    owner_ptr<keyboard::keyboard_instance> keyboard_ptr;

    id_generator display_id_generator;
    std::vector<display_instance> displays;
    bool setting_display_mode = false;

    id_generator window_id_generator;
    std::vector<window_instance> windows;
    window_id grabbed_window = INVALID_ID;
    os::atomic<window_id> wakeup_window = INVALID_ID;

    bool suspend_screen_saver = false;
    video_hints hints;
};

///////////////////////////////////////////////////////////////////////////////
// video_instance_impl
///////////////////////////////////////////////////////////////////////////////

class video_instance_impl;

struct video_instance_impl_deleter
{
    void operator()(video_instance_impl* ptr) const noexcept;
};

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

    std::vector<display_id> list_displays() const;
    size_t get_display_index(display_id id) const;
    bool is_display_connected(display_id id) const;

    display_instance* get_display_instance(display_id id);
    const display_instance* get_display_instance(display_id id) const;

    display_id get_primary_display() const;

    display_id get_display_for_point(const math::vec2i& p) const;
    display_id get_display_for_rect(const math::recti& rect) const;
    display_id get_display_at_origin(const math::vec2i& origin) const;
    display_id get_display_for_window(window_id id, bool ignore_pending) const;

    math::recti get_desktop_area() const;

    ///////////////////////////////////////

    std::string get_display_name(display_id id) const;
    display_orientation get_display_orientation(display_id id) const;
    display_orientation get_display_natural_orientation(display_id id) const;
    math::vec2 get_display_content_scale(display_id id) const;

    math::recti get_display_bounds(display_id id) const;
    math::recti get_display_work_area(display_id id) const;

    const display_mode_instance* get_display_desktop_mode(display_id id) const;
    const display_mode* get_display_current_mode(display_id id) const;
    bool set_display_current_mode(display_id id, const display_mode& mode);

    void reset_display_mode(display_id id, bool clear_fullscreen_window);

    std::vector<display_mode> list_display_modes_for_display(display_id id) const;
    bool display_has_mode(display_id id, const display_mode& mode) const;

    const display_mode_instance* find_display_mode(const display_mode& mode) const;
    const display_mode_instance* find_closest_display_mode(const display_mode& mode) const;
    const display_mode_instance* find_display_mode_candidate(const display_mode& mode) const;

    ///////////////////////////////////////////////////////////////////////////////
    // screen saver
    ///////////////////////////////////////////////////////////////////////////////

    bool enable_screen_saver();
    bool screen_saver_enabled() const;
    bool disable_screen_saver();

    ///////////////////////////////////////////////////////////////////////////////
    // windows
    ///////////////////////////////////////////////////////////////////////////////

    window_id create_window(const window_config& config);
    void destroy_window(window_id id);
    void destroy_windows();

    std::vector<window_id> list_windows() const;
    size_t get_window_index(window_id id) const;
    bool window_exists(window_id id) const;

    window_instance* get_window_instance(window_id id);
    const window_instance* get_window_instance(window_id id) const;

    void check_window_display_changed(window_id id);

    window_id get_active_window();
    void set_wakeup_window(window* w);

    window_instance* get_grabbed_window();
    void set_grabbed_window(window_id w);
    void validate_grabbed_window();

    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen helpers
    ///////////////////////////////////////////////////////////////////////////////

    window_id get_display_fullscreen_window_id(display_id id) const;
    void set_display_fullscreen_window_id(display_id id, window_id wid);
    display_id find_display_with_fullscreen_window(window_id id);

    void reset_display_modes_for_window(window_id w, display_id target_display);

    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsevents.c#L2432

    void pump_events();
    bool wait_event_timeout(window_id w, time::time_point t);
    void send_wakeup_event(); 

    bool post_system_theme_changed(system_theme theme);

    bool post_display_added(display_id id);
    void on_display_added();

    bool post_display_removed(display_id id);
    bool post_display_moved(display_id id);
    bool post_display_orientation_changed(display_id id, display_orientation orientation);
    bool post_display_desktop_mode_changed(display_id id, const display_mode& mode);
    bool post_display_current_mode_changed(display_id id, const display_mode& mode);
    bool post_display_content_scale_changed(display_id id, const math::vec2& content_scale);

    bool post_window_destroyed(window_id id);

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    app_instance* app = nullptr;
    video_data data;
    owner_ptr<video_instance_impl, video_instance_impl_deleter> impl_ptr;

};

///////////////////////////////////////////////////////////////////////////////
// display mode impl
///////////////////////////////////////////////////////////////////////////////

class display_mode_instance_impl;

struct display_mode_instance_impl_deleter
{
    void operator()(display_mode_instance_impl* ptr) const noexcept;
};

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

struct display_mode_data
{
    display_mode mode;
};

class display_mode_instance
{
public:

    display_mode_data data;
    owner_ptr<display_mode_instance_impl, display_mode_instance_impl_deleter> impl_ptr;
};

///////////////////////////////////////////////////////////////////////////////
// display impl
///////////////////////////////////////////////////////////////////////////////

class display_instance_impl;

struct display_instance_impl_deleter
{
    void operator()(display_instance_impl* ptr) const noexcept;
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
    display_orientation natural_orientation;
    math::vec2 content_scale;

    // This is true if we are fullscreen or fullscreen is pending
    bool fullscreen_active = false;
    window_id fullscreen_window_id = INVALID_ID;
};

class display_instance
{
public:

    display as_display() const { return video_instance::create_display_reference(data.id); }

    const std::string& get_name() const;

    void set_orientation(display_orientation orientation);
    display_orientation get_orientation() const;
    display_orientation get_natural_orientation() const;

    math::vec2 get_content_scale() const;
    void set_content_scale(const math::vec2& scale);

    math::recti get_bounds() const;
    math::recti get_work_area() const;

    const display_mode_instance& get_desktop_mode() const;
    void set_desktop_mode(display_mode_instance& mode);

    const display_mode& get_current_mode() const;
    bool set_current_mode(const display_mode& mode);

    void reset_mode(bool clear_fullscreen_window);

    std::vector<display_mode> list_modes() const;
    bool has_mode(const display_mode& mode) const;
    const display_mode_instance* find_mode(const display_mode& mode) const;
    const display_mode_instance* find_closest_mode(const display_mode& mode) const;

public:

    void init_modes() const;

public:

    video_instance* video = nullptr;
    display_data data;
    owner_ptr<display_instance_impl, display_instance_impl_deleter> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx