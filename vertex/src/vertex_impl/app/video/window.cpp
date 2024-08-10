#include "vertex/config.h"
#include "vertex/app/event/event.h"
#include "vertex/app/video/window.h"

#if defined(VX_SYSTEM_WINDOWS)
#   include "_win32/win32_window.h"
#endif

namespace vx {
namespace app {

#define IS_FULLSCREEN_VISIBLE(w) (((w)->is_fullscreen()) && ((w)->is_visible()) && !((w)->is_minimized()))

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

video::window::window()
    : m_id(get_next_device_id())
    , m_locked_aspect(0.0f)
    , m_flags(flags::NONE), m_pending_flags(flags::NONE)
    , m_last_display_id(0)
    , m_requested_fullscreen_mode_set(false)
    , m_fullscreen_exclusive(false)
    , m_last_fullscreen_exclusive_display_id(0)
    , m_opacity(0.0f)
    , m_initializing(false)
    , m_destroying(false)
    , m_repositioning(false)
    , m_hiding(false)
    , m_tiled(false)
    , m_sync_requested(false)
{
}

video::window::~window()
{
    destroy();
}

video::window::window(window&& w) noexcept
    : m_id(w.m_id)
    , m_title(w.m_title)
    , m_position(w.m_position), m_size(w.m_size)
    , m_min_size(w.m_min_size), m_max_size(w.m_max_size)
    , m_locked_aspect(w.m_locked_aspect)
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
    , m_hiding(w.m_hiding)
    , m_repositioning(w.m_repositioning)
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
    m_locked_aspect = w.m_locked_aspect;
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
    m_hiding = w.m_hiding;
    m_repositioning = w.m_repositioning;
    m_tiled = w.m_tiled;
    m_sync_requested = w.m_sync_requested;

    m_impl = std::move(w.m_impl);
    if (m_impl)
    {
        m_impl->m_owner = this;
    }

    return *this;
}

bool video::window::create(const window_config& config)
{
    m_initializing = true;

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
        return false;
    }

    // If auto position is true, we center the window on the display

    if (config.center_on_display)
    {
        math::recti bounds = display->get_work_area();
        m_position = bounds.position + (bounds.size - m_size) / 2;
    }

    m_floating_rect.position = m_windowed_rect.position = m_position;
    m_floating_rect.size = m_windowed_rect.size = m_size;

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
    m_flags = window_flags & (flags::HIDDEN | flags::MINIMIZED | flags::BORDERLESS | flags::RESIZABLE | flags::TOPMOST);

    m_impl = std::make_unique<window_impl>();
    if (!m_impl || !m_impl->create(this))
    {
        return false;
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

    m_initializing = false;

    set_title(config.title);
    finish_creation(window_flags);

    return true;
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

void video::window::destroy()
{
    m_destroying = true;
    post_window_destroyed();

    if (m_impl)
    {
        update_fullscreen_mode(window::fullscreen_op::LEAVE, true);
        hide();
    }

    // Make sure no displays reference the window
    for (display* d : video::list_displays())
    {
        if (d->m_fullscreen_window_id == m_id)
        {
            d->m_fullscreen_window_id = 0;
        }
    }

    // Kill focus

    if (m_impl)
    {
        m_impl->destroy();
        m_impl = nullptr;
    }
}

bool video::window::validate() const
{
    return m_id && m_impl && m_impl->validate();
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
    if (m_sync_requested)
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

        m_impl->set_resizable();
    }
}

bool video::window::is_resizable() const
{
    return (m_flags & flags::RESIZABLE);
}

math::vec2i video::window::get_position() const
{
    if (m_flags & flags::FULLSCREEN)
    {
        // Fullscreen windows should always be at the origin
        // of their displays event when minimized or hidden.
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

    m_repositioning = true;
    m_impl->set_position();
    m_repositioning = false;
    sync();
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

    // Enforce locked aspect ratio if set
    if (m_locked_aspect > 0.0f)
    {
        float new_aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
        if (new_aspect_ratio > m_locked_aspect)
        {
            // Adjust width to maintain aspect ratio
            w = static_cast<int>(math::round(h * m_locked_aspect));
        }
        else if (new_aspect_ratio < m_locked_aspect)
        {
            // Adjust height to maintain aspect ratio
            h = static_cast<int>(math::round(w / m_locked_aspect));
        }
    }

    // Enforce minimum and maximum size constraints
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

    m_impl->set_size();
    sync();
}

math::vec2i video::window::get_center() const
{
    return m_position + (m_size / 2);
}

math::recti video::window::get_rect() const
{
    return math::recti(m_position, m_size);
}

void video::window::get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const
{
    left = right = bottom = top = 0;
    m_impl->get_border_size(left, right, bottom, top);
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
    int w = math::max(0, size.x);
    int h = math::max(0, size.y);

    if ((w && m_max_size.x > 0 && w > m_max_size.x) ||
        (h && m_max_size.y > 0 && h > m_max_size.y))
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
    int w = math::max(0, size.x);
    int h = math::max(0, size.y);

    if ((w && m_min_size.x && w < m_min_size.x) ||
        (h && m_min_size.y && h < m_min_size.y))
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

float video::window::get_aspect_ratio() const
{
    return static_cast<float>(m_size.x) / static_cast<float>(m_size.y);
}

void video::window::lock_aspect_ratio(float aspect_ratio)
{
    m_locked_aspect = math::max(0.0f, aspect_ratio);
    set_size(m_size);
}

///////////////////////////////////////////////////////////////////////////////
// bordered
///////////////////////////////////////////////////////////////////////////////

void video::window::set_bordered(bool bordered)
{
    if (bordered != is_bordered())
    {
        if (bordered)
        {
            m_flags &= ~flags::BORDERLESS;
        }
        else
        {
            m_flags |= flags::BORDERLESS;
        }

        m_impl->set_bordered();
    }
}

bool video::window::is_bordered() const
{
    return !(m_flags & flags::BORDERLESS);
}

///////////////////////////////////////////////////////////////////////////////
// always on top
///////////////////////////////////////////////////////////////////////////////

void video::window::set_always_on_top(bool always_on_top)
{
    if (always_on_top != is_always_on_top())
    {
        if (always_on_top)
        {
            m_flags |= flags::TOPMOST;
        }
        else
        {
            m_flags &= ~flags::TOPMOST;
        }

        m_impl->set_always_on_top();
    }
}

bool video::window::is_always_on_top() const
{
    return (m_flags & flags::TOPMOST);
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
    post_window_shown();
}

void video::window::hide()
{
    if (m_flags & flags::HIDDEN)
    {
        return;
    }

    m_pending_flags = m_flags;

    m_hiding = true;
    m_impl->hide();
    m_hiding = false;

    post_window_hidden();
}

bool video::window::is_visible() const
{
    return !(m_flags & flags::HIDDEN);
}

bool video::window::is_hidden() const
{
    return !is_visible();
}

void video::window::minimize()
{
    // check support

    if (m_flags & flags::HIDDEN)
    {
        m_pending_flags |= flags::MINIMIZED;
        return;
    }

    m_impl->minimize();
    sync();
}

bool video::window::is_minimized() const
{
    return (m_flags & flags::MINIMIZED);
}

void video::window::maximize()
{
    // check support

    if (!(m_flags & flags::RESIZABLE))
    {
        return;
    }
    
    if (m_flags & flags::HIDDEN)
    {
        m_pending_flags |= flags::MAXIMIZED;
        return;
    }

    m_impl->maximize();
    sync();
}

bool video::window::is_maximized() const
{
    return (m_flags & flags::MAXIMIZED);
}

void video::window::restore()
{
    if (m_flags & flags::HIDDEN)
    {
        m_pending_flags &= ~(flags::MINIMIZED | flags::MAXIMIZED);
        return;
    }

    m_impl->restore();
    sync();
}

void video::window::focus()
{
    m_impl->focus();
}

bool video::window::is_focused() const
{
    return m_impl->is_focused();
}

void video::window::flash(flash_op operation)
{
    m_impl->flash(operation);
}

///////////////////////////////////////////////////////////////////////////////
// fullscreen
///////////////////////////////////////////////////////////////////////////////

const video::display_mode* video::window::get_fullscreen_mode() const
{
    const display* d = video::get_display_for_window(this);
    const display_mode* mode = nullptr;

    if (d)
    {
        // Try to find our current fullscreen mode first
        mode = d->find_mode(m_current_fullscreen_mode);

        if (!mode && m_requested_fullscreen_mode_set)
        {
            // Try to find the closest match if the requested mode is set
            mode = d->find_closest_mode(
                m_requested_fullscreen_mode.resolution.x,
                m_requested_fullscreen_mode.resolution.y,
                m_requested_fullscreen_mode.refresh_rate
            );
        }

        if (!mode)
        {
            // Default to the current mode
            mode = d->find_mode(d->get_current_mode());
        }
    }

    return mode;
}

bool video::window::set_fullscreen_mode(const display_mode& mode)
{
    m_requested_fullscreen_mode = mode;
    m_requested_fullscreen_mode_set = true;

    if (IS_FULLSCREEN_VISIBLE(this))
    {
        update_fullscreen_mode(fullscreen_op::UPDATE, true);
        sync();
    }

    return true;
}

bool video::window::update_fullscreen_mode(fullscreen_op::type fullscreen, bool commit)
{
    display* d = nullptr;
    const display_mode* mode = nullptr;

    if (m_hiding || m_destroying)
    {
        fullscreen = fullscreen_op::LEAVE;
    }

    if (fullscreen)
    {
        d = video::get_display_for_window(this);
        if (!d)
        {
            // Should never happen
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
        mode = get_fullscreen_mode();
        if (!mode)
        {
            goto error;
        }

        m_fullscreen_exclusive = true;
    }

    if (d)
    {
        // If the window is switching displays, restore the display mode on the old ones
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
            // If there is another fullscreen window on the target display, minimize it
            window* w = video::get_window(d->m_fullscreen_window_id);
            if (w)
            {
                w->minimize();
            }
        }

        display_mode final_mode = *mode;
        if (!d->set_current_mode(final_mode))
        {
            goto error;
        }

        if (commit)
        {
            if (!m_impl->set_fullscreen(fullscreen, d))
            {
                goto error;
            }

            // Set the current fullscreen mode
            m_current_fullscreen_mode = final_mode;

            // Post the enter fullscreen event if it has not been posted yet
            if (!(m_flags & flags::FULLSCREEN))
            {
                post_window_enter_fullscreen();
            }
        }

        if (m_flags & flags::FULLSCREEN)
        {
            d->m_fullscreen_window_id = m_id;

            // Some backends don't send a new window size event when fullscreening
            if (!(s_video_data.video_caps & caps::SENDS_FULLSCREEN_DIMENSIONS))
            {
                if (m_size != mode->resolution)
                {
                    post_window_resized(mode->resolution.x, mode->resolution.y);
                }
                else
                {
                    on_window_resized();
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
        // Restore desktop mode
        if (d)
        {
            d->clear_mode();
        }

        if (commit)
        {
            if (!m_impl->set_fullscreen(fullscreen_op::LEAVE, (d ? d : video::get_display_for_window(this))))
            {
                goto error;
            }

            // Clear the current fullscreen mode
            m_current_fullscreen_mode.m_display_id = 0;

            // Post the leave fullscreen event if it has not been posted yet
            if (m_flags & flags::FULLSCREEN)
            {
                post_window_leave_fullscreen();
            }
        }

        if (!(m_flags & flags::FULLSCREEN))
        {
            if (d)
            {
                d->m_fullscreen_window_id = 0;
            }

            // Some backends don't send a new window size event when fullscreening
            if (!(s_video_data.video_caps & caps::SENDS_FULLSCREEN_DIMENSIONS))
            {
                if (m_size != m_windowed_rect.size)
                {
                    post_window_resized(m_windowed_rect.size.x, m_windowed_rect.size.y);
                }
                else
                {
                    on_window_resized();
                }
            }

            if (d && !(s_video_data.video_caps & video::caps::DISABLE_FULLSCREEN_MOUSE_WARP))
            {
                // restore cursor position
            }
        }
    }

    done:
    {
        if (d && (m_flags & flags::FULLSCREEN) && m_fullscreen_exclusive)
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
            // Something went wrong, exit fullscreen
            update_fullscreen_mode(fullscreen_op::LEAVE, commit);
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
    if (m_flags & flags::HIDDEN)
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

    bool success = update_fullscreen_mode(fullscreen ? fullscreen_op::ENTER : fullscreen_op::LEAVE, true);
    sync();

    return success;
}

///////////////////////////////////////////////////////////////////////////////
// icon
///////////////////////////////////////////////////////////////////////////////

bool video::window::set_icon(const pixel::surface& surf)
{
    if (surf.empty())
    {
        return false;
    }

    pixel::surface icon_surf = surf.convert(pixel::pixel_format::RGBA_8);
    return m_impl->set_icon(icon_surf);
}

void video::window::clear_icon()
{
    m_impl->clear_icon();
}

///////////////////////////////////////////////////////////////////////////////
// window events
///////////////////////////////////////////////////////////////////////////////

static bool filter_duplicate_window_events(void* user_data, const event& e)
{
    const event* ne = reinterpret_cast<const event*>(user_data);

    if (e.type == ne->type)
    {
        return e.window_id == ne->window_id;
    }

    return false;
}

bool video::window::post_window_shown()
{
    if (m_destroying)
    {
        return false;
    }

    if (!(m_flags & window::flags::HIDDEN))
    {
        return false;
    }

    m_flags &= ~(window::flags::HIDDEN | window::flags::MINIMIZED);

    event e{};
    e.type = event_type::WINDOW_SHOWN;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    on_window_shown();
    return posted;
}

void video::window::on_window_shown()
{
    apply_flags(m_pending_flags);
    m_pending_flags = window::flags::NONE;
}

bool video::window::post_window_hidden()
{
    if (m_destroying)
    {
        return false;
    }

    if (m_flags & window::flags::HIDDEN)
    {
        return false;
    }

    m_flags |= window::flags::HIDDEN;

    event e{};
    e.type = event_type::WINDOW_HIDDEN;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    on_window_hidden();
    return posted;
}

void video::window::on_window_hidden()
{
    // We will restore the state of these flags when the window is shown again
    m_pending_flags |= (m_flags & (window::flags::FULLSCREEN | window::flags::MAXIMIZED));
    update_fullscreen_mode(fullscreen_op::LEAVE, false);
}

bool video::window::post_window_moved(int32_t x, int32_t y)
{
    if (m_destroying)
    {
        return false;
    }

    if (!(m_flags & flags::FULLSCREEN))
    {
        m_windowed_rect.position.x = x;
        m_windowed_rect.position.y = y;

        if (!(m_flags & flags::MAXIMIZED) && !m_tiled)
        {
            m_floating_rect.position.x = x;
            m_floating_rect.position.y = y;
        }
    }

    if (m_position.x == x && m_position.y == y)
    {
        return false;
    }

    m_position.x = x;
    m_position.y = y;

    event::flush_events(event_type::WINDOW_MOVED);

    event e{};
    e.type = event_type::WINDOW_MOVED;
    e.window_id = m_id;
    e.window_moved.x = x;
    e.window_moved.y = y;

    event::filter_events(filter_duplicate_window_events, static_cast<void*>(&e));
    bool posted = event::post_event(e);

    on_window_moved();
    return posted;
}

void video::window::on_window_moved()
{
    video::check_window_display_changed(*this);
}

bool video::window::post_window_resized(int32_t w, int32_t h)
{
    if (m_destroying)
    {
        return false;
    }

    if (!(m_flags & flags::FULLSCREEN))
    {
        m_windowed_rect.size.x = w;
        m_windowed_rect.size.y = h;

        if (!(m_flags & flags::MAXIMIZED) && !m_tiled)
        {
            m_floating_rect.size.x = w;
            m_floating_rect.size.y = h;
        }
    }

    if (m_size.x == w && m_size.y == h)
    {
        return false;
    }

    m_size.x = w;
    m_size.y = h;

    event e{};
    e.type = event_type::WINDOW_RESIZED;
    e.window_id = m_id;
    e.window_resized.w = w;
    e.window_resized.h = h;

    event::filter_events(filter_duplicate_window_events, static_cast<void*>(&e));
    bool posted = event::post_event(e);

    on_window_resized();
    return posted;
}

void video::window::on_window_resized()
{
    video::check_window_display_changed(*this);

    // update window shape & safe area
}

bool video::window::post_window_minimized()
{
    if (m_destroying)
    {
        return false;
    }

    if (m_flags & flags::MINIMIZED)
    {
        return false;
    }

    m_flags &= ~flags::MAXIMIZED;
    m_flags |= flags::MINIMIZED;

    event e{};
    e.type = event_type::WINDOW_MINIMIZED;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    on_window_minimized();
    return posted;
}

void video::window::on_window_minimized()
{
    if (m_flags & flags::FULLSCREEN)
    {
        update_fullscreen_mode(fullscreen_op::LEAVE, false);
    }
}

bool video::window::post_window_maximized()
{
    if (m_destroying)
    {
        return false;
    }

    if (m_flags & flags::MAXIMIZED)
    {
        return false;
    }

    m_flags &= ~flags::MINIMIZED;
    m_flags |= flags::MAXIMIZED;

    event e{};
    e.type = event_type::WINDOW_MAXIMIZED;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    on_window_maximized();
    return posted;
}

void video::window::on_window_maximized()
{
}

bool video::window::post_window_restored()
{
    if (m_destroying)
    {
        return false;
    }

    if (!(m_flags & (flags::MINIMIZED | flags::MAXIMIZED)))
    {
        return false;
    }

    m_flags &= ~(flags::MINIMIZED | flags::MAXIMIZED);

    event e{};
    e.type = event_type::WINDOW_RESTORED;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    on_window_restored();
    return posted;
}

void video::window::on_window_restored()
{
    if (m_flags & flags::FULLSCREEN)
    {
        update_fullscreen_mode(fullscreen_op::ENTER, true);
    }
}

bool video::window::post_window_enter_fullscreen()
{
    if (m_destroying)
    {
        return false;
    }

    if (m_flags & window::flags::FULLSCREEN)
    {
        return false;
    }

    m_flags |= window::flags::FULLSCREEN;

    event e{};
    e.type = event_type::WINDOW_ENTER_FULLSCREEN;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video::window::post_window_leave_fullscreen()
{
    if (m_destroying)
    {
        return false;
    }

    if (!(m_flags & window::flags::FULLSCREEN))
    {
        return false;
    }

    m_flags &= ~window::flags::FULLSCREEN;

    event e{};
    e.type = event_type::WINDOW_LEAVE_FULLSCREEN;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video::window::post_window_display_changed(const display& d)
{
    if (m_destroying)
    {
        return false;
    }

    if (m_last_display_id == d.id())
    {
        return false;
    }

    m_last_display_id = d.id();

    event e{};
    e.type = event_type::WINDOW_DISPLAY_CHANGED;
    e.window_id = m_id;
    e.window_display_changed.display_id = d.id();
    bool posted = event::post_event(e);

    on_window_display_changed(d);
    return posted;
}

void video::window::on_window_display_changed(const display& d)
{
    if (is_fullscreen())
    {
        const display_mode* new_mode = nullptr;

        if (m_requested_fullscreen_mode_set)
        {
            new_mode = d.find_closest_mode(
                m_requested_fullscreen_mode.resolution.x,
                m_requested_fullscreen_mode.resolution.y,
                m_requested_fullscreen_mode.refresh_rate
            );
        }

        if (new_mode && IS_FULLSCREEN_VISIBLE(this))
        {
            update_fullscreen_mode(fullscreen_op::UPDATE, true);
        }
    }
}

bool video::window::post_window_close_requested()
{
    event e{};
    e.type = event_type::WINDOW_CLOSE_REQUESTED;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video::window::post_window_destroyed()
{
    event e{};
    e.type = event_type::WINDOW_DESTROYED;
    e.window_id = m_id;
    bool posted = event::post_event(e);

    return posted;
}

}
}