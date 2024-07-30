#include "vertex/config.h"
#include "vertex/app/video/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/video/win32_window.h"

#endif

namespace vx {
namespace app {

#define IS_FULLSCREEN_VISIBLE(w) (((w)->is_fullscreen()) && ((w)->is_visible()) && !((w)->is_minimized()))

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

video::window::window(const window_config& config)
    : m_id(get_next_device_id())
    , m_min_aspect(0.0f), m_max_aspect(0.0f)
    , m_flags(flags::NONE), m_pending_flags(flags::NONE)
    , m_last_display_id(0)
    , m_requested_fullscreen_mode_set(false)
    , m_opacity(0.0f)
    , m_initializing(true)
    , m_destroying(false)
    , m_moving_or_resizing(false)
    , m_hiding(false)
    , m_tiled(false)
    , m_sync_requested(false)
{
    m_position = config.position;
    m_size = config.size;

    // Some backends don't support 0 sized windows

    if (m_size.x <= 0)
    {
        m_size.x = 1;
    }
    if (m_size.y <= 0)
    {
        m_size.y = 1;
    }

    // Find the display for the window

    const video::display* display = nullptr;
    video::update_displays();

    display = video::get_display_for_rect(math::recti(m_position, m_size));
    if (!display)
    {
        display = video::get_primary_display();
    }
    if (!display)
    {
        return;
    }

    // If auto position is true, we center the window on the display

    if (config.center_on_display)
    {
        math::recti bounds = display->get_work_area();
        m_position = bounds.position + (bounds.size - m_size) / 2;
    }

    m_floating_rect.position = m_windowed_rect.position = m_position;
    m_floating_rect.size = m_windowed_rect.size = m_size;

    //flags::type creation_flags = flags::HIDDEN;
    flags::type window_flags = flags::NONE;

    if (config.minimized)
    {
        window_flags |= flags::MINIMIZED;
    }
    if (config.maximized)
    {
        window_flags |= flags::MAXIMIZED;
    }
    if (config.hidden)
    {
        window_flags |= flags::HIDDEN;
    }
    if (config.borderless)
    {
        window_flags |= flags::BORDERLESS;
    }
    if (config.resizable)
    {
        window_flags |= flags::RESIZABLE;
    }
    if (config.topmost)
    {
        window_flags |= flags::TOPMOST;
    }

    if (config.fullscreen || (s_video_data.video_caps & caps::IS_FULLSCREEN_ONLY))
    {
        math::recti bounds = display->get_bounds();

        m_position = bounds.position;
        m_size = bounds.size;

        m_pending_flags |= flags::FULLSCREEN;
        window_flags |= flags::FULLSCREEN;
    }

    display = video::get_display_for_window(this);
    if (display)
    {
        m_last_display_id = display->id();
    }

    // Flags that should be set when the window is created
    m_flags = window_flags & (flags::HIDDEN | flags::BORDERLESS | flags::RESIZABLE | flags::TOPMOST);
    m_opacity = config.opacity;

    window_impl::create(*this);
    if (!m_impl)
    {
        return;
    }

    // Clear minimized if not on windows, only windows handles it at create rather than FinishWindowCreation,
    // but it's important or window focus will get broken on windows!
#   if !defined(VX_SYSTEM_WINDOWS)
    {
        if (m_flags & flags::MINIMIZED)
        {
            m_flags &= ~flags::MINIMIZED;
        }
    }
#   endif

    set_title(config.title);
    finish_creation(window_flags);
}

video::window::~window() {}

video::window::window(window&& w) noexcept
    : m_id(w.m_id)
    , m_title(w.m_title)
    , m_position(w.m_position), m_size(w.m_size)
    , m_min_size(w.m_min_size), m_max_size(w.m_max_size)
    , m_min_aspect(w.m_min_aspect), m_max_aspect(w.m_max_aspect)
    , m_windowed_rect(w.m_windowed_rect), m_floating_rect(w.m_floating_rect)
    , m_flags(w.m_flags), m_pending_flags(w.m_pending_flags)
    , m_last_display_id(w.m_last_display_id)
    , m_requested_fullscreen_mode(std::move(w.m_requested_fullscreen_mode))
    , m_current_fullscreen_mode(std::move(w.m_current_fullscreen_mode))
    , m_requested_fullscreen_mode_set(w.m_requested_fullscreen_mode_set)
    , m_fullscreen_exclusive(w.m_fullscreen_exclusive)
    , m_last_fullscreen_exclusive_display_id(w.m_last_fullscreen_exclusive_display_id)
    , m_opacity(w.m_opacity)
    , m_initializing(w.m_initializing)
    , m_destroying(w.m_destroying)
    , m_moving_or_resizing(w.m_moving_or_resizing)
    , m_hiding(w.m_hiding)
    , m_tiled(w.m_tiled)
    , m_sync_requested(w.m_sync_requested)
    , m_impl(std::move(w.m_impl))
{
    if (m_impl)
    {
        m_impl->m_owner = this;
    }
}

video::window& video::window::operator=(window&& w) noexcept
{
    m_id = w.m_id;
    m_title = w.m_title;
    m_position = w.m_position;
    m_size = w.m_size;
    m_min_size = w.m_min_size;
    m_max_size = w.m_max_size;
    m_min_aspect = w.m_min_aspect;
    m_max_aspect = w.m_max_aspect;
    m_windowed_rect = w.m_windowed_rect;
    m_floating_rect = w.m_floating_rect;
    m_flags = w.m_flags;
    m_pending_flags = w.m_pending_flags;
    m_last_display_id = w.m_last_display_id;
    m_requested_fullscreen_mode = std::move(w.m_requested_fullscreen_mode);
    m_current_fullscreen_mode = std::move(w.m_current_fullscreen_mode);
    m_requested_fullscreen_mode_set = w.m_requested_fullscreen_mode_set;
    m_opacity = w.m_opacity;
    m_initializing = w.m_initializing;
    m_destroying = w.m_destroying;
    m_moving_or_resizing = w.m_moving_or_resizing;
    m_hiding = w.m_hiding;
    m_tiled = w.m_tiled;
    m_sync_requested = w.m_sync_requested;

    m_impl = std::move(w.m_impl);
    if (m_impl)
    {
        m_impl->m_owner = this;
    }

    return *this;
}

void video::window::finish_creation(flags::type new_flags)
{
    // drag and drop
    apply_flags(new_flags);

    if (!(new_flags & flags::HIDDEN))
    {
        show();
    }
}

void video::window::apply_flags(flags::type new_flags)
{
    if (!(new_flags & (flags::MINIMIZED | flags::MAXIMIZED)))
    {
        restore();
    }
    if (new_flags & flags::MAXIMIZED)
    {
        maximize();
    }

    set_fullscreen(new_flags & flags::FULLSCREEN);

    if (new_flags & flags::MINIMIZED)
    {
        minimize();
    }

    // grab mouse or keyboard
}

device_id video::window::id() const
{
    return m_id;
}

void video::window::sync()
{
    if ((s_video_data.video_caps & caps::ASYNCHRONOUS_WINDOWING) && m_sync_requested)
    {
        m_impl->sync();
        m_sync_requested = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

const std::string& video::window::get_title() const
{
    return m_title;
}

void video::window::set_title(const std::string& title)
{
    if (title == m_title)
    {
        return;
    }

    m_title = title;
    m_impl->set_title(title);
}

///////////////////////////////////////////////////////////////////////////////
// position and size
///////////////////////////////////////////////////////////////////////////////

void video::window::set_resizable(bool resizable)
{
    if (resizable != is_resizable())
    {
        if (resizable)
        {
            m_flags |= flags::RESIZABLE;
        }
        else
        {
            m_flags &= ~flags::RESIZABLE;
            m_windowed_rect = m_floating_rect;
        }

        m_impl->set_resizable(resizable);
    }
}

bool video::window::is_resizable() const
{
    return m_flags & flags::RESIZABLE;
}

math::vec2i video::window::get_position() const
{
    if (is_fullscreen())
    {
        const display* d = video::get_display_for_window(this);
        if (d)
        {
            return d->get_bounds().position;
        }
    }
    
    return m_position;
}

void video::window::set_position(const math::vec2i& position)
{
    m_floating_rect.position = position;

    if (!(s_video_data.video_caps & caps::STATIC_WINDOW))
    {
        m_impl->set_position();
        sync();
    }
}

const math::vec2i& video::window::get_size() const
{
    return m_size;
}

void video::window::set_size(const math::vec2i& size)
{
    int w = size.x;
    int h = size.y;

    if (w <= 0)
    {
        w = 1;
    }
    if (h <= 0)
    {
        h = 1;
    }

    float new_aspect = static_cast<float>(w) / static_cast<float>(h);
    if (m_max_aspect > 0.0f && new_aspect > m_max_aspect)
    {
        w = static_cast<int>(math::round(h * m_max_aspect));
    }
    else if (m_min_aspect > 0.0f && new_aspect < m_min_aspect)
    {
        h = static_cast<int>(math::round(w * m_min_aspect));
    }

    if (m_min_size.x > 0 && w < m_min_size.x)
    {
        w = m_min_size.x;
    }
    if (m_max_size.x > 0 && w > m_max_size.x)
    {
        w = m_max_size.x;
    }
    if (m_min_size.y > 0 && h < m_min_size.y)
    {
        h = m_min_size.y;
    }
    if (m_max_size.y > 0 && h > m_max_size.y)
    {
        h = m_max_size.y;
    }

    m_floating_rect.size.x = w;
    m_floating_rect.size.y = h;

    if (!(s_video_data.video_caps & caps::STATIC_WINDOW))
    {
        m_impl->set_size();
        sync();
    }
}

math::recti video::window::get_rect() const
{
    return math::recti(m_position, m_size);
}

const math::vec2i& video::window::get_min_size() const
{
    return m_min_size;
}

const math::vec2i& video::window::get_max_size() const
{
    return m_max_size;
}

void video::window::set_min_size(const math::vec2i& size)
{
    int w = size.x;
    int h = size.y;

    if (w < 0 || h < 0)
    {
        // error
        return;
    }

    if ((w && w > m_max_size.x) || (h && h > m_max_size.y))
    {
        // error
        return;
    }

    m_min_size.x = w;
    m_min_size.y = h;

    w = m_min_size.x ? math::max(m_min_size.x, m_floating_rect.size.x) : m_floating_rect.size.x;
    h = m_min_size.y ? math::max(m_min_size.y, m_floating_rect.size.y) : m_floating_rect.size.y;
    set_size(math::vec2i(w, h));
}

void video::window::set_max_size(const math::vec2i& size)
{
    int w = size.x;
    int h = size.y;

    if (w < 0 || h < 0)
    {
        // error
        return;
    }

    if ((w && w < m_min_size.x) || (h && h < m_min_size.y))
    {
        // error
        return;
    }

    m_max_size.x = w;
    m_max_size.y = h;

    w = m_max_size.x ? math::min(m_max_size.x, m_floating_rect.size.x) : m_floating_rect.size.x;
    h = m_max_size.y ? math::max(m_max_size.y, m_floating_rect.size.y) : m_floating_rect.size.y;
    set_size(math::vec2i(w, h));
}

float video::window::get_min_aspect() const
{
}

float video::window::get_max_aspect() const
{
}

void video::window::set_min_aspect(float aspect)
{
}

void video::window::set_max_aspect(float aspect)
{
}

///////////////////////////////////////////////////////////////////////////////
// window operators
///////////////////////////////////////////////////////////////////////////////

void video::window::show()
{
    if (!(m_flags & flags::HIDDEN))
    {
        return;
    }

    m_impl->show();
}

bool video::window::is_visible() const
{
    return m_flags & ~flags::HIDDEN;
}

bool video::window::is_hidden() const
{
    return !is_visible();
}

void video::window::minimize()
{
    m_impl->minimize();
}

bool video::window::is_minimized() const
{
    return m_flags & flags::MINIMIZED;
}

void video::window::maximize()
{
    m_impl->maximize();
}

bool video::window::is_maximized() const
{
    return m_flags & flags::MAXIMIZED;
}

void video::window::restore()
{
    m_impl->restore();
}

void video::window::focus()
{
    m_impl->focus();
}

bool video::window::is_focused() const
{
    return m_impl->is_focused();
}

void video::window::request_attention()
{
    m_impl->request_attention();
}

///////////////////////////////////////////////////////////////////////////////
// fullscreen
///////////////////////////////////////////////////////////////////////////////

const video::display_mode* video::window::get_fullscreen_mode() const
{
    const display* d = video::get_display(m_current_fullscreen_mode.m_display_id);

    if (is_fullscreen())
    {
        return d ? d->find_mode(m_current_fullscreen_mode) : nullptr;
    }

    const display_mode* mode = nullptr;

    if (!d)
    {
        d = video::get_display_for_window(this);
    }
    if (d)
    {
        if (m_requested_fullscreen_mode_set)
        {
            mode = d->find_closest_mode(
                m_requested_fullscreen_mode.resolution.x,
                m_requested_fullscreen_mode.resolution.y,
                m_requested_fullscreen_mode.refresh_rate
            );
        }
        if (!mode)
        {
            mode = d->find_mode(d->get_current_mode());
        }
    }

    return mode;
}

bool video::window::set_fullscreen_mode(const display_mode& mode)
{
    if (IS_FULLSCREEN_VISIBLE(this))
    {
        update_fullscreen_mode(true, true);
        sync();
    }

    return true;
}

bool video::window::update_fullscreen_mode(bool fullscreen, bool commit)
{
    bool success = false;

    display* d = nullptr;
    const display_mode* mode = nullptr;

    if (m_hiding || m_destroying)
    {
        fullscreen = false;
    }

    if (fullscreen)
    {
        if (!d)
        {
            d = video::get_display_for_fullscreen_window(this);
        }
        if (!d)
        {
            goto done;
        }
    }
    else
    {
        // Check if the window is currently fullscreen on any display
        for (display* od : video::list_displays())
        {
            if (od->m_fullscreen_window_id == m_id)
            {
                d = od;
                break;
            }
        }
    }

    if (fullscreen)
    {
        // maybe pass in display here
        mode = get_fullscreen_mode();
        if (mode)
        {
            m_fullscreen_exclusive = true;
        }
        else
        {
            m_current_fullscreen_mode.m_display_id = 0;
        }
    }

    if (d)
    {
        for (display* od : video::list_displays())
        {
            if (d != od && od->m_fullscreen_window_id == m_id)
            {
                od->clear_mode();
                od->m_fullscreen_window_id = 0;
            }
        }
    }

    if (fullscreen)
    {
        if (d->m_fullscreen_window_id != m_id)
        {
            window* w = video::get_window(d->m_fullscreen_window_id);
            if (w)
            {
                w->minimize();
            }
        }

        display_mode m = *mode;
        if (!d->set_current_mode(m))
        {
            goto error;
        }

        if (commit)
        {
            success = m_impl->set_fullscreen(fullscreen, d);
            if (!success)
            {
                goto error;
            }

            if (!is_fullscreen())
            {
                video::post_window_enter_fullscreen(this);
            }
        }

        if (is_fullscreen())
        {
            d->m_fullscreen_window_id = m_id;

            if (!(s_video_data.video_caps & caps::SENDS_FULLSCREEN_DIMENSIONS))
            {
                math::vec2i size = mode ? mode->resolution : d->get_desktop_mode().resolution;

                if (get_size() == size)
                {
                    video::post_window_resized(this, size);
                }
                else
                {
                    video::on_window_resized(this);
                }
            }

            if (!(s_video_data.video_caps & video::caps::DISABLE_FULLSCREEN_MOUSE_WARP))
            {
                // restore cursor position
            }
        }
    }
    else
    {
        if (d)
        {
            d->clear_mode();
        }

        if (commit)
        {
            display* fsd = d ? d : video::get_display_for_fullscreen_window(this);
            success = m_impl->set_fullscreen(fullscreen, fsd);
            if (!success)
            {
                goto error;
            }

            if (is_fullscreen())
            {
                video::post_window_leave_fullscreen(this);
            }
        }

        if (!is_fullscreen())
        {
            if (d)
            {
                d->m_fullscreen_window_id = 0;
            }

            if (!(s_video_data.video_caps & caps::SENDS_FULLSCREEN_DIMENSIONS))
            {
                video::on_window_resized(this);
            }

            if (d && !(s_video_data.video_caps & video::caps::DISABLE_FULLSCREEN_MOUSE_WARP))
            {
                // restore cursor position
            }
        }
    }

    done:
    {
        if (d && is_fullscreen() && m_fullscreen_exclusive)
        {
            m_last_fullscreen_exclusive_display_id = d->m_id;
        }
        else
        {
            m_last_fullscreen_exclusive_display_id = 0;
        }

        return true;
    }

    error:
    {
        if (fullscreen)
        {
            update_fullscreen_mode(false, commit);
        }

        return false;
    }
}

bool video::window::is_fullscreen() const
{
    return m_flags & flags::FULLSCREEN;
}

bool video::window::set_fullscreen(bool fullscreen)
{
    if (is_hidden())
    {
        if (fullscreen)
        {
            m_pending_flags |= flags::FULLSCREEN;
        }
        else
        {
            m_pending_flags &= ~flags::FULLSCREEN;
        }

        return true;
    }

    if (fullscreen)
    {
        m_current_fullscreen_mode = m_requested_fullscreen_mode;
    }

    bool success = update_fullscreen_mode(fullscreen, true);
    if (!fullscreen || !success)
    {
        m_current_fullscreen_mode.m_display_id = 0;
    }
    sync();

    return success;
}

///////////////////////////////////////////////////////////////////////////////
// icon
///////////////////////////////////////////////////////////////////////////////

bool video::window::set_icon(const uint8_t* pixels, const math::vec2i& size)
{
    return m_impl->set_icon(pixels, size);
}

void video::window::clear_icon()
{
    m_impl->clear_icon();
}

///////////////////////////////////////////////////////////////////////////////
// mouse
///////////////////////////////////////////////////////////////////////////////

math::vec2i video::window::get_mouse_position() const
{
    return m_impl->get_mouse_position();
}

void video::window::set_mouse_position(const math::vec2i& position)
{
    m_impl->set_mouse_position(position);
}

bool video::window::is_hovered() const
{
    return m_impl->is_hovered();
}

bool video::window::get_cursor_visibility() const
{
    return m_impl->get_cursor_visibility();
}

void video::window::set_cursor_visibility(bool visible)
{
    m_impl->set_cursor_visibility(visible);
}

}
}