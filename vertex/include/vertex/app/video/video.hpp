#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/app/id.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

class window;

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

struct display_mode_data
{
    math::vec2i resolution;             // resolution
    int bpp;                            // bits per pixel
    pixel::pixel_format pixel_format;   // pixel format
    float pixel_density;                // scale converting size to pixels (e.g. a 1920x1080 mode with 2.0 scale would have 3840x2160 pixels)
    float refresh_rate;                 // refresh rate
};

VX_API bool compare_display_modes(const display_mode_data& mode1, const display_mode_data& mode2);

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

VX_API void update_displays();

VX_API std::vector<display_id> list_display_ids();
VX_API bool is_display_connected(display_id id);

VX_API display_id get_primary_display();

VX_API display_id get_display_for_point(const math::vec2i& p);
VX_API display_id get_display_for_rect(const math::recti& rect);
VX_API display_id get_display_for_window(const window& w);

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
private:

    display(display_id id) : m_id(id) {}
    ~display() {}

public:

    display_id id() const noexcept { return m_id; }
    bool is_connected() const { return display_connected(m_id); }

    VX_API const char* get_name() const;
    VX_API display_orientation get_orientation() const;
    VX_API math::vec2 get_content_scale() const;

    VX_API math::recti get_bounds() const;
    VX_API math::recti get_work_area() const;

    VX_API bool get_desktop_mode(display_mode_data& mode) const;
    VX_API bool get_current_mode(display_mode_data& mode) const;

    VX_API bool set_current_mode(const display_mode_data& mode);
    VX_API void reset_mode();

    VX_API std::vector<display_mode_data> list_modes() const;
    VX_API bool has_mode(const display_mode_data& mode) const;
    VX_API bool find_closest_mode(const display_mode_data& mode, display_mode_data& closest) const;

private:

    display_id m_id;
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

    window_id display_id = INVALID_ID;
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

VX_API window* const create_window(const window_config& config);
VX_API void destroy_window(window& w);
VX_API window* const get_window(window_id id);

VX_API size_t window_count();
VX_API window* const enum_windows(size_t i);

} // namespace video
} // namespace app
} // namespace vx