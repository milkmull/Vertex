 #pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex/system/error.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/video/window_internal.hpp"

namespace vx {
namespace app {

class app_instance;

namespace video {

//=============================================================================
// forward declares
//=============================================================================

class display_mode_instance;
class display_instance;
class video_instance_impl;
class display_mode_instance_impl;
class display_instance_impl;

//=============================================================================
// video data
//=============================================================================

struct video_hints
{
    bool sync_window_operations = false;
};

struct video_data
{
    // Input
    owner_ptr<mouse::mouse_instance> mouse_ptr;
    owner_ptr<keyboard::keyboard_instance> keyboard_ptr;

    // Displays
    id_generator display_id_generator;
    std::vector<display_instance> displays;
    bool setting_display_mode = false;

    // Windows
    id_generator window_id_generator;
    std::vector<window_instance> windows;
    window_id grabbed_window = INVALID_ID;
    os::atomic<window_id> wakeup_window = INVALID_ID;

    // System settings
    bool suspend_screen_saver = false;
    video_hints hints;
    system_theme theme = system_theme::UNKNOWN;
    math::recti desktop_area;
};

//=============================================================================
// deleters
//=============================================================================

struct video_instance_impl_deleter { void operator()(video_instance_impl* ptr) const noexcept; };
struct display_mode_instance_impl_deleter { void operator()(display_mode_instance_impl* ptr) const noexcept; };
struct display_instance_impl_deleter { void operator()(display_instance_impl* ptr) const noexcept; };

//=============================================================================
// video_instance
//=============================================================================

class video_instance
{
public:

    //=============================================================================
    // lifecycle
    //=============================================================================

    bool init(app_instance* owner);
    void quit();

    ~video_instance() { quit(); }

    //=============================================================================
    // dpi
    //=============================================================================

    process_dpi_awareness get_dpi_awareness() const;

    //=============================================================================
    // system theme
    //=============================================================================

    system_theme get_system_theme() const;
    void set_system_theme(system_theme theme);

    //=============================================================================
    // displays
    //=============================================================================

    bool add_display(display_instance& display, bool post_event);
    void remove_display(display_id id, bool post_event);
    void clear_displays(bool post_events);

    std::vector<display_id> list_displays() const;
    size_t get_display_index(display_id id) const;
    bool is_display_connected(display_id id) const;

    display_instance* get_display_instance(display_id id);
    const display_instance* get_display_instance(display_id id) const;

    display_id get_primary_display() const;
    display_id get_display_for_point(const math::vec2i& p) const;
    display_id get_display_for_rect(const math::recti& rect) const;
    display_id get_display_at_origin(const math::vec2i& origin) const;
    display_id get_display_for_window(window_id w, bool ignore_pending_display_id) const;

    void update_desktop_area();
    math::recti get_desktop_area() const;

    // Display properties
    std::string get_display_name(display_id id) const;
    display_orientation get_display_orientation(display_id id) const;
    display_orientation get_display_natural_orientation(display_id id) const;
    math::vec2 get_display_content_scale(display_id id) const;

    math::recti get_display_bounds(display_id id) const;
    math::recti get_display_work_area(display_id id) const;

    // Display modes
    const display_mode_instance* get_display_desktop_mode(display_id id) const;
    const display_mode* get_display_current_mode(display_id id) const;
    bool set_display_mode(display_id id, const display_mode& mode);
    void reset_display_mode(display_id id, bool clear_fullscreen_window);
    std::vector<display_mode> list_display_modes_for_display(display_id id) const;
    bool display_has_mode(display_id id, const display_mode& mode) const;

    // Mode search
    const display_mode_instance* find_display_mode_for_display(display_id id, const display_mode& mode) const;
    const display_mode_instance* find_closest_display_mode_for_display(display_id id, const display_mode& mode, bool include_high_density_modes, bool match_resolution) const;
    const display_mode_instance* find_display_mode_candidate_for_display(display_id id, const display_mode& mode, bool include_high_density_modes, bool match_resolution) const;

    //=============================================================================
    // screen saver
    //=============================================================================

