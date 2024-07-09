#pragma once

#include "../_priv/device_id.h"
#include "vertex/pixel/pixel.h"
#include "vertex/math/math/type/vec2.h"
#include "vertex/math/geometry/type/recti.h"

namespace vx {
namespace app {
namespace video {

class window;

enum class process_dpi_awareness
{
    UNAWARE,
    SYSTEM,
    PER_MONITOR
};

enum class display_orientation
{
    UNKNOWN,

    PORTRAIT,
    LANDSCAPE,

    PORTRAIT_FLIPPED,
    LANDSCAPE_FLIPPED
};

//===========================================================================

class display;

struct display_mode
{
    friend display;

public:

    display_mode();
    display_mode(const display_mode&);
    display_mode(display_mode&&) noexcept;
    ~display_mode();

    display_mode& operator=(const display_mode&);
    display_mode& operator=(display_mode&&) noexcept;

public:

    math::vec2i resolution;
    int bpp;
    pixel::pixel_format pixel_format;
    float pixel_density;
    float refresh_rate;

public:

    bool operator==(const display_mode& dm) const;
    bool operator!=(const display_mode& dm) const;

private:

    class display_mode_impl;
    std::unique_ptr<display_mode_impl> m_impl;

};

//===========================================================================

class display
{
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

    const display_mode& get_mode() const;
    bool set_mode(display_mode& mode) const;

    const std::vector<display_mode>& list_modes() const;
    bool has_mode(const display_mode& mode) const;

    display_orientation get_orientation() const;
    const math::vec2& get_content_scale() const;

    math::recti get_bounds() const;
    math::recti get_work_area() const;

private:

    class display_impl;
    std::unique_ptr<display_impl> m_impl;

private:

    // necessary to call display_impl::update_displays
    friend void update_displays();
};

//===========================================================================

bool init();
void quit();

process_dpi_awareness get_dpi_awareness();

//===========================================================================

void update_displays();

const display* get_display(device_id id);
display* get_primary_display();
std::vector<const display*> list_displays();

const display* get_display_for_point(const math::vec2i& p);
const display* get_display_for_rect(const math::recti& rect);
const display* get_display_for_window_position(const window* window);
const display* get_display_for_window(const window* window);

math::recti get_desktop_area();

}
}
}