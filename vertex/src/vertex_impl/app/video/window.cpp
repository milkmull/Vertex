#include "vertex_impl/app/video/_platform/platform_window.hpp"
#include "vertex/app/event/event.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// window_instance_impl
///////////////////////////////////////////////////////////////////////////////

void window_instance_impl_deleter::operator()(window_instance_impl* ptr) const noexcept
{
    if (ptr) { delete ptr; }
}

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

#define IS_FULLSCREEN_VISIBLE(w) (((w)->data.flags & window_flags::FULLSCREEN) && !((w)->data.flags & window_flags::HIDDEN) && !((w)->data.flags & window_flags::MINIMIZED))

///////////////////////////////////////

#define VX_CHECK_WINDOW_INIT(r) \
do \
{ \
    if (!(validate())) \
    { \
        return (r); \
    } \
} while (0)

#define VX_CHECK_WINDOW_INIT_VOID() VX_CHECK_WINDOW_INIT(void())

///////////////////////////////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////////////////////////////

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

    // Some backends don't support 0 sized windows
    if (data.size.x <= 0)
    {
        data.size.x = 1;
    }
    if (data.size.y <= 0)
    {
        data.size.y = 1;
    }

#if VX_VIDEO_HAVE_REFRESH_DISPLAYS

    video->impl_ptr->refresh_displays();

#endif // VX_VIDEO_HAVE_REFRESH_DISPLAYS

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

    if (config.fullscreen || VX_VIDEO_FULLSCREEN_ONLY)
    {
        const math::recti bounds = video->get_display_bounds(display);

        data.position = bounds.position;
        data.size = bounds.size;

        data.pending_flags |= window_flags::FULLSCREEN;
        window_flags |= window_flags::FULLSCREEN;
    }

    // Flags that should be set by the os when the window is created
    data.flags = window_flags & window_flags::CREATE_FLAGS;
    data.display_scale = 1.0f;
    data.opacity = 1.0f;
    data.destroying = false;
    data.last_display_id = video->get_display_for_window(data.id, false);

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
    finish_creation(window_flags);

    return true;
}