    bool enable_screen_saver();
    bool screen_saver_enabled() const;
    bool disable_screen_saver();

    //=============================================================================
    // windows
    //=============================================================================

    window_id create_window(const window_config& config);
    void destroy_window(window_id id);
    void destroy_windows();

    std::vector<window_id> list_windows() const;
    size_t get_window_index(window_id id) const;
    bool window_exists(window_id id) const;

    window_instance* get_window_instance(window_id id);
    const window_instance* get_window_instance(window_id id) const;

    window_id get_active_window();

    window_instance* get_grabbed_window();
    void set_grabbed_window(window_id w);
    void validate_grabbed_window();

    bool should_quit_on_window_close() const;

    //=============================================================================
    // fullscreen helpers
    //=============================================================================

    void set_display_fullscreen_window(display_id id, window_id wid, bool post_event);
    display_id find_display_with_fullscreen_window(window_id id);
    void reset_display_modes_for_window(window_id w, display_id target_display);
    void clear_fullscreen_window_from_all_displays(window_id id);

    //=============================================================================
    // events
    //=============================================================================

    void pump_events();
    bool wait_event_timeout(window_id w, time::time_point t);
    void send_wakeup_event(); 

    // System events
    bool post_system_theme_changed(system_theme theme);
    bool post_display_added(display_id id);
    void on_display_added();
    bool post_display_removed(display_id id);
    bool post_display_moved(display_id id);
    void on_display_moved();

    bool post_display_orientation_changed(display_id id, display_orientation orientation);
    bool post_display_desktop_mode_changed(display_id id, const display_mode& mode);
    bool post_display_current_mode_changed(display_id id, const display_mode& mode);
    bool post_display_content_scale_changed(display_id id, const math::vec2& content_scale);

    //=============================================================================
    // data
    //=============================================================================

    app_instance* app = nullptr;
    video_data data;
    owner_ptr<video_instance_impl, video_instance_impl_deleter> impl_ptr;
};

//=============================================================================
// display_mode_instance
//=============================================================================

struct display_mode_data
{
    display_mode mode;
};

class display_mode_instance
{
public:

    void finalize();

public:

    display_mode_data data;
    owner_ptr<display_mode_instance_impl, display_mode_instance_impl_deleter> impl_ptr;
};

//=============================================================================
// display_instance
//=============================================================================

struct display_data
{
    display_id id;
    std::string name;

    display_mode current_mode;
    display_mode_instance desktop_mode;

    // prevent calls to init_modes when inside init_modes call
    mutable bool no_init_modes = false;
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

    //=============================================================================
    // lifecycle
    //=============================================================================

    void finalize();

    //=============================================================================
    // accessors
    //=============================================================================

    display as_display() const { return display(data.id); }
    const std::string& get_name() const;

    //=============================================================================
    // orientation
    //=============================================================================

    void set_orientation(display_orientation orientation);
    display_orientation get_orientation() const;
    display_orientation get_natural_orientation() const;

    //=============================================================================
    // sclale
    //=============================================================================

    math::vec2 get_content_scale() const;
    void set_content_scale(const math::vec2& scale);

    //=============================================================================
    // geometry
    //=============================================================================

    math::recti get_bounds() const;
    math::recti get_work_area() const;

    //=============================================================================
    // display modes
    //=============================================================================

    void init_modes() const;
    void reset_modes();

    const display_mode_instance& get_desktop_mode() const;
    void set_desktop_mode(display_mode_instance& mode);

    const display_mode& get_current_mode() const;
    void set_current_mode(const display_mode& mode);

    bool set_display_mode(const display_mode& mode);
    void reset_display_mode(bool clear_fullscreen_window);

    bool add_mode(display_mode_instance& mode) const;
    std::vector<display_mode> list_modes() const;
    bool has_mode(const display_mode& mode) const;
    const display_mode_instance* find_mode(const display_mode& mode) const;
    const display_mode_instance* find_closest_mode(const display_mode& mode, bool include_high_density_modes, bool match_resolution) const;

public:

    video_instance* video = nullptr;
    display_data data;
    owner_ptr<display_instance_impl, display_instance_impl_deleter> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx