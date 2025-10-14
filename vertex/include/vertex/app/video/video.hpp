#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/app/id.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

using display_id = id_type;
using window_id = id_type;

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

struct display_mode
{
    math::vec2i resolution;                                                     // resolution
    int bpp = 0;                                                                // bits per pixel
    pixel::pixel_format pixel_format = pixel::pixel_format::UNKNOWN;            // pixel format
    float pixel_density = 1.0f;                                                 // scale converting size to pixels (e.g. a 1920x1080 mode with 2.0 scale would have 3840x2160 pixels)
    float refresh_rate = 0.0f;                                                  // refresh rate
};

VX_API bool compare_display_modes(const display_mode& mode1, const display_mode& mode2);

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

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

    // implicit on purpose: allows seamless conversion
    display(display_id id) : m_id(id) {}
    ~display() {}

public:

    // implicit conversion back to id
    operator display_id() const noexcept { return m_id; }
    display_id id() const noexcept { return m_id; }

    bool is_valid() const noexcept { return is_valid_id(m_id); }
    bool is_connected() const;

    VX_API std::string get_name() const;
    VX_API display_orientation get_orientation() const;
    VX_API display_orientation get_natural_orientation() const;
    VX_API math::vec2 get_content_scale() const;

    VX_API math::recti get_bounds() const;
    VX_API math::recti get_work_area() const;

    VX_API bool get_desktop_mode(display_mode& mode) const;
    VX_API bool get_current_mode(display_mode& mode) const;

    VX_API bool set_current_mode(const display_mode& mode);
    VX_API void reset_mode();

    VX_API std::vector<display_mode> list_modes() const;
    VX_API bool has_mode(const display_mode& mode) const;
    VX_API bool find_closest_mode(const display_mode& mode, display_mode& closest) const;

private:

    display_id m_id;
};

VX_API std::vector<display_id> list_displays();
inline bool is_display_connected(display_id id) { return display(id).is_connected(); }
VX_API display_id get_primary_display();

VX_API display_id get_display_for_point(const math::vec2i& p);
VX_API display_id get_display_for_rect(const math::recti& rect);
VX_API display_id get_display_for_window(window_id w);

VX_API math::recti get_desktop_area();

///////////////////////////////////////////////////////////////////////////////
// screen saver
///////////////////////////////////////////////////////////////////////////////

VX_API bool enable_screen_saver();
VX_API bool screen_saver_enabled();
VX_API bool disable_screen_saver();

///////////////////////////////////////////////////////////////////////////////
// windows
///////////////////////////////////////////////////////////////////////////////

struct window_config
{
    std::string title;
    math::vec2i size;
    math::vec2i position;

    float opacity = 1.0f;

    display_id display_id = INVALID_ID;
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

VX_API window_id create_window(const window_config& config);
VX_API void destroy_window(window_id id);

VX_API bool window_exists(window_id id);
VX_API std::vector<window_id> list_windows();

} // namespace video
} // namespace app
} // namespace vx