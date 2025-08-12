#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/app/device_id.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

VX_API bool init();
VX_API bool is_init();
VX_API void quit();

class display_mode;
class display;
class window;

namespace _priv { class video_internal; }
namespace _priv { class display_mode_impl; }
namespace _priv { class display_impl; }
namespace _priv { class window_impl; }

enum class process_dpi_awareness
{
    UNAWARE,
    SYSTEM,
    PER_MONITOR
};

VX_API process_dpi_awareness get_dpi_awareness();

enum class system_theme
{
    UNKNOWN = 0,
    LIGHT,
    DARK
};

VX_API system_theme get_system_theme();

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

class display_mode
{
public:

    VX_API display_mode();
    VX_API display_mode(const display_mode&);
    VX_API display_mode(display_mode&&) noexcept;
    VX_API ~display_mode();

    VX_API display_mode& operator=(const display_mode&);
    VX_API display_mode& operator=(display_mode&&) noexcept;

    VX_API static bool compare(const display_mode& mode1, const display_mode& mode2);

public:

    math::vec2i resolution;             // resolution
    int bpp;                            // bits per pixel
    pixel::pixel_format pixel_format;   // pixel format
    float pixel_density;                // scale converting size to pixels (e.g. a 1920x1080 mode with 2.0 scale would have 3840x2160 pixels)
    float refresh_rate;                 // refresh rate

private:

    friend _priv::video_internal;

    friend display;
    friend _priv::display_impl;
    mutable device_id m_display_id;     // the display that this mode is associated with

    friend _priv::display_mode_impl;
    std::shared_ptr<_priv::display_mode_impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

VX_API void update_displays();

VX_API display* get_display(device_id id);
VX_API display* get_primary_display();

VX_API size_t display_count();
VX_API display* enum_displays(size_t i);

VX_API display* get_display_for_point(const math::vec2i& p);
VX_API display* get_display_for_rect(const math::recti& rect);
VX_API display* get_display_for_window(const window* w);

VX_API math::recti get_desktop_area();

enum class display_orientation
{
    UNKNOWN = 0,

    PORTRAIT,
    PORTRAIT_FLIPPED,

    LANDSCAPE,
    LANDSCAPE_FLIPPED
};

class display
{
public:

    VX_API display();
    VX_API ~display();

    VX_API display(display&&) noexcept;
    VX_API display& operator=(display&&) noexcept;

    display(const display&) = delete;
    display& operator=(const display&) = delete;

public:

    VX_API device_id id() const;
    VX_API const std::string& name() const;

    VX_API display_orientation get_orientation() const;
    VX_API const math::vec2& get_content_scale() const;

    VX_API math::recti get_bounds() const;
    VX_API math::recti get_work_area() const;

private:

    void update_modes() const;

public:

    VX_API const display_mode& get_desktop_mode() const;
    VX_API const display_mode& get_current_mode() const;

    VX_API bool set_current_mode(const display_mode& mode);
    VX_API void clear_mode();

    VX_API const std::vector<display_mode>& list_modes() const;
    VX_API const display_mode* find_mode(const display_mode& mode) const;
    VX_API const display_mode* find_closest_mode(int width, int height, float refresh_rate) const;

private:

    device_id m_id;
    std::string m_name;

    display_mode m_desktop_mode;
    display_mode m_current_mode;
    mutable std::vector<display_mode> m_modes;

    display_orientation m_orientation;
    math::vec2 m_content_scale;

private:

    friend _priv::video_internal;

    friend _priv::display_impl;
    std::unique_ptr<_priv::display_impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
// windows
///////////////////////////////////////////////////////////////////////////////

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

VX_API window* create_window(const window_config& config);
VX_API void destroy_window(window* w);
VX_API window* get_window(device_id id);

VX_API size_t window_count();
VX_API window* enum_windows(size_t i);

} // namespace video
} // namespace app
} // namespace vx