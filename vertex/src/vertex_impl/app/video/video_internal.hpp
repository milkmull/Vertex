 #pragma once

#include "vertex/app/instance_pair.hpp"
#include "vertex/app/video/message_box.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex/system/error.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/pen_internal.hpp"
#include "vertex_impl/app/input/touch_internal.hpp"
#include "vertex_impl/app/input/clipboard_internal.hpp"
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

struct static_video_data
{
    //=======================================
    // hints
    //=======================================
    
    bool sync_window_operations;

    //=======================================
    // message box
    //=======================================

    os::atomic<size_t> message_box_count;
};

struct video_data
{
    //=======================================
    // input
    //=======================================

    std::unique_ptr<mouse::mouse_instance> mouse_ptr;
    std::unique_ptr<keyboard::keyboard_instance> keyboard_ptr;
    std::unique_ptr<touch::touch_manager> touch_ptr;
    std::unique_ptr<pen::pen_manager> pen_ptr;
    std::unique_ptr<clipboard::clipboard_instance> clipboard_ptr;

    //=======================================
    // thread
    //=======================================

    typename os::thread::id thread_id;

    //=======================================
    // displays
    //=======================================

    id_generator display_id_generator;
    std::vector<display_instance> displays;
    bool setting_display_mode = false;

    //=======================================
    // windows
    //=======================================

    id_generator window_id_generator;
    std::vector<window_instance> windows;
    window_id grabbed_window = invalid_id;
    os::atomic<window_id> wakeup_window;

    //=======================================
    // system
    //=======================================

    bool is_quitting = false;
    bool suspend_screen_saver = false;
    system_theme theme = system_theme::unknown;
    math::recti desktop_area;
};

//=============================================================================
// video_instance
//=============================================================================

class video_instance
{
public:

    video_instance();
    ~video_instance();

    video_instance(const video_instance&) = delete;
    video_instance& operator=(const video_instance&) = delete;

    video_instance(video_instance&&) noexcept = delete;
    video_instance& operator=(video_instance&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(app_instance* owner);
    void quit();

    //=============================================================================
    // thread
    //=============================================================================

    bool on_video_thread() const;

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

    bool add_display(display_instance& display, bool send_event);
    void remove_display(display_id id, bool send_event);

    std::vector<display_id> list_displays() const;
    size_t get_display_index(display_id id) const;
    bool is_display_connected(display_id id) const;

    display_instance* get_display_instance(display_id id);
    const display_instance* get_display_instance(display_id id) const;

    display_id get_primary_display() const;
    display_id get_display_for_point(const math::vec2i& p) const;
    display_id get_display_for_rect(const math::recti& rect) const;
    display_id get_display_at_origin(const math::vec2i& origin) const;

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

    window_id get_active_window() const;
    window_instance* get_window_instance(window_id id);
    const window_instance* get_window_instance(window_id id) const;

    window_instance* get_grabbed_window();
    void set_grabbed_window(window_id w);
    void validate_grabbed_window();

    bool should_quit_on_window_close() const;

    //=============================================================================
    // fullscreen helpers
    //=============================================================================

    void set_display_fullscreen_window(display_id id, window_id wid, bool send_event);
    display_id find_display_with_fullscreen_window(window_id w);
    void reset_display_modes_for_window(window_id w, display_id target_display);
    void clear_fullscreen_window_from_all_displays(window_id w);

    //=============================================================================
    // input
    //=============================================================================

    void set_all_focus(window_id w);
    bool has_screen_keyboard_support() const;

    //=============================================================================
    // message box
    //=============================================================================

    // https://github.com/libsdl-org/SDL/blob/b2585ac2369c563ce91962b6ac2382bb40e36340/include/SDL3/SDL_messagebox.h#L131

    static size_t message_box_count();

    // these functions can be called before video system is initialized so they are static
    static bool show_message_box(video_instance* this_, const message_box::config& config, message_box::button_id* button);
    static bool show_simple_message_box(video_instance* this_, message_box::type type, const std::string& title, const std::string& messag, window_id w);

    //=============================================================================
    // events
    //=============================================================================

    void pump_events();
    bool wait_event_timeout(window_id w, time::time_point t);
    void send_wakeup_event(); 

    void will_enter_background();
    void did_enter_foreground();

    // System events
    bool send_system_theme_changed(system_theme theme);
    bool send_display_added(display_id id);
    void on_display_added();
    bool send_display_removed(display_id id);
    bool send_display_moved(display_id id);
    void on_display_moved();

    bool send_display_orientation_changed(display_id id, display_orientation orientation);
    bool send_display_desktop_mode_changed(display_id id, const display_mode& mode);
    bool send_display_current_mode_changed(display_id id, const display_mode& mode);
    bool send_display_content_scale_changed(display_id id, const math::vec2& content_scale);

    //=============================================================================
    // data
    //=============================================================================

    app_instance* app = nullptr;
    video_data data;
    static static_video_data s_data;
    std::unique_ptr<video_instance_impl> impl_ptr;
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

    display_mode_instance();
    ~display_mode_instance();

    display_mode_instance(display_mode_instance&&) noexcept;
    display_mode_instance& operator=(display_mode_instance&&) noexcept;

public:

    void finalize();

public:

    video_instance* video = nullptr;
    display_mode_data data;
    std::unique_ptr<display_mode_instance_impl> impl_ptr;
};

//=============================================================================
// display_instance
//=============================================================================

struct display_data
{
    display_id id = invalid_id;
    std::string name;

    display_mode current_mode;
    display_mode_instance desktop_mode;

    // prevent calls to init_modes when inside init_modes call
    mutable bool no_init_modes = false;
    mutable std::vector<display_mode_instance> modes;
    bool setting_display_mode = false;

    display_orientation orientation = display_orientation::unknown;
    display_orientation natural_orientation = display_orientation::unknown;
    math::vec2 content_scale;

    // This is true if we are fullscreen or fullscreen is pending
    bool fullscreen_active = false;
    window_id fullscreen_window_id = invalid_id;
};

class display_instance
{
public:

    display_instance();
    ~display_instance();

    display_instance(display_instance&&) noexcept;
    display_instance& operator=(display_instance&&) noexcept;

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
    std::unique_ptr<display_instance_impl> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx