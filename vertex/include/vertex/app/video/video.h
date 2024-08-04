#pragma once

#include "vertex/pixel/pixel.h"
#include "vertex/math/math/type/vec2.h"
#include "vertex/math/geometry/type/recti.h"
#include "vertex/app/_priv/device_id.h"

namespace vx {
namespace app {

class video
{
private:

    class video_impl;

public:

    struct display_mode;
    class display;
    class window;

public:

    enum class process_dpi_awareness
    {
        UNAWARE,
        SYSTEM,
        PER_MONITOR
    };

    enum class display_orientation : int32_t
    {
        UNKNOWN,

        PORTRAIT,
        LANDSCAPE,

        PORTRAIT_FLIPPED,
        LANDSCAPE_FLIPPED
    };

    enum class system_theme
    {
        UNKNOWN = 0,
        LIGHT,
        DARK
    };

    struct caps
    {
        using type = uint32_t;

        enum : type
        {
            NONE                          = 0,
            MODE_SWITCHING_EMULATED       = (1 << 0),
            SENDS_FULLSCREEN_DIMENSIONS   = (1 << 1),
            IS_FULLSCREEN_ONLY            = (1 << 2),
            SEND_DISPLAY_CHANGES          = (1 << 3),
            DISABLE_FULLSCREEN_MOUSE_WARP = (1 << 4),
            ASYNCHRONOUS_WINDOWING        = (1 << 5),
            STATIC_WINDOW                 = (1 << 6)
        };
    };

public:

    //===========================================================================

    struct display_mode
    {
    private:

        friend video;
        friend video_impl;
        friend display;
        friend window;

    public:

        display_mode();
        display_mode(const display_mode&);
        display_mode(display_mode&&) noexcept;
        ~display_mode();

        display_mode& operator=(const display_mode&);
        display_mode& operator=(display_mode&&) noexcept;

    public:

        static bool compare(const display_mode& mode1, const display_mode& mode2);

    public:

        math::vec2i resolution;
        int bpp;
        pixel::pixel_format pixel_format;
        float pixel_density;
        float refresh_rate;

    private:

        device_id m_display_id;

    private:

        class display_mode_impl;
        std::unique_ptr<display_mode_impl> m_impl;
    };

    //===========================================================================

public:

    class display
    {
    private:

        friend video;
        friend video_impl;
        friend window;

    public:

        display();
        display(const display&);
        display(display&&) noexcept;

        ~display();

        display& operator=(const display&);
        display& operator=(display&&) noexcept;

    public:

        device_id id() const;
        const std::string& name() const;

        const display_mode& get_desktop_mode() const;

        const display_mode& get_current_mode() const;
        bool set_current_mode(display_mode& mode);
        void clear_mode();

        const std::vector<display_mode>& list_modes() const;
        const display_mode* find_mode(const display_mode& mode) const;
        const display_mode* find_closest_mode(int width, int height, float refresh_rate) const;

    private:

        display_orientation get_orientation() const;
        const math::vec2& get_content_scale() const;

        math::recti get_bounds() const;
        math::recti get_work_area() const;

    private:

        device_id m_id;
        std::string m_name;

        display_mode m_desktop_mode;
        display_mode m_current_mode;
        std::vector<display_mode> m_modes;

        display_orientation m_orientation;
        math::vec2 m_content_scale;

        device_id m_fullscreen_window_id;

    private:

        class display_impl;
        std::unique_ptr<display_impl> m_impl;
    };

    //===========================================================================

    struct window_config
    {
        std::string title;
        math::vec2i size;
        math::vec2i position;

        float opacity = 1.0f;

        bool center_on_display = false;

        bool minimized = false;
        bool maximized = false;
        bool fullscreen = false;

        bool borderless = false;
        bool resizable = false;
        bool topmost = false;

        bool hidden = false;
        bool focussed = true;

        bool mouse_grabbed = false;
        bool mouse_capture = false;
    };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // video
    ///////////////////////////////////////////////////////////////////////////////

    video() = delete;
    ~video() = delete;

    video(const video&) = delete;
    video(video&&) noexcept = delete;

    video& operator=(const video&) = delete;
    video& operator=(video&&) noexcept = delete;

    static bool init();
    static void quit();

    static process_dpi_awareness get_dpi_awareness();
    static system_theme get_system_theme();

    ///////////////////////////////////////////////////////////////////////////////
    // displays
    ///////////////////////////////////////////////////////////////////////////////

    static void update_displays();

    static display* get_display(device_id id);
    static display* get_primary_display();
    static std::vector<display*> list_displays();

    static display* get_display_for_point(const math::vec2i& p);
    static display* get_display_for_rect(const math::recti& rect);

    ///////////////////////////////////////////////////////////////////////////////
    // window display
    ///////////////////////////////////////////////////////////////////////////////

public:

    static display* get_display_for_window(const window& w);

private:

    static void check_window_display_changed(window& w);

public:

    //===========================================================================

    static math::recti get_desktop_area();

    //===========================================================================

    static window* create_window(const window_config& config);
    static void destroy_window(window& w);
    static window* get_window(device_id id);

private:

    ///////////////////////////////////////////////////////////////////////////////
    // display events
    ///////////////////////////////////////////////////////////////////////////////

    static bool post_display_added(const display* d);
    static void on_display_added();

    static bool post_display_removed(const display* d);
    static bool post_display_moved(const display* d);
    static bool post_display_orientation_changed(display* d, display_orientation orientation);
    static bool post_display_desktop_mode_changed(display* d, const display_mode& mode);
    static bool post_display_current_mode_changed(display* d, const display_mode& mode);
    static bool post_display_content_scale_changed(display* d, const math::vec2& content_scale);

private:

    struct video_data
    {
        static const char* driver_name;
        static const caps::type video_caps;

        bool is_init = false;

        std::vector<std::unique_ptr<display>> displays;
        std::vector<std::unique_ptr<window>> windows;

        bool setting_display_mode = false;
    };

    static video_data s_video_data;

};

}
}