void window_instance::finish_creation(typename window_flags::type new_flags)
{
    // drag and drop
    apply_flags(new_flags);

    if (!(new_flags & window_flags::HIDDEN))
    {
        show();
    }
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L4375

void window_instance::destroy()
{
    data.destroying = true;
    post_window_destroyed();

    // restore original fullscreen mode
    update_fullscreen_mode(fullscreen_op::LEAVE, true);
    hide();

    if (impl_ptr)
    {
        impl_ptr->destroy();
    }
}

bool window_instance::validate() const
{
    return is_valid_id(data.id) && impl_ptr != nullptr;
}

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

///////////////////////////////////////////////////////////////////////////////
// sync
///////////////////////////////////////////////////////////////////////////////

void window_instance::sync()
{
#if VX_VIDEO_HAVE_WINDOW_SYNC

    impl_ptr->sync();

#endif // VX_VIDEO_HAVE_WINDOW_SYNC
}

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

const std::string& window_instance::get_title() const
{
    return data.title;
}

bool window_instance::set_title(const std::string& title)
{
    if (title == data.title)
    {
        return true;
    }

    data.title = title;

#if VX_VIDEO_HAVE_WINDOW_SET_TITLE

    return impl_ptr->set_title(title);

#endif // VX_VIDEO_HAVE_WINDOW_SET_TITLE

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// position and size
///////////////////////////////////////////////////////////////////////////////

bool window_instance::set_resizable(bool resizable)
{
#if VX_VIDEO_HAVE_WINDOW_SET_RESIZABLE

    if (resizable != (data.flags & window_flags::RESIZABLE))
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

        impl_ptr->set_resizable(resizable);
    }

#else

    return false;

#endif // VX_VIDEO_HAVE_WINDOW_SET_RESIZABLE
}

bool window_instance::is_resizable() const
{
    return (data.flags & window_flags::RESIZABLE);
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3048

bool window_instance::get_position(int* x, int* y) const
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

#endif // VX_VIDEO_BACKEND_COCOA

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

    // Some backends delay position updates for hidden windows until they
    // are shown again. If the window is hidden and a move has been requested,
    // the actual position may still reflect the old value. In that case, we
    // return the pending position instead of the backend-reported one.
    // Once the backend applies the move, data.last_position_pending is cleared,
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

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2901

bool window_instance::set_position(int x, int y)
{
    const math::vec2i position{ x, y };

    // update the pending display id
    data.pending_display_id = video->get_display_at_origin(position);

    data.pending_rect.position = position;
    // set the position as pending while we wait for the backend to adjust
    data.last_position_pending = true;

#if VX_VIDEO_HAVE_WINDOW_SET_POSITION

    if (!impl_ptr->set_position())
    {
        return false;
    }

    sync();

#endif // VX_VIDEO_HAVE_WINDOW_SET_POSITION

    return true;
}

bool window_instance::get_size(int* x, int* y) const
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

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3072

bool window_instance::set_size(int w, int h)
{
    if (w <= 0)
    {
        w = 1;
    }
    if (h <= 0)
    {
        h = 1;
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

#if VX_VIDEO_HAVE_WINDOW_SET_SIZE

    if (!impl_ptr->set_size())
    {
        return false;
    }

    sync();

#endif // VX_VIDEO_HAVE_WINDOW_SET_SIZE

    return true;
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
    int x, y, w, h;

    get_position(&x, &y);
    get_size(&w, &h);

    return math::recti(x, y, w, h);
}

bool window_instance::get_border_size(int* left, int* right, int* bottom, int* top) const
{
    int dummy = 0;

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

#if VX_VIDEO_HAVE_WINDOW_GET_BORDER_SIZE
    
    return impl_ptr->get_border_size(left, right, bottom, top);

#endif // VX_VIDEO_HAVE_WINDOW_GET_BORDER_SIZE

    return true;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3269

bool window_instance::get_window_size_in_pixels(int* w, int* h)
{
    int dummy;

    if (!w)
    {
        w = &dummy;
    }
    if (!h)
    {
        h = &dummy;
    }

#if VX_VIDEO_HAVE_WINDOW_GET_SIZE_IN_PIXELS && 0

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

#endif // VX_VIDEO_HAVE_WINDOW_GET_SIZE_IN_PIXELS

    return true;
}

bool window_instance::get_min_size(int* w, int* h) const
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

bool window_instance::get_max_size(int* w, int* h) const
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

bool window_instance::set_min_size(int w, int h)
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

#if VX_VIDEO_HAVE_WINDOW_SET_MIN_SIZE

    impl_ptr->set_min_size();

#endif // VX_VIDEO_HAVE_WINDOW_SET_MIN_SIZE

    // ensure window is not smaller than min size
    math::vec2i adjusted_size = data.last_size_pending ? data.pending_rect.size : data.floating_rect.size;
    adjusted_size.x = data.min_size.x ? math::max(data.min_size.x, adjusted_size.x) : adjusted_size.x;
    adjusted_size.y = data.min_size.y ? math::max(data.min_size.y, adjusted_size.y) : adjusted_size.y;

    return set_size(adjusted_size.x, adjusted_size.y);
}

bool window_instance::set_max_size(int w, int h)
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

#if VX_VIDEO_HAVE_WINDOW_SET_MAX_SIZE

    impl_ptr->set_max_size();

#endif // VX_VIDEO_HAVE_WINDOW_SET_MAX_SIZE

    // ensure window is not larger than max size
    math::vec2i adjusted_size = data.last_size_pending ? data.pending_rect.size : data.floating_rect.size;
    adjusted_size.x = data.max_size.x ? math::min(data.max_size.x, adjusted_size.x) : adjusted_size.x;
    adjusted_size.y = data.max_size.y ? math::min(data.max_size.y, adjusted_size.y) : adjusted_size.y;

    return set_size(adjusted_size.x, adjusted_size.y);
}

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

#if VX_VIDEO_HAVE_WINDOW_SET_ASPECT_RATIO

    impl_ptr->set_aspect_ratio();

#endif // VX_VIDEO_HAVE_WINDOW_SET_ASPECT_RATIO

    return set_size(data.floating_rect.size.x, data.floating_rect.size.y);
}

///////////////////////////////////////////////////////////////////////////////
// bordered
///////////////////////////////////////////////////////////////////////////////

bool window_instance::set_bordered(bool bordered)
{
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

#if VX_VIDEO_HAVE_WINDOW_SET_BORDERED

        return impl_ptr->set_bordered();

#endif // VX_VIDEO_HAVE_WINDOW_SET_BORDERED
    }

    return true;
}

bool window_instance::is_bordered() const
{
    return !(data.flags & window_flags::BORDERLESS);
}

///////////////////////////////////////////////////////////////////////////////
// always on top
///////////////////////////////////////////////////////////////////////////////

bool window_instance::set_always_on_top(bool always_on_top)
{
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

#if VX_VIDEO_HAVE_WINDOW_SET_ALWAYS_ON_TOP

        return impl_ptr->set_always_on_top();

#endif // VX_VIDEO_HAVE_WINDOW_SET_ALWAYS_ON_TOP
    }

    return true;
}

bool window_instance::is_always_on_top() const
{
    return (data.flags & window_flags::TOPMOST);
}

///////////////////////////////////////////////////////////////////////////////
// window operators
///////////////////////////////////////////////////////////////////////////////

bool window_instance::show()
{
    if (!(data.flags & window_flags::HIDDEN))
    {
        return;
    }

#if VX_VIDEO_HAVE_WINDOW_SHOW

    impl_ptr->show();

#else

    video->data.mouse_ptr->set_focus(data.id);
    video->data.keyboard_ptr->set_focus(data.id);

#endif // VX_VIDEO_HAVE_WINDOW_SHOW

    post_window_shown();
}

bool window_instance::hide()
{
    if (data.flags & window_flags::HIDDEN)
    {
        data.restore_on_show = false;
        return true;
    }

    data.pending_flags = (data.flags & window_flags::PENDING_FLAGS);

    data.hiding = true;

#if VX_VIDEO_HAVE_WINDOW_HIDE

    impl_ptr->hide();

#else

    video->data.mouse_ptr->set_focus(INVALID_ID);
    video->data.keyboard_ptr->set_focus(INVALID_ID);

#endif // VX_VIDEO_HAVE_WINDOW_HIDE

    data.hiding = false;

    post_window_hidden();
    return true;
}

bool window_instance::is_visible() const
{
    return !(data.flags & window_flags::HIDDEN);
}

bool window_instance::is_hidden() const
{
    return !is_visible();
}

bool window_instance::minimize()
{
#if !VX_VIDEO_HAVE_WINDOW_MINIMIZE

    return false;

#endif // VX_VIDEO_HAVE_WINDOW_MINIMIZE

    if (data.flags & window_flags::HIDDEN)
    {
        // we will minimize when shown again
        data.pending_flags |= window_flags::MINIMIZED;
        return true;
    }

    impl_ptr->minimize();
    sync();

    return true;
}

bool window_instance::is_minimized() const
{
    return (data.flags & window_flags::MINIMIZED);
}

bool window_instance::maximize()
{
#if !VX_VIDEO_HAVE_WINDOW_MAXIMIZE

    return false;

#endif // VX_VIDEO_HAVE_WINDOW_MAXIMIZE

    if (!(data.flags & window_flags::RESIZABLE))
    {
        return false;
    }
    
    if (data.flags & window_flags::HIDDEN)
    {
        // we will maximize when shown again
        data.pending_flags |= window_flags::MAXIMIZED;
        return;
    }

    impl_ptr->maximize();
    sync();

    return true;
}

bool window_instance::is_maximized() const
{
    return (data.flags & window_flags::MAXIMIZED);
}

bool window_instance::restore()
{
#if !VX_VIDEO_HAVE_WINDOW_RESTORE

    return false;

#endif // VX_VIDEO_HAVE_WINDOW_RESTORE

    if (data.flags & window_flags::HIDDEN)
    {
        // remove any pending minimize or maximize states
        data.pending_flags &= ~(window_flags::MINIMIZED | window_flags::MAXIMIZED);
        return true;
    }

    impl_ptr->restore();
    sync();

    return true;
}

bool window_instance::flash(window_flash_op operation)
{
#if VX_VIDEO_HAVE_WINDOW_FLASH

    return impl_ptr->flash(operation);

#else

    return false;

#endif // VX_VIDEO_HAVE_WINDOW_FLASH
}

///////////////////////////////////////////////////////////////////////////////
// fullscreen
///////////////////////////////////////////////////////////////////////////////

const display_mode_instance* window_instance::get_fullscreen_mode() const
{
    const display_mode_pair& pair = (data.flags & window_flags::FULLSCREEN) ? data.current_fullscreen_mode : data.requested_fullscreen_mode;

    // first try to find an exact match
    const display_mode_instance* dmi = video->find_display_mode_for_display(pair.display, pair.mode);
    if (dmi)
    {
        return dmi;
    }

    // return the best candidate mode
    return video->find_closest_display_mode_for_display(pair.display, pair.mode);
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L2127

bool window_instance::set_fullscreen_mode(const display_mode& mode)
{
    data.requested_fullscreen_mode = mode;

    if (IS_FULLSCREEN_VISIBLE(this))
    {
        update_fullscreen_mode(fullscreen_op::UPDATE, true);
        sync();
    }

    return true;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1894

//bool window_instance::update_fullscreen_mode(fullscreen_op fullscreen, bool commit)
//{
//    display* d = nullptr;
//    const display_mode* mode = nullptr;
//
//    data.fullscreen_exclusive = false;
//
//    // If we are hiding or destroying, don't go back into fullscreen
//    if (data.hiding || data.destroying)
//    {
//        fullscreen = fullscreen_op::LEAVE;
//    }
//
//    if (fullscreen)
//    {
//        d = get_display_for_window(*this);
//        if (!d)
//        {
//            // Should never happen
//            goto done;
//        }
//    }
//    else
//    {
//        // Check if the window is currently fullscreen on any display
//        display* od = video_internal::find_display_with_fullscreen_window(*this);
//        if (od)
//        {
//            d = od;
//        }
//    }
//
//    if (fullscreen)
//    {
//        mode = get_fullscreen_mode();
//        if (!mode)
//        {
//            goto error;
//        }
//
//        data.fullscreen_exclusive = true;
//    }
//
//    if (d)
//    {
//        // If the window is switching displays, restore the display mode on the old ones
//        display* od = video_internal::find_display_with_fullscreen_window(*this);
//        if (od && d != od)
//        {
//            od->clear_mode();
//            video_internal::clear_display_fullscreen_window_id(*od);
//        }
//    }
//
//    if (fullscreen)
//    {
//        // d is guarenteed not to be null here
//
//        const device_id d_fullscreen_window_id = video_internal::get_display_fullscreen_window_id(*d);
//        if (d_fullscreen_window_id != data.id)
//        {
//            // If there is another fullscreen window on the target display, minimize it
//            window* w = get_window(d_fullscreen_window_id);
//            if (w)
//            {
//                w->minimize();
//            }
//        }
//
//        display_mode final_mode = *mode;
//        if (!d->set_current_mode(final_mode))
//        {
//            goto error;
//        }
//
//        if (commit)
//        {
//            if (!impl_ptr->set_fullscreen(fullscreen, d))
//            {
//                goto error;
//            }
//
//            // Set the current fullscreen mode
//            data.current_fullscreen_mode = final_mode;
//
//            // Post the enter fullscreen event if it has not been posted yet
//            if (!(data.flags & window_flags::FULLSCREEN))
//            {
//                post_window_enter_fullscreen();
//            }
//        }
//
//        if (data.flags & window_flags::FULLSCREEN)
//        {
//            video_internal::set_display_fullscreen_window_id(*d, data.id);
//
//            // Some backends don't send a new window size event when fullscreening
//            if (!(has_capabilities(capabilities::SENDS_FULLSCREEN_DIMENSIONS)))
//            {
//                if (data.size != mode->resolution)
//                {
//                    post_window_resized(mode->resolution.x, mode->resolution.y);
//                }
//                else
//                {
//                    on_window_resized();
//                }
//            }
//
//            if (!(has_capabilities(capabilities::DISABLE_FULLSCREEN_MOUSE_WARP)))
//            {
//                // restore cursor position
//            }
//        }
//    }
//    else
//    {
//        // Restore desktop mode
//        if (d)
//        {
//            d->clear_mode();
//        }
//
//        if (commit)
//        {
//            if (!impl_ptr->set_fullscreen(fullscreen_op::LEAVE, (d ? d : get_display_for_window(*this))))
//            {
//                goto error;
//            }
//
//            // Clear the current fullscreen mode
//            video_internal::clear_display_mode_display_id(data.current_fullscreen_mode);
//
//            // Post the leave fullscreen event if it has not been posted yet
//            if (data.flags & window_flags::FULLSCREEN)
//            {
//                post_window_leave_fullscreen();
//            }
//        }
//
//        if (!(data.flags & window_flags::FULLSCREEN))
//        {
//            if (d)
//            {
//                video_internal::clear_display_fullscreen_window_id(*d);
//            }
//
//            // Some backends don't send a new window size event when fullscreening
//            if (!(has_capabilities(capabilities::SENDS_FULLSCREEN_DIMENSIONS)))
//            {
//                if (data.size != data.windowed_rect.size)
//                {
//                    post_window_resized(data.windowed_rect.size.x, data.windowed_rect.size.y);
//                }
//                else
//                {
//                    on_window_resized();
//                }
//            }
//
//            if (d && !(has_capabilities(capabilities::DISABLE_FULLSCREEN_MOUSE_WARP)))
//            {
//                // restore cursor position
//            }
//        }
//    }
//
//    done:
//    {
//        if (d && (data.flags & window_flags::FULLSCREEN) && data.fullscreen_exclusive)
//        {
//            data.last_fullscreen_exclusive_display_id = video_internal::get_display_id(*d);
//        }
//        else
//        {
//            data.last_fullscreen_exclusive_display_id = 0;
//        }
//
//        return true;
//    }
//
//    error:
//    {
//        if (fullscreen)
//        {
//            // Something went wrong, exit fullscreen
//            update_fullscreen_mode(fullscreen_op::LEAVE, commit);
//        }
//
//        return false;
//    }
//}
//
//bool window_instance::is_fullscreen() const
//{
//    return data.flags & window_flags::FULLSCREEN;
//}
//
//// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L3448
//
//bool window_instance::set_fullscreen(bool fullscreen)
//{
//    if (data.flags & window_flags::HIDDEN)
//    {
//        if (fullscreen)
//        {
//            data.pending_flags |= window_flags::FULLSCREEN;
//        }
//        else
//        {
//            data.pending_flags &= ~window_flags::FULLSCREEN;
//        }
//
//        return true;
//    }
//
//    if (update_fullscreen_mode(fullscreen ? fullscreen_op::ENTER : fullscreen_op::LEAVE, true))
//    {
//        sync();
//        return true;
//    }
//
//    return false;
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// icon
/////////////////////////////////////////////////////////////////////////////////
//
//bool window_instance::set_icon(const pixel::surface_rgba8& surf)
//{
//    if (surf.empty())
//    {
//        return false;
//    }
//
//    return impl_ptr->set_icon(surf);
//}
//
//void window_instance::clear_icon()
//{
//    impl_ptr->clear_icon();
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// window events
/////////////////////////////////////////////////////////////////////////////////
//
//static bool filter_duplicate_window_events(const event::event& e, void* user_data)
//{
//    const event::event* ne = reinterpret_cast<const event::event*>(user_data);
//
//    if (e.type == ne->type)
//    {
//        return e.window_id == ne->window_id;
//    }
//
//    return false;
//}
//
//bool window_instance::post_window_shown()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (!(data.flags & window_flags::HIDDEN))
//    {
//        return false;
//    }
//
//    data.flags &= ~(window_flags::HIDDEN | window_flags::MINIMIZED);
//
//    event::event e{};
//    e.type = event::WINDOW_SHOWN;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    on_window_shown();
//    return posted;
//}
//
//void window_instance::on_window_shown()
//{
//    apply_flags(data.pending_flags);
//    data.pending_flags = window_instance::window_flags::NONE;
//}
//
//bool window_instance::post_window_hidden()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (data.flags & window_instance::window_flags::HIDDEN)
//    {
//        return false;
//    }
//
//    data.flags |= window_instance::window_flags::HIDDEN;
//
//    event::event e{};
//    e.type = event::WINDOW_HIDDEN;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    on_window_hidden();
//    return posted;
//}
//
//void window_instance::on_window_hidden()
//{
//    // We will restore the state of these flags when the window is shown again
//    data.pending_flags |= (data.flags & (window_instance::window_flags::FULLSCREEN | window_instance::window_flags::MAXIMIZED));
//    update_fullscreen_mode(fullscreen_op::LEAVE, false);
//}
//
//// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_windowevents.c#L99
//
//bool window_instance::post_window_moved(int32_t x, int32_t y)
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    data.last_position_pending = false;
//
//    if (!(data.flags & window_flags::FULLSCREEN))
//    {
//        data.windowed_rect.position.x = x;
//        data.windowed_rect.position.y = y;
//
//        if (!(data.flags & window_flags::MAXIMIZED) && !data.tiled)
//        {
//            data.floating_rect.position.x = x;
//            data.floating_rect.position.y = y;
//        }
//    }
//
//    if (data.position.x == x && data.position.y == y)
//    {
//        return false;
//    }
//
//    data.position.x = x;
//    data.position.y = y;
//
//    event::flush_events(event::WINDOW_MOVED);
//
//    event::event e{};
//    e.type = event::WINDOW_MOVED;
//    e.window_id = data.id;
//    e.window_moved.x = x;
//    e.window_moved.y = y;
//
//    event::filter_events(filter_duplicate_window_events, static_cast<void*>(&e));
//    bool posted = event::post_event(e);
//
//    on_window_moved();
//    return posted;
//}
//
//void window_instance::on_window_moved()
//{
//    video_internal::check_window_display_changed(*this);
//}
//
//bool window_instance::post_window_resized(int32_t w, int32_t h)
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    data.last_size_pending = false;
//
//    if (!(data.flags & window_flags::FULLSCREEN))
//    {
//        data.windowed_rect.size.x = w;
//        data.windowed_rect.size.y = h;
//
//        if (!(data.flags & window_flags::MAXIMIZED) && !data.tiled)
//        {
//            data.floating_rect.size.x = w;
//            data.floating_rect.size.y = h;
//        }
//    }
//
//    if (data.size.x == w && data.size.y == h)
//    {
//        return false;
//    }
//
//    data.size.x = w;
//    data.size.y = h;
//
//    event::event e{};
//    e.type = event::WINDOW_RESIZED;
//    e.window_id = data.id;
//    e.window_resized.w = w;
//    e.window_resized.h = h;
//
//    event::filter_events(filter_duplicate_window_events, static_cast<void*>(&e));
//    bool posted = event::post_event(e);
//
//    on_window_resized();
//    return posted;
//}
//
//void window_instance::on_window_resized()
//{
//    video_internal::check_window_display_changed(*this);
//
//    // update window shape & safe area
//}
//
//bool window_instance::post_window_minimized()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (data.flags & window_flags::MINIMIZED)
//    {
//        return false;
//    }
//
//    data.flags &= ~window_flags::MAXIMIZED;
//    data.flags |= window_flags::MINIMIZED;
//
//    event::event e{};
//    e.type = event::WINDOW_MINIMIZED;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    on_window_minimized();
//    return posted;
//}
//
//void window_instance::on_window_minimized()
//{
//    if (data.flags & window_flags::FULLSCREEN)
//    {
//        update_fullscreen_mode(fullscreen_op::LEAVE, false);
//    }
//}
//
//bool window_instance::post_window_maximized()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (data.flags & window_flags::MAXIMIZED)
//    {
//        return false;
//    }
//
//    data.flags &= ~window_flags::MINIMIZED;
//    data.flags |= window_flags::MAXIMIZED;
//
//    event::event e{};
//    e.type = event::WINDOW_MAXIMIZED;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    on_window_maximized();
//    return posted;
//}
//
//void window_instance::on_window_maximized()
//{
//}
//
//bool window_instance::post_window_restored()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (!(data.flags & (window_flags::MINIMIZED | window_flags::MAXIMIZED)))
//    {
//        return false;
//    }
//
//    data.flags &= ~(window_flags::MINIMIZED | window_flags::MAXIMIZED);
//
//    event::event e{};
//    e.type = event::WINDOW_RESTORED;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    on_window_restored();
//    return posted;
//}
//
//void window_instance::on_window_restored()
//{
//    if (data.flags & window_flags::FULLSCREEN)
//    {
//        update_fullscreen_mode(fullscreen_op::ENTER, true);
//    }
//}
//
//bool window_instance::post_window_enter_fullscreen()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (data.flags & window_instance::window_flags::FULLSCREEN)
//    {
//        return false;
//    }
//
//    data.flags |= window_instance::window_flags::FULLSCREEN;
//
//    event::event e{};
//    e.type = event::WINDOW_ENTER_FULLSCREEN;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    return posted;
//}
//
//bool window_instance::post_window_leave_fullscreen()
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (!(data.flags & window_instance::window_flags::FULLSCREEN))
//    {
//        return false;
//    }
//
//    data.flags &= ~window_instance::window_flags::FULLSCREEN;
//
//    event::event e{};
//    e.type = event::WINDOW_LEAVE_FULLSCREEN;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    return posted;
//}
//
//bool window_instance::post_window_display_changed(const display& d)
//{
//    if (data.destroying)
//    {
//        return false;
//    }
//
//    if (data.last_display_id == d.id())
//    {
//        return false;
//    }
//
//    data.last_display_id = d.id();
//
//    event::event e{};
//    e.type = event::WINDOW_DISPLAY_CHANGED;
//    e.window_id = data.id;
//    e.window_display_changed.display_id = d.id();
//    bool posted = event::post_event(e);
//
//    on_window_display_changed(d);
//    return posted;
//}
//
//void window_instance::on_window_display_changed(const display& d)
//{
//    if (is_fullscreen())
//    {
//        const display_mode* new_mode = nullptr;
//
//        if (data.requested_fullscreen_mode_set)
//        {
//            new_mode = d.find_closest_mode(
//                data.requested_fullscreen_mode.resolution.x,
//                data.requested_fullscreen_mode.resolution.y,
//                data.requested_fullscreen_mode.refresh_rate
//            );
//        }
//
//        if (new_mode && IS_FULLSCREEN_VISIBLE(this))
//        {
//            update_fullscreen_mode(fullscreen_op::UPDATE, true);
//        }
//    }
//}
//
//bool window_instance::post_window_close_requested()
//{
//    event::event e{};
//    e.type = event::WINDOW_CLOSE_REQUESTED;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    return posted;
//}
//
//bool window_instance::post_window_destroyed()
//{
//    event::event e{};
//    e.type = event::WINDOW_DESTROYED;
//    e.window_id = data.id;
//    bool posted = event::post_event(e);
//
//    return posted;
//}

} // namespace video
} // namespace app
} // namespace vx