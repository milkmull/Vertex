#include "vertex_impl/app/video/_platform/platform_window.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

#if defined(TRANSPARENT)
#   undef TRANSPARENT
#endif

namespace vx {
namespace app {
namespace video {

//=============================================================================
// window_instance_impl
//=============================================================================

void window_instance_impl_deleter::operator()(window_instance_impl* ptr) const noexcept
{
    if (ptr) { delete ptr; }
}

//=============================================================================
// creation
//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2378

bool window_instance::create(video_instance* owner, const window_config& config)
{
    if (!owner || video)
    {
        return false;
    }

    video = owner;
    data.id = video->data.window_id_generator.next();
    data.initializing = true;

    data.position = config.position;
    data.size = config.size;

    // Some drivers don't support 0 sized windows
    if (data.size.x <= 0)
    {
        data.size.x = 1;
    }
    if (data.size.y <= 0)
    {
        data.size.y = 1;
    }

#if VX_VIDEO_BACKEND_HAVE_REFRESH_DISPLAYS

    video->impl_ptr->refresh_displays();

#endif // VX_VIDEO_BACKEND_HAVE_REFRESH_DISPLAYS

    // Find the display for the window
    display_id display = config.display_id;
    {
        if (!video->is_display_connected(display))
        {
            display = video->get_primary_display();
        }
        if (!is_valid_id(display))
        {
            return false;
        }
    }

    if (config.center_on_display)
    {
        const math::recti bounds = video->get_display_work_area(display);
        data.position = bounds.position + (bounds.size - data.size) / 2;
    }

    data.floating_rect.position = data.windowed_rect.position = data.position;
    data.floating_rect.size = data.windowed_rect.size = data.size;
    data.pending_display_id = display;

    // collect all flags that should be set based on the configuration
    typename window_flags::type window_flags = window_flags::NONE;

    if (config.minimized)
    {
        window_flags |= window_flags::MINIMIZED;
    }
    if (config.maximized)
    {
        window_flags |= window_flags::MAXIMIZED;
    }
    if (config.hidden)
    {
        window_flags |= window_flags::HIDDEN;
    }
    if (config.borderless)
    {
        window_flags |= window_flags::BORDERLESS;
    }
    if (config.resizable)
    {
        window_flags |= window_flags::RESIZABLE;
    }
    if (config.topmost)
    {
        window_flags |= window_flags::TOPMOST;
    }

    if (config.fullscreen || VX_VIDEO_BACKEND_FULLSCREEN_ONLY)
    {
        const math::recti bounds = video->get_display_bounds(display);

        data.position = bounds.position;
        data.size = bounds.size;

        data.pending_flags |= window_flags::FULLSCREEN;
        window_flags |= window_flags::FULLSCREEN;
    }

    // Flags that should be set by the os when the window is created
    data.flags = window_flags & window_flags::CREATE_FLAGS;
    data.display_scale.x = data.display_scale.y = 1.0f;
    data.opacity = 1.0f;
    data.destroying = false;
    data.current_display_id = video->get_display_for_window(data.id, false);

    impl_ptr.reset(new window_instance_impl);
    if (!impl_ptr || !impl_ptr->create(this))
    {
        return false;
    }

    // Clear minimized if not on windows, only windows handles it
    // at create rather than FinishWindowCreation, but it's important
    // or window focus will get broken on windows!
#if !defined(VX_VIDEO_BACKEND_WINDOWS)
    if (data.flags & window_flags::MINIMIZED)
    {
        data.flags &= ~window_flags::MINIMIZED;
    }
#endif // VX_VIDEO_BACKEND_WINDOWS

    data.initializing = false;

    set_title(config.title);
    finish_creation(window_flags, config.drag_and_drop);

    // make sure pixel size is up to date
    check_pixel_size_changed();

    return true;
}

//=============================================================================

bool window_instance::recreate(typename window_flags::type flags)
{
    // Restore video mode, etc.
    const bool restore_on_show = data.restore_on_show;
    hide();
    data.restore_on_show = restore_on_show;

    // destroy the window surface
    destroy_surface();

#if VX_VIDEO_BACKEND_HAVE_WINDOW_DESTROY

    impl_ptr->destroy();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_DESTROY

    data.flags = window_flags::CREATE_FLAGS | window_flags::HIDDEN;
    data.destroying = false;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_CREATE

    // Reset the position and size to the floating value so
    // the recreated window has the proper base position and size
    data.position = data.windowed_rect.position = data.floating_rect.position;
    data.size = data.windowed_rect.size = data.floating_rect.size;

    if (!impl_ptr->create(this))
    {
        return false;
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_CREATE

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_TITLE

    if (!data.title.empty())
    {
        impl_ptr->set_title(data.title);
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_TITLE

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ICON

    if (!data.icon.empty())
    {
        return impl_ptr->set_icon(data.icon);
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ICON

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MIN_SIZE

    if (data.min_size.x || data.min_size.y)
    {
        impl_ptr->set_min_size();
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MIN_SIZE

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MAX_SIZE

    if (data.max_size.x || data.max_size.y)
    {
        impl_ptr->set_max_size();
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MAX_SIZE

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ASPECT_RATIO

    if (data.locked_aspect)
    {
        impl_ptr->set_aspect_ratio();
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ASPECT_RATIO

    finish_creation(flags, data.drag_and_drop_enabled);
    return true;
}

//=============================================================================

void window_instance::finish_creation(typename window_flags::type new_flags, bool drag_and_drop)
{
    toggle_drag_and_drop(drag_and_drop);

    apply_flags(new_flags);

    if (!(new_flags & window_flags::HIDDEN))
    {
        show();
    }
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L4375

void window_instance::begin_destroy()
{
    data.destroying = true;
    post_window_destroyed();

    destroy_surface();

    // restore original fullscreen mode
    update_fullscreen_mode(fullscreen_op::LEAVE, true);
    hide();
}

//=============================================================================

void window_instance::destroy()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_DESTROY

    if (impl_ptr)
    {
        impl_ptr->destroy();
        impl_ptr.reset();
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_DESTROY
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2271

void window_instance::apply_flags(typename window_flags::type new_flags)
{
    if (!(new_flags & (window_flags::MINIMIZED | window_flags::MAXIMIZED)))
    {
        restore();
    }
    if (new_flags & window_flags::MAXIMIZED)
    {
        maximize();
    }
    
    set_fullscreen(new_flags & window_flags::FULLSCREEN);
    
    if (new_flags & window_flags::MINIMIZED)
    {
        minimize();
    }

    if (new_flags & window_flags::MOUSE_GRABBED)
    {
        set_mouse_grab(true);
    }
    if (new_flags & window_flags::KEYBOARD_GRABBED)
    {
        set_keyboard_grab(true);
    }
}

//=============================================================================
// surface
//=============================================================================

//=============================================================================
// sync
//=============================================================================

VX_API void window::sync()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID();

    window_instance* w = s_video_ptr->get_window_instance(m_id);
    if (w)
    {
        w->sync();
    }
}

void window_instance::sync()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SYNC

    impl_ptr->sync();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SYNC
}

void window_instance::sync_if_required()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SYNC

    if (video->data.sync_window_operations)
    {
        sync();
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SYNC
}

//=============================================================================
// title
//=============================================================================

VX_API std::string window::get_title() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(std::string{});
    window_instance* w = s_video_ptr->get_window_instance(m_id);
    return w ? w->get_title() : std::string{};
}

const std::string& window_instance::get_title() const
{
    return data.title;
}

//=============================================================================

VX_API bool window::set_title(const std::string& title)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    window_instance* w = s_video_ptr->get_window_instance(m_id);
    return w ? w->set_title(title) : false;
}

bool window_instance::set_title(const std::string& title)
{
    if (title == data.title)
    {
        return true;
    }

    data.title = title;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_TITLE

    return impl_ptr->set_title(title);

#else

    return true;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_TITLE
}

//=============================================================================
// position and size
//=============================================================================

VX_API bool window::set_resizable(bool resizable)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    window_instance* w = s_video_ptr->get_window_instance(m_id);
    return w ? w->set_resizable(resizable) : false;
}

bool window_instance::set_resizable(bool resizable)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_RESIZABLE

    const bool is_resizable = (data.flags & window_flags::RESIZABLE);

    if (resizable != is_resizable)
    {
        if (resizable)
        {
            data.flags |= window_flags::RESIZABLE;
        }
        else
        {
            data.flags &= ~window_flags::RESIZABLE;
            data.windowed_rect = data.floating_rect;
        }

        return impl_ptr->set_resizable(resizable);
    }

    return true;

#else

    VX_UNSUPPORTED("set_resizable()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_RESIZABLE
}

//=============================================================================

VX_API bool window::is_resizable() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    window_instance* w = s_video_ptr->get_window_instance(m_id);
    return w ? w->is_resizable() : false;
}

bool window_instance::is_resizable() const
{
    return (data.flags & window_flags::RESIZABLE);
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3048

bool window_instance::get_position(int32_t* x, int32_t* y) const
{
    if (x)
    {
        *x = 0;
    }
    if (y)
    {
        *y = 0;
    }

#if defined(VX_OS_MACOS)

    // On newer MacBooks, the fullscreen window might be placed
    // below the camera notch, so use the actual window position
    const bool use_display_origin = false;

#else

    // fullscreen windows should always be at the origin of their display
    const bool use_display_origin = (data.flags & window_flags::FULLSCREEN);

#endif // VX_OS_MACOS

    if (use_display_origin)
    {
        const display_id display = video->get_display_for_window(data.id, false);
        if (is_valid_id(display))
        {
            const math::recti bounds = video->get_display_bounds(display);

            if (x)
            {
                *x = bounds.position.x;
            }
            if (y)
            {
                *y = bounds.position.y;
            }
        }

        // if fullscreen, default is the origin
        return true;
    }

    // Some drivers delay position updates for hidden windows until they
    // are shown again. If the window is hidden and a move has been requested,
    // the actual position may still reflect the old value. In that case, we
    // return the pending position instead of the driver-reported one.
    // Once the driver applies the move, data.last_position_pending is cleared,
    // ensuring this logic always returns the most up-to-date position available.
    const bool use_pending = (data.flags & window_flags::HIDDEN) && data.last_position_pending;

    if (x)
    {
        *x = use_pending ? data.pending_rect.position.x : data.position.x;
    }
    if (y)
    {
        *y = use_pending ? data.pending_rect.position.y : data.position.y;
    }

    return true;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2976

bool window_instance::set_position(int32_t x, int32_t y)
{
    const math::vec2i position{ x, y };

    // See if the requested window position matches the origin of any displays and set
    // the pending fullscreen display ID if it does. This needs to be set early in case
    // the window is prevented from moving to the exact origin due to struts.
    data.pending_display_id = video->get_display_at_origin(position);

    data.pending_rect.position = position;
    // set the position as pending while we wait for the driver to adjust
    data.last_position_pending = true;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_POSITION

    if (!impl_ptr->set_position())
    {
        return false;
    }

    sync_if_required();
    return true;

#else

    VX_UNSUPPORTED("set_position()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_POSITION
}

//=============================================================================

bool window_instance::get_size(int32_t* x, int32_t* y) const
{
    if (x)
    {
        *x = data.size.x;
    }
    if (y)
    {
        *y = data.size.y;
    }

    return true;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3072

bool window_instance::set_size(int32_t w, int32_t h)
{
    if (w <= 0)
    {
        err::set(err::INVALID_ARGUMENT, "w");
        return false;
    }
    if (h <= 0)
    {
        err::set(err::INVALID_ARGUMENT, "h");
        return false;
    }

    // Note: Aspect ratio is applied first, but min/max constraints
    // may override it. This means the final size may not perfectly
    // match the locked aspect ratio if constraints are stricter.
    if (data.locked_aspect > 0.0f)
    {
        const float new_aspect_ratio = static_cast<float>(w) / static_cast<float>(h);

        if (new_aspect_ratio > data.locked_aspect)
        {
            // Adjust width to maintain aspect ratio
            w = static_cast<int>(math::round(h * data.locked_aspect));
        }
        else if (new_aspect_ratio < data.locked_aspect)
        {
            // Adjust height to maintain aspect ratio
            h = static_cast<int>(math::round(w / data.locked_aspect));
        }
    }

    // Enforce minimum and maximum size constraints
    if (data.min_size.x > 0 && w < data.min_size.x)
    {
        w = data.min_size.x;
    }
    if (data.max_size.x > 0 && w > data.max_size.x)
    {
        w = data.max_size.x;
    }
    if (data.min_size.y > 0 && h < data.min_size.y)
    {
        h = data.min_size.y;
    }
    if (data.max_size.y > 0 && h > data.max_size.y)
    {
        h = data.max_size.y;
    }

    data.last_size_pending = true;
    data.pending_rect.size.x = w;
    data.pending_rect.size.y = h;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_SIZE

    if (!impl_ptr->set_size())
    {
        return false;
    }

    sync_if_required();
    return true;

#else

    VX_UNSUPPORTED("set_size()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_SIZE
}

math::vec2i window_instance::get_center() const
{
    math::vec2i position;
    get_position(&position.x, &position.y);

    math::vec2i size;
    get_size(&size.x, &size.y);

    return position + (size / 2);
}

math::recti window_instance::get_rect() const
{
    int32_t x, y, w, h;

    get_position(&x, &y);
    get_size(&w, &h);

    return math::recti(x, y, w, h);
}

//=============================================================================

bool window_instance::get_border_size(int32_t* left, int32_t* right, int32_t* bottom, int32_t* top) const
{
    int32_t dummy = 0;

    if (!left)
    {
        left = &dummy;
    }
    if (!right)
    {
        right = &dummy;
    }
    if (!bottom)
    {
        bottom = &dummy;
    }
    if (!top)
    {
        top = &dummy;
    }

    *top = *left = *bottom = *right = 0;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_GET_BORDER_SIZE
    
    return impl_ptr->get_border_size(left, right, bottom, top);

#else

    VX_UNSUPPORTED("get_border_size()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_GET_BORDER_SIZE
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3269

bool window_instance::get_size_in_pixels(int32_t* w, int32_t* h) const
{
    int32_t dummy;

    if (!w)
    {
        w = &dummy;
    }
    if (!h)
    {
        h = &dummy;
    }

#if VX_VIDEO_BACKEND_HAVE_WINDOW_GET_SIZE_IN_PIXELS

    return impl_ptr->get_size_in_pixels(w, h);

#else

    const display_id id = video->get_display_for_window(data.id, true);
    if (!is_valid_id(id))
    {
        return false;
    }

    const display_instance* display = video->get_display_instance(id);
    if (!display)
    {
        return false;
    }

    get_size(w, h);

    const display_mode& mode = display->get_current_mode();
    const float pixel_density = mode.pixel_density;

    *w = math::ceil(*w * pixel_density);
    *h = math::ceil(*h * pixel_density);

    return true;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_GET_SIZE_IN_PIXELS
}

float window_instance::get_pixel_density() const
{
    int32_t window_w, window_h;
    int32_t pixel_w, pixel_h;

    if (get_size(&window_w, &window_h) && get_size_in_pixels(&pixel_w, &pixel_h))
    {
        return static_cast<float>(pixel_w) / window_w;
    }

    return 1.0f;
}

//=============================================================================

bool window_instance::get_min_size(int32_t* w, int32_t* h) const
{
    if (w)
    {
        *w = data.min_size.x;
    }
    if (h)
    {
        *h = data.min_size.y;
    }

    return true;
}

bool window_instance::get_max_size(int32_t* w, int32_t* h) const
{
    if (w)
    {
        *w = data.max_size.x;
    }
    if (h)
    {
        *h = data.max_size.y;
    }

    return true;
}

//=============================================================================

bool window_instance::set_min_size(int32_t w, int32_t h)
{
    if (w < 0)
    {
        err::set(err::INVALID_ARGUMENT, "w");
        return false;
    }
    if (h < 0)
    {
        err::set(err::INVALID_ARGUMENT, "h");
        return false;
    }

    if ((data.max_size.x && w > data.max_size.x) ||
        (data.max_size.y && h > data.max_size.y))
    {
        err::set(err::SYSTEM_ERROR, "min size cannot be larger than max size");
        return false;
    }

    data.min_size.x = w;
    data.min_size.y = h;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MIN_SIZE

    impl_ptr->set_min_size();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MIN_SIZE

    // ensure window is not smaller than min size
    math::vec2i adjusted_size = data.last_size_pending ? data.pending_rect.size : data.floating_rect.size;
    adjusted_size.x = data.min_size.x ? math::max(data.min_size.x, adjusted_size.x) : adjusted_size.x;
    adjusted_size.y = data.min_size.y ? math::max(data.min_size.y, adjusted_size.y) : adjusted_size.y;

    return set_size(adjusted_size.x, adjusted_size.y);
}

bool window_instance::set_max_size(int32_t w, int32_t h)
{
    if (w < 0)
    {
        err::set(err::INVALID_ARGUMENT, "w");
        return false;
    }
    if (h < 0)
    {
        err::set(err::INVALID_ARGUMENT, "h");
        return false;
    }

    if ((data.min_size.x && w < data.min_size.x) ||
        (data.min_size.y && h < data.min_size.y))
    {
        err::set(err::SYSTEM_ERROR, "max size cannot be smaller than min size");
        return false;
    }

    data.max_size.x = w;
    data.max_size.y = h;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MAX_SIZE

    impl_ptr->set_max_size();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MAX_SIZE

    // ensure window is not larger than max size
    math::vec2i adjusted_size = data.last_size_pending ? data.pending_rect.size : data.floating_rect.size;
    adjusted_size.x = data.max_size.x ? math::min(data.max_size.x, adjusted_size.x) : adjusted_size.x;
    adjusted_size.y = data.max_size.y ? math::min(data.max_size.y, adjusted_size.y) : adjusted_size.y;

    return set_size(adjusted_size.x, adjusted_size.y);
}

//=============================================================================

float window_instance::get_aspect_ratio() const
{
    return static_cast<float>(data.size.x) / static_cast<float>(data.size.y);
}

float window_instance::get_locked_aspect_ratio() const
{
    return data.locked_aspect;
}

bool window_instance::lock_aspect_ratio(float aspect_ratio)
{
    data.locked_aspect = math::max(0.0f, aspect_ratio);

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ASPECT_RATIO

    impl_ptr->set_aspect_ratio();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ASPECT_RATIO

    return set_size(data.floating_rect.size.x, data.floating_rect.size.y);
}

//=============================================================================
// safe area
//=============================================================================

bool window_instance::set_safe_area(const math::recti& area)
{
    if (data.safe_inset == area)
    {
        return true;
    }

    data.safe_inset = area;
    post_window_safe_area_changed(area);

    return true;
}

math::recti window_instance::get_safe_area() const
{
    return data.safe_inset;
}

//=============================================================================
// bordered
//=============================================================================

bool window_instance::set_bordered(bool bordered)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_BORDERED

    const bool is_bordered = !(data.flags & window_flags::BORDERLESS);

    if (bordered != is_bordered)
    {
        if (bordered)
        {
            data.flags &= ~window_flags::BORDERLESS;
        }
        else
        {
            data.flags |= window_flags::BORDERLESS;
        }

        return impl_ptr->set_bordered();

    }

    return true;

#else

    VX_UNSUPPORTED("set_bordered()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_BORDERED
}

bool window_instance::is_bordered() const
{
    return !(data.flags & window_flags::BORDERLESS);
}

//=============================================================================
// always on top
//=============================================================================

bool window_instance::set_always_on_top(bool always_on_top)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ALWAYS_ON_TOP

    const bool is_always_on_top = (data.flags & window_flags::TOPMOST);

    if (always_on_top != is_always_on_top)
    {
        if (always_on_top)
        {
            data.flags |= window_flags::TOPMOST;
        }
        else
        {
            data.flags &= ~window_flags::TOPMOST;
        }


        return impl_ptr->set_always_on_top();

    }

    return true;

#else

    VX_UNSUPPORTED("set_always_on_top()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ALWAYS_ON_TOP
}

bool window_instance::is_always_on_top() const
{
    return (data.flags & window_flags::TOPMOST);
}

//=============================================================================
// window operators
//=============================================================================

bool window_instance::show()
{
    if (!(data.flags & window_flags::HIDDEN))
    {
        return true;
    }

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SHOW

    impl_ptr->show();

#else

    video->set_all_focus(data.id);

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SHOW

    post_window_shown();
    return true;
}

//=============================================================================

bool window_instance::hide()
{
    if (data.flags & window_flags::HIDDEN)
    {
        data.restore_on_show = false;
        return true;
    }

    data.pending_flags = (data.flags & window_flags::PENDING_FLAGS);

    data.hiding = true;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_HIDE

    impl_ptr->hide();

#else

    video->set_all_focus(INVALID_ID);

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_HIDE

    data.hiding = false;

    post_window_hidden();
    return true;
}

//=============================================================================

bool window_instance::is_visible() const
{
    return !(data.flags & window_flags::HIDDEN);
}

bool window_instance::is_hidden() const
{
    return !is_visible();
}

bool window_instance::is_occluded() const
{
    return (data.flags & window_flags::OCCLUDED);
}

bool window_instance::is_exposed() const
{
    return !is_occluded();
}

//=============================================================================

bool window_instance::minimize()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_MINIMIZE

    if (data.flags & window_flags::HIDDEN)
    {
        // we will minimize when shown again
        data.pending_flags |= window_flags::MINIMIZED;
        return true;
    }

    impl_ptr->minimize();
    sync_if_required();

    return true;

#else

    VX_UNSUPPORTED("minimize()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_MINIMIZE
}

bool window_instance::is_minimized() const
{
    return (data.flags & window_flags::MINIMIZED);
}

//=============================================================================

bool window_instance::maximize()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_MAXIMIZE

    if (!(data.flags & window_flags::RESIZABLE))
    {
        return false;
    }

    if (data.flags & window_flags::HIDDEN)
    {
        // we will maximize when shown again
        data.pending_flags |= window_flags::MAXIMIZED;
        return true;
    }

    impl_ptr->maximize();
    sync_if_required();

    return true;

#else

    VX_UNSUPPORTED("maximize()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_MAXIMIZE
}

bool window_instance::is_maximized() const
{
    return (data.flags & window_flags::MAXIMIZED);
}

//=============================================================================

bool window_instance::restore()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_RESTORE

    if (data.flags & window_flags::HIDDEN)
    {
        // remove any pending minimize or maximize states
        data.pending_flags &= ~(window_flags::MINIMIZED | window_flags::MAXIMIZED);
        return true;
    }

    impl_ptr->restore();
    sync_if_required();

    return true;

#else

    VX_UNSUPPORTED("restore()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_RESTORE
}

//=============================================================================

bool window_instance::raise()
{
    if (data.flags & window_flags::HIDDEN)
    {
        return true;
    }

#if VX_VIDEO_BACKEND_HAVE_WINDOW_RAISE

    impl_ptr->raise();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_RAISE

    return true;
}

//=============================================================================
// system
//=============================================================================

bool window_instance::flash(window_flash_op operation)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_FLASH

    return impl_ptr->flash(operation);

#else

    VX_UNSUPPORTED("flash()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_FLASH
}

//=============================================================================
// fullscreen
//=============================================================================

bool window_instance::is_fullscreen() const
{
    return (data.flags & window_flags::FULLSCREEN);
}

bool window_instance::is_fullscreen_visible() const
{
    return ((data.flags & window_flags::FULLSCREEN) && !(data.flags & window_flags::HIDDEN) && !(data.flags & window_flags::MINIMIZED));
}

//=============================================================================

const display_mode& window_instance::get_fullscreen_mode() const
{
    const display_mode_instance* mode = nullptr;

    if (data.flags & window_flags::FULLSCREEN)
    {
        mode = video->find_display_mode_for_display(data.current_fullscreen_mode.display, data.current_fullscreen_mode);
    }
    else if (data.requested_fullscreen_mode.is_set())
    {
        mode = video->find_display_mode_for_display(data.requested_fullscreen_mode.display, data.requested_fullscreen_mode);
    }

    if (!mode)
    {
        // fallback to desktop mode for current display
        const display_id d = video->get_display_for_window(data.id, false);
        mode = video->get_display_desktop_mode(d);
    }

    // mode should always be set
    VX_ASSERT(mode);
    return mode->data.mode;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2127

bool window_instance::set_fullscreen_mode(const display_mode* mode)
{
    if (mode)
    {
        // Attempt to auto-detect the target display.
        // Passing INVALID_ID triggers automatic display resolution inside
        // find_display_mode_for_display(), which defaults to the display set in `mode`
        // (if valid) or the primary display otherwise.
        //
        // Callers may pre-set `mode.display` to force a specific display.
        if (!video->find_display_mode_for_display(INVALID_ID, *mode))
        {
            err::set(err::SYSTEM_ERROR, "invalid fullscreen display mode");
            return false;
        }

        data.requested_fullscreen_mode = *mode;
    }
    else
    {
        // clear the requested mode to indicate use of the desktop mode
        data.requested_fullscreen_mode.clear();
    }

    // Mirror the requested mode into the "current" mode immediately
    //
    // This ensures that if the driver is already in the middle of an asynchronous
    // fullscreen transition (e.g. entering fullscreen), we don’t end up with stale
    // data. The actual active mode may still change when the transition completes,
    // but this keeps the app’s internal state consistent until then.
    data.current_fullscreen_mode = data.requested_fullscreen_mode;

#if defined(VX_VIDEO_BACKEND_COCOA)

    // On macOS, entering fullscreen involves an OS-level "Spaces" animation
    // and transition that temporarily detaches the window from normal rendering
    // control. If we try to change display modes during that transition,
    // the window can get stuck in a corrupted or undefined state.
    //
    // So here, we wait until the transition is finished before continuing.

    if (impl_ptr->in_fullscreen_space_transition())
    {
        sync();
    }

#endif // VX_VIDEO_BACKEND_COCOA

    if (is_fullscreen_visible())
    {
        update_fullscreen_mode(fullscreen_op::UPDATE, true);
        sync_if_required();
    }

    return true;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1904

bool window_instance::update_fullscreen_mode(typename fullscreen_op::type fullscreen, bool commit)
{
    // Track the display we are operating on
    display_id target_display_id = INVALID_ID;
    const display_mode* target_mode = nullptr;

    // Reset fullscreen state flags
    data.fullscreen_exclusive = false;

    // We are explicitly requesting a fullscreen chnage. If we are
    // in an event watcher responding to a display change event,
    // prevent override of the new configuration by
    // "on display changed" function.
    data.update_fullscreen_on_display_changed = false;

    // If the window is being hidden or destroyed, force leaving fullscreen
    if (data.hiding || data.destroying)
    {
        fullscreen = fullscreen_op::LEAVE;
    }

    // Determine the relevant display based on fullscreen operation
    if (fullscreen)
    {
        target_display_id = video->get_display_for_window(data.id, false);
        if (!is_valid_id(target_display_id))
        {
            goto done; // Should never happen
        }
    }
    else
    {
        // Check if the window is currently fullscreen on any display
        target_display_id = video->find_display_with_fullscreen_window(data.id);
    }

    if (fullscreen)
    {
        target_mode = &get_fullscreen_mode();
    }

#if defined(VX_VIDEO_BACKEND_COCOA)

    // ------------------ COCOA-SPECIFIC LOGIC ------------------
    // Cocoa has two types of fullscreen:
    // 1. Fullscreen Spaces (macOS native fullscreen)  
    // 2. Exclusive fullscreen (true fullscreen mode for games, controlled display resolution)
    //
    // Switching between these modes requires careful ordering:
    // - We may need to leave one type before entering the other
    // - If we destroy the window without a last exclusive fullscreen, skip mode changes to avoid flicker

    if (data.destroying && !is_valid_id(data.last_fullscreen_exclusive_display_id))
    {
        // If window is being destroyed and was never in exclusive fullscreen,
        // just reset the exclusive flag and remove fullscreen reference from the display

        data.fullscreen_exclusive = false;

        if (is_valid_id(target_display_id))
        {
            video->set_display_fullscreen_window_id(d, INVALID_ID);
        }

        goto done;
    }

    if (commit)
    {
        // Switching between fullscreen Space and exclusive fullscreen requires a temporary reset
        if (fullscreen && impl_ptr->in_fullscreen_space() && !is_valid_id(data.last_fullscreen_exclusive_display_id) && data.fullscreen_exclusive)
        {
            // If currently in a Space (Cocoa fullscreen) but we want exclusive fullscreen:
            // temporarily leave fullscreen Space first. This avoids a visual glitch.

            if (!impl_ptr->set_fullscreen_space(false, true))
            {
                goto error;
            }
        }
        else if (fullscreen && is_valid_id(data.last_fullscreen_exclusive_display_id) && !data.fullscreen_exclusive)
        {
            // If previously in exclusive fullscreen but now want fullscreen Space:
            // reset display modes for this window first
            video->reset_display_modes_for_window(data.id, target_display_id);
        }

        // Attempt to apply fullscreen Space directly if supported
        // This may succeed immediately, in which case we are done
        if (impl_ptr->set_fullscreen_space(fullscreen, true))
        {
            goto done;
        }
    }

#endif // VX_VIDEO_BACKEND_COCOA

    // Restore previous display modes if switching displays
    if (is_valid_id(target_display_id))
    {
        video->reset_display_modes_for_window(data.id, target_display_id);
    }

    if (fullscreen)
    {
        // ------------------ ENTER FULLSCREEN ------------------

        bool resized = false;

        // target_display_id is guarenteed not to be null here
        VX_ASSERT(is_valid_id(target_display_id));

        display_instance* display_inst = video->get_display_instance(target_display_id);
        if (!display_inst)
        {
            goto error;
        }

        if (display_inst->data.fullscreen_window_id != data.id)
        {
            // Minimize any other fullscreen window on this display
            window_instance* other_window = video->get_window_instance(display_inst->data.fullscreen_window_id);
            if (other_window)
            {
                other_window->minimize();
            }
        }

        display_inst->data.fullscreen_active = data.fullscreen_exclusive;

        // Apply target fullscreen mode
        if (!display_inst->set_display_mode(*target_mode))
        {
            goto error;
        }

        if (commit)
        {
            fullscreen_result result = fullscreen_result::SUCCEEDED;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_FULLSCREEN
            // Platform-specific API to actually enter fullscreen
            result = impl_ptr->set_fullscreen(fullscreen, *display_inst);
#else
            // On platforms without this API, we must resize the window manually
            resized = true;
#endif

            if (result == fullscreen_result::SUCCEEDED)
            {
                // Window is fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
                if (!(data.flags & window_flags::FULLSCREEN))
                {
                    post_window_enter_fullscreen();
                }
            }
            else if (result == fullscreen_result::FAILED)
            {
                display_inst->data.fullscreen_active = false;
                goto error;
            }

            if (data.flags & window_flags::FULLSCREEN)
            {
                display_inst->data.fullscreen_window_id = data.id;

                // If the OS does not generate an event telling us the final window
                // size, we must manually adjust the position and size here.

                if (!VX_VIDEO_BACKEND_SENDS_FULLSCREEN_DIMENSIONS)
                {
                    const math::rect display_rect = video->get_display_bounds(target_mode->display);

                    if (data.size != target_mode->resolution)
                    {
                        resized = true;
                    }

                    post_window_moved(display_rect.position.x, display_rect.position.y);

                    if (resized)
                    {
                        post_window_resized(target_mode->resolution.x, target_mode->resolution.y);
                    }
                    else
                    {
                        on_window_resized();
                    }
                }
            }
        }
    }
    else
    {
        // ------------------ LEAVE FULLSCREEN ------------------

        bool resized = false;
        display_instance* display_inst = video->get_display_instance(target_display_id);

        // Restore desktop mode if display exists
        if (display_inst)
        {
            display_inst->data.fullscreen_active = false;
            display_inst->reset_display_mode(false);
        }

        if (commit)
        {
            fullscreen_result result = fullscreen_result::SUCCEEDED;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_FULLSCREEN

            display_instance* target_display_inst = display_inst ? display_inst : video->get_display_instance(video->get_display_for_window(data.id, false));

            if (target_display_inst)
            {
                result = impl_ptr->set_fullscreen(fullscreen_op::LEAVE, *target_display_inst);
            }

#else

            resized = true;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_FULLSCREEN

            if (result == fullscreen_result::SUCCEEDED)
            {
                // Window left fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
                if (!(data.flags & window_flags::FULLSCREEN))
                {
                    post_window_leave_fullscreen();
                }
            }
            else if (result == fullscreen_result::FAILED)
            {
                goto error;
            }
        }

        if (!(data.flags & window_flags::FULLSCREEN))
        {
            if (display_inst)
            {
                display_inst->data.fullscreen_window_id = INVALID_ID;
            }

            if (!VX_VIDEO_BACKEND_SENDS_FULLSCREEN_DIMENSIONS)
            {
                // If the OS does not generate an event telling us the final window
                // size, we must manually adjust the position and size here.

                post_window_moved(data.windowed_rect.position.x, data.windowed_rect.position.y);

                if (resized)
                {
                    post_window_resized(data.windowed_rect.size.x, data.windowed_rect.size.y);
                }
                else
                {
                    on_window_resized();
                }
            }
        }
    }

    done:
    {
        // This is used by Cocoa to know if the last fullscreen was exclusive, which affects Space transitions
        data.last_fullscreen_exclusive_display_id
            = (is_valid_id(target_display_id) && (data.flags & window_flags::FULLSCREEN) && data.fullscreen_exclusive)
            ? target_display_id
            : INVALID_ID;

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

//=============================================================================

bool window_instance::set_fullscreen(bool fullscreen)
{
    if (data.flags & window_flags::HIDDEN)
    {
        if (fullscreen)
        {
            data.pending_flags |= window_flags::FULLSCREEN;
        }
        else
        {
            data.pending_flags &= ~window_flags::FULLSCREEN;
        }

        return true;
    }

    if (fullscreen)
    {
        // update the current mode to the requested mode
        // so it will be used when we call update_fullscreen_mode
        data.current_fullscreen_mode = data.requested_fullscreen_mode;
    }

    const bool result = update_fullscreen_mode(fullscreen ? fullscreen_op::ENTER : fullscreen_op::LEAVE, true);

    if (!fullscreen || !result)
    {
        data.current_fullscreen_mode.clear();
    }

    if (result)
    {
        sync_if_required();
    }

    return result;
}

//=============================================================================
// icon
//=============================================================================

bool window_instance::set_icon(const surface_argb& surf)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ICON

    data.icon = surf;

    if (surf.empty())
    {
        return false;
    }

    return impl_ptr->set_icon(surf);

#else

    VX_UNSUPPORTED("set_icon()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_ICON
}

const surface_argb& window_instance::get_icon() const
{
    return data.icon;
}

//=============================================================================
// shape
//=============================================================================

bool window_instance::set_shape(const surface_argb& shape)
{
    if (!(data.flags & window_flags::TRANSPARENT))
    {
        err::set(err::SYSTEM_ERROR, "window must be created as transparent");
        return false;
    }

    data.shape_surface = shape;
    return update_shape(true);
}

bool window_instance::update_shape(bool force)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_UPDATE_SHAPE

    if ((data.flags & window_flags::TRANSPARENT) && (force || !data.shape_surface.empty()))
    {
        return impl_ptr->update_shape(data.shape_surface);
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_UPDATE_SHAPE

    return true;
}

//=============================================================================

float window_instance::get_opacity() const
{
    return data.opacity;
}

bool window_instance::set_opacity(float opacity)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_OPACITY

    opacity = math::clamp(opacity, 0.0f, 1.0f);

    const bool result = impl_ptr->set_opacity(opacity);
    if (result)
    {
        data.opacity = opacity;
    }

    return result;

#else

    VX_UNSUPPORTED("set_opacity()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_OPACITY
}

//=============================================================================

std::vector<uint8_t> window_instance::get_icc_profile() const
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_GET_ICC_PROFILE

    return impl_ptr->get_icc_profile();

#else

    VX_UNSUPPORTED("get_icc_profile()");
    return std::vector<uint8_t>{};

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_GET_ICC_PROFILE
}

//=============================================================================

pixel::pixel_format window_instance::get_pixel_format() const
{
    const display_id display = video->get_display_for_window(data.id, true);
    const display_mode* mode = video->get_display_current_mode(display);
    return mode ? mode->pixel_format : pixel::pixel_format::UNKNOWN;
}

//=============================================================================
// grab
//=============================================================================

bool window_instance::get_mouse_grab() const
{
    return video->data.grabbed_window == data.id && (data.flags & window_flags::MOUSE_GRABBED);
}

bool window_instance::get_keyboard_grab() const
{
    return video->data.grabbed_window == data.id && (data.flags & window_flags::KEYBOARD_GRABBED);
}

bool window_instance::set_mouse_grab(bool grabbed)
{
    if (data.flags & window_flags::HIDDEN)
    {
        if (grabbed)
        {
            data.pending_flags |= window_flags::MOUSE_GRABBED;
        }
        else
        {
            data.pending_flags &= ~window_flags::MOUSE_GRABBED;
        }

        return true;
    }

    if (grabbed == !!(data.flags & window_flags::MOUSE_GRABBED))
    {
        data.flags |= window_flags::MOUSE_GRABBED;
    }
    else
    {
        data.flags &= ~window_flags::MOUSE_GRABBED;
    }

    update_grab();

    if (grabbed && !(data.flags & window_flags::MOUSE_GRABBED))
    {
        return false;
    }

    return true;
}

bool window_instance::set_keyboard_grab(bool grabbed)
{
    if (data.flags & window_flags::HIDDEN)
    {
        if (grabbed)
        {
            data.pending_flags |= window_flags::KEYBOARD_GRABBED;
        }
        else
        {
            data.pending_flags &= ~window_flags::KEYBOARD_GRABBED;
        }

        return true;
    }

    if (grabbed == !!(data.flags & window_flags::KEYBOARD_GRABBED))
    {
        data.flags |= window_flags::KEYBOARD_GRABBED;
    }
    else
    {
        data.flags &= ~window_flags::KEYBOARD_GRABBED;
    }

    update_grab();

    if (grabbed && !(data.flags & window_flags::KEYBOARD_GRABBED))
    {
        return false;
    }

    return true;
}

void window_instance::clear_grab()
{
    data.flags &= ~(window_flags::MOUSE_GRABBED | window_flags::KEYBOARD_GRABBED);

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_GRAB

    impl_ptr->set_mouse_grab(false);

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_GRAB

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_KEYBOARD_GRAB

    impl_ptr->set_keyboard_grab(false);

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_KEYBOARD_GRAB
}

void window_instance::update_grab()
{
    bool mouse_grabbed = false;
    bool keyboard_grabbed = false;

    if (data.flags & window_flags::INPUT_FOCUS)
    {
        if (video->data.mouse_ptr->data.relative_mode.enabled || (data.flags & window_flags::MOUSE_GRABBED))
        {
            mouse_grabbed = true;
        }
        else
        {
            mouse_grabbed = false;
        }

        if (data.flags & window_flags::KEYBOARD_GRABBED)
        {
            keyboard_grabbed = true;
        }
        else
        {
            keyboard_grabbed = false;
        }
    }

    if (mouse_grabbed || keyboard_grabbed)
    {
        // stealing grab from another window
        video->set_grabbed_window(data.id);
    }
    else if (video->data.grabbed_window == data.id)
    {
        video->set_grabbed_window(INVALID_ID);
    }

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_GRAB

    if (!impl_ptr->set_mouse_grab(mouse_grabbed))
    {
        data.flags &= ~window_flags::MOUSE_GRABBED;
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_GRAB

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_KEYBOARD_GRAB

    if (!impl_ptr->set_keyboard_grab(keyboard_grabbed))
    {
        data.flags &= ~window_flags::KEYBOARD_GRABBED;
    }

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_KEYBOARD_GRAB

    video->validate_grabbed_window();
}

//=============================================================================
// focus
//=============================================================================

bool window_instance::set_focusable(bool focusable)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_FOCUSABLE

    const bool is_focusable = !(data.flags & window_flags::NOT_FOCUSABLE);

    if (focusable != is_focusable)
    {
        if (!focusable)
        {
            data.flags |= window_flags::NOT_FOCUSABLE;
        }
        else
        {
            data.flags &= ~window_flags::NOT_FOCUSABLE;
        }

        return impl_ptr->set_focusable(focusable);
    }

    return true;

#else

    VX_UNSUPPORTED("set_focusable()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_FOCUSABLE
}

bool window_instance::has_mouse_focus() const
{
    return video->data.mouse_ptr->get_focus() == data.id;
}

bool window_instance::has_keyboard_focus() const
{
    return video->data.keyboard_ptr->get_focus() == data.id;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L4370

bool window_instance::should_minimize_on_focus_loss() const
{
    if (!(data.flags & window_flags::FULLSCREEN) || data.destroying)
    {
        return false;
    }

#if defined(VX_OS_MACOS)

    // On macOS:
    //
    // - Some fullscreen windows can show modal dialogs (e.g. file pickers).
    // - These dialogs are system-level and appear above the fullscreen window.
    // - In such cases, we should NOT minimize the fullscreen window.

    if (impl_ptr->is_showing_modal_dialogue())
    {
        // modal system dialogs can live over fullscreen windows, don't minimize.
        return false;
    }

    // macOS also has the concept of a "fullscreen space" — a virtual desktop
    // dedicated to a fullscreen app. When a window is in such a space,
    // losing focus doesn't require minimizing, since macOS manages visibility.

    if (impl_ptr->in_fullscreen_space())
    {
        return false;
    }

#endif // VX_OS_MACOS

#if defined(VX_PLATFORM_ANDROID)



#endif // VX_PLATFORM_ANDROID

    // Real fullscreen windows should minimize on focus loss so the desktop video mode is restored
    const bool hint = video->app->data.hints_ptr->get_hint_boolean(
        HINT_AND_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOW_MINIMIZE_ON_FOCUS_LOSS)
    );

    if (hint)
    {
        return true;
    }

    // - If this is an exclusive fullscreen mode (one that changed the display mode)
    // - And if the video mode switching is *not* emulated (i.e., it's a real mode switch)
    // Then we minimize on focus loss to restore the desktop video mode.
    return data.fullscreen_exclusive && !VX_VIDEO_BACKEND_MODE_SWITCHING_EMULATED;
}

//=============================================================================
// mouse
//=============================================================================

math::recti window_instance::get_mouse_rect() const
{
    return data.mouse_rect;
}

bool window_instance::set_mouse_rect(const math::recti& rect)
{
    data.mouse_rect = rect;

#if VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_RECT

    return impl_ptr->set_mouse_rect(rect);

#else

    return true;

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_SET_MOUSE_RECT
}

//=============================================================================
// drag and drop
//=============================================================================

bool window_instance::toggle_drag_and_drop(bool enabled)
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_ACCEPT_DRAG_AND_DROP

    return impl_ptr->toggle_drag_and_drop(enabled);

#else

    VX_UNSUPPORTED("toggle_drag_and_drop()");
    return false;

#endif
}

//=============================================================================
// events
//=============================================================================

void window_instance::send_wakeup_event() const
{
#if VX_VIDEO_BACKEND_HAVE_SEND_WAKEUP_EVENT

    impl_ptr->send_wakeup_event();

#endif // VX_VIDEO_BACKEND_HAVE_SEND_WAKEUP_EVENT
}

//=============================================================================

#define events_ptr video->app->data.events_ptr

static bool filter_duplicate_window_events(const event::event& e, void* user_data)
{
    const event::event* new_event = reinterpret_cast<const event::event*>(user_data);
    return (e.type == new_event->type) && (e.window_event.common.window_id == new_event->window_event.common.window_id);
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_windowevents.c#L71

bool window_instance::post_window_shown()
{
    if (!(data.flags & window_flags::HIDDEN))
    {
        return false;
    }

    data.flags &= ~(window_flags::HIDDEN | window_flags::MINIMIZED);

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_SHOWN;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_shown();
    return posted;
}

void window_instance::on_window_shown()
{
    // Set window state if we have pending window flags cached
    apply_flags(data.pending_flags);
    data.pending_flags = window_flags::NONE;
}

//=============================================================================

bool window_instance::post_window_hidden()
{
    if (data.flags & window_flags::HIDDEN)
    {
        return false;
    }

    data.flags |= window_flags::HIDDEN;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_HIDDEN;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_hidden();
    return posted;
}

void window_instance::on_window_hidden()
{
    // We will restore the state of these flags when the window is shown again
    data.pending_flags |= (data.flags & (window_flags::FULLSCREEN | window_flags::MAXIMIZED));
    // The window is already hidden at this point, so just change the mode back if necessary
    update_fullscreen_mode(fullscreen_op::LEAVE, false);
}

//=============================================================================

bool window_instance::post_window_occluded()
{
    if (data.flags & window_flags::OCCLUDED)
    {
        return false;
    }

    data.flags |= window_flags::OCCLUDED;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_OCCLUDED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, &e);

    on_window_occluded();
    return posted;
}

//=============================================================================

bool window_instance::post_window_exposed()
{
    data.flags &= ~window_flags::OCCLUDED;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_HIDDEN;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, &e);

    on_window_exposed();
    return posted;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_windowevents.c#L99

bool window_instance::post_window_moved(int32_t x, int32_t y)
{
    // Clear the pending display if this move was not the result of an explicit request,
    // and the window is not scheduled to become fullscreen when shown.
    if (data.last_position_pending && !(data.pending_flags & window_flags::FULLSCREEN))
    {
        data.pending_display_id = INVALID_ID;
    }

    data.last_position_pending = false;

    if (!(data.flags & window_flags::FULLSCREEN))
    {
        data.windowed_rect.position.x = x;
        data.windowed_rect.position.y = y;

        if (!(data.flags & window_flags::MAXIMIZED) && !data.tiled)
        {
            data.floating_rect.position.x = x;
            data.floating_rect.position.y = y;
        }
    }

    if (data.position.x == x && data.position.y == y)
    {
        return false;
    }

    data.position.x = x;
    data.position.y = y;

    if (data.destroying)
    {
        return false;
    }

    event::flush_events(event::WINDOW_MOVED);

    event::event e{};
    e.type = event::WINDOW_MOVED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_moved.x = x;
    e.window_event.window_moved.y = y;

    // remove any window move events currently in the queue
    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, static_cast<void*>(&e));

    on_window_moved();
    return posted;
}

void window_instance::on_window_moved()
{
    check_window_display_changed();
}

//=============================================================================

bool window_instance::post_window_resized(int32_t w, int32_t h)
{
    data.last_size_pending = false;

    if (!(data.flags & window_flags::FULLSCREEN))
    {
        data.windowed_rect.size.x = w;
        data.windowed_rect.size.y = h;

        if (!(data.flags & window_flags::MAXIMIZED) && !data.tiled)
        {
            data.floating_rect.size.x = w;
            data.floating_rect.size.y = h;
        }
    }

    if (data.size.x == w && data.size.y == h)
    {
        check_pixel_size_changed();
        return false;
    }

    data.size.x = w;
    data.size.y = h;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_RESIZED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_resized.w = w;
    e.window_event.window_resized.h = h;

    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, static_cast<void*>(&e));

    on_window_resized();
    return posted;
}

void window_instance::on_window_resized()
{
    check_window_display_changed();
    check_pixel_size_changed();
    on_window_safe_area_changed();
    update_shape(false);
}

//=============================================================================

void window_instance::check_pixel_size_changed()
{
    int32_t w, h;
    get_size_in_pixels(&w, &h);
    post_window_pixel_size_changed(w, h);

    check_display_scale_changed();
}

bool window_instance::post_window_pixel_size_changed(int32_t w, int32_t h)
{
    if (data.last_pixel_size.x == w && data.last_pixel_size.y == h)
    {
        return false;
    }

    data.last_pixel_size.x = w;
    data.last_pixel_size.y = h;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_RESIZED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_pixel_size_changed.w = w;
    e.window_event.window_pixel_size_changed.h = h;

    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, static_cast<void*>(&e));

    on_window_pixel_size_changed();
    return posted;
}

void window_instance::on_window_pixel_size_changed()
{
    data.surface_valid = false;
}

//=============================================================================

void window_instance::check_display_scale_changed()
{
    math::vec2 display_scale;

#if VX_VIDEO_HAVE_WINDOW_GET_CONTENT_SCALE

    display_scale = impl_ptr->get_display_scale();

#else

    const float pixel_density = get_pixel_density();
    const math::vec2 content_scale = video->get_display_content_scale(data.current_display_id);

    display_scale = content_scale * pixel_density;

#endif // VX_VIDEO_HAVE_WINDOW_GET_CONTENT_SCALE

    if (display_scale != data.display_scale)
    {
        data.display_scale = display_scale;
        post_window_display_scale_changed();
    }
}

bool window_instance::post_window_display_scale_changed()
{
    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_DISPLAY_SCALE_CHANGED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_display_scale_changed.xscale = data.display_scale.x;
    e.window_event.window_display_scale_changed.yscale = data.display_scale.y;
    const bool posted = events_ptr->push_event(e);

    on_window_display_scale_changed();
    return posted;
}

//=============================================================================

bool window_instance::post_window_safe_area_changed(const math::recti& area)
{
    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_SAFE_AREA_CHANGED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_safe_area_changed.x = area.position.x;
    e.window_event.window_safe_area_changed.y = area.position.y;
    e.window_event.window_safe_area_changed.w = area.size.x;
    e.window_event.window_safe_area_changed.h = area.size.y;
    const bool posted = events_ptr->push_event_filtered(e, filter_duplicate_window_events, &e);

    on_window_safe_area_changed();
    return posted;
}

//=============================================================================

bool window_instance::post_window_minimized()
{
    if (data.flags & window_flags::MINIMIZED)
    {
        return false;
    }

    data.flags &= ~window_flags::MAXIMIZED;
    data.flags |= window_flags::MINIMIZED;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_MINIMIZED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_minimized();
    return posted;
}

void window_instance::on_window_minimized()
{
    if (data.flags & window_flags::FULLSCREEN)
    {
        update_fullscreen_mode(fullscreen_op::LEAVE, false);
    }
}

//=============================================================================

bool window_instance::post_window_maximized()
{
    if (data.flags & window_flags::MAXIMIZED)
    {
        return false;
    }

    data.flags &= ~window_flags::MINIMIZED;
    data.flags |= window_flags::MAXIMIZED;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_MAXIMIZED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_maximized();
    return posted;
}

//=============================================================================

bool window_instance::post_window_restored()
{
    if (!(data.flags & (window_flags::MINIMIZED | window_flags::MAXIMIZED)))
    {
        return false;
    }

    data.flags &= ~(window_flags::MINIMIZED | window_flags::MAXIMIZED);

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_RESTORED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_restored();
    return posted;
}

void window_instance::on_window_restored()
{
    if (data.flags & window_flags::FULLSCREEN)
    {
        update_fullscreen_mode(fullscreen_op::ENTER, true);
    }
}

//=============================================================================

bool window_instance::post_window_enter_fullscreen()
{
    if (data.flags & window_flags::FULLSCREEN)
    {
        return false;
    }

    data.flags |= window_flags::FULLSCREEN;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_ENTER_FULLSCREEN;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_enter_fullscreen();
    return posted;
}

//=============================================================================

bool window_instance::post_window_leave_fullscreen()
{
    if (!(data.flags & window_flags::FULLSCREEN))
    {
        return false;
    }

    data.flags &= ~window_flags::FULLSCREEN;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_LEAVE_FULLSCREEN;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_leave_fullscreen();
    return posted;
}

//=============================================================================

bool window_instance::post_window_gained_focus()
{
    if (data.flags & window_flags::INPUT_FOCUS)
    {
        return false;
    }

    data.flags |= window_flags::INPUT_FOCUS;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_GAINED_FOCUS;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_gained_focus();
    return posted;
}

void window_instance::on_window_gained_focus()
{
    if (video->data.mouse_ptr->data.relative_mode.enabled)
    {
        video->data.mouse_ptr->set_focus(data.id);
    }

    update_grab();
}

//=============================================================================

bool window_instance::post_window_lost_focus()
{
    if (!(data.flags && window_flags::INPUT_FOCUS))
    {
        return false;
    }

    data.flags &= ~window_flags::INPUT_FOCUS;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_LOST_FOCUS;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_lost_focus();
    return posted;
}

void window_instance::on_window_lost_focus()
{
    update_grab();

    if (should_minimize_on_focus_loss())
    {
        minimize();
    }
}

//=============================================================================

bool window_instance::post_window_mouse_enter()
{
    if (data.flags & window_flags::MOUSE_FOCUS)
    {
        return false;
    }

    data.flags |= window_flags::MOUSE_FOCUS;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_MOUSE_ENTER;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_mouse_enter();
    return posted;
}

void window_instance::on_window_mouse_enter()
{
#if VX_VIDEO_BACKEND_HAVE_WINDOW_ON_MOUSE_ENTER

    impl_ptr->on_window_mouse_enter();

#endif // VX_VIDEO_BACKEND_HAVE_WINDOW_ON_MOUSE_ENTER
}

//=============================================================================

bool window_instance::post_window_mouse_leave()
{
    if (!(data.flags & window_flags::MOUSE_FOCUS))
    {
        return false;
    }

    data.flags &= ~window_flags::MOUSE_FOCUS;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_MOUSE_LEAVE;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_mouse_leave();
    return posted;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1797

void window_instance::check_window_display_changed()
{
#if VX_VIDEO_SENDS_DISPLAY_CHANGES

    return;

#else

    const display_id display = video->get_display_for_window(data.id, true);

    if (display != data.current_display_id)
    {
        // See if we are fully committed to the new display
        // 80% is about the right value, tested with 350% scale on the left monitor and 100% scale on the right

        const math::recti old_bounds = video->get_display_bounds(data.current_display_id);
        const math::recti new_bounds = video->get_display_bounds(display);
        const math::recti window_rect = math::rect(data.position, data.size);

        const math::recti old_intersection = math::g2::crop(old_bounds, window_rect);
        const math::recti new_intersection = math::g2::crop(new_bounds, window_rect);

        const int old_area = old_intersection.area();
        const int new_area = new_intersection.area();

        // only accept the new display if we are no longer 80% on it
        const float new_overlap_ratio = (new_area / (static_cast<float>(old_area) + new_area));
        if (new_overlap_ratio < 0.80f)
        {
            return;
        }

        video->set_display_fullscreen_window(display, data.id, true);
    }

#endif // VX_VIDEO_SENDS_DISPLAY_CHANGES
}

bool window_instance::post_window_display_changed(display_id d)
{
    if (!is_valid_id(d) || data.current_display_id == d)
    {
        return false;
    }

    data.update_fullscreen_on_display_changed = true;
    data.current_display_id = d;

    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_DISPLAY_CHANGED;
    e.window_event.common.window_id = data.id;
    e.window_event.window_display_changed.display_id = d;
    const bool posted = events_ptr->push_event(e);

    on_window_display_changed(d);
    return posted;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L4198

void window_instance::on_window_display_changed(display_id d)
{
    // Prevent override of configuration if a fullscreen change was made
    // in an event watcher callback in response to a display changed event
    if (data.update_fullscreen_on_display_changed && (data.flags & window_flags::FULLSCREEN))
    {
        const bool auto_mode_switch = video->app->data.hints_ptr->get_hint_boolean(
            HINT_AND_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOW_MATCH_EXCLUSIVE_MODE_ON_MOVE)
        );

        bool clear_requested_mode = true;

        if (auto_mode_switch && data.requested_fullscreen_mode.is_set())
        {
            const display_id display = video->get_display_for_window(data.id, true);
            const bool include_high_density_modes = data.requested_fullscreen_mode.is_high_density();

            const display_mode_instance* new_mode = video->find_closest_display_mode_for_display(
                display, data.requested_fullscreen_mode, include_high_density_modes, true
            );

            if (new_mode)
            {
                // set the current mode to our desired mode so when
                // we update the fullscreen mode, it will be used
                data.current_fullscreen_mode = new_mode->data.mode;
                clear_requested_mode = false;
            }
        }

        // if we couldn't find a match or we just don't care what
        // the mode is, reset the requested mode so the desktop
        // mode will be used
        if (clear_requested_mode)
        {
            data.requested_fullscreen_mode.clear();
        }

        if (is_fullscreen_visible())
        {
            update_fullscreen_mode(fullscreen_op::UPDATE, true);
        }
    }

    check_pixel_size_changed();
}

//=============================================================================

bool window_instance::post_window_close_requested()
{
    if (data.destroying)
    {
        return false;
    }

    event::event e{};
    e.type = event::WINDOW_CLOSE_REQUESTED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_close_requested();
    return posted;
}

void window_instance::on_window_close_requested()
{
    if (video->should_quit_on_window_close())
    {
        video->app->post_app_quit();
    }
}

//=============================================================================

bool window_instance::post_window_destroyed()
{
    event::event e{};
    e.type = event::WINDOW_DESTROYED;
    e.window_event.common.window_id = data.id;
    const bool posted = events_ptr->push_event(e);

    on_window_destroyed();
    return posted;
}

} // namespace video
} // namespace app
} // namespace vx