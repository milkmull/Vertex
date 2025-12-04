#include "vertex_impl/app/message_box/_platform/platform_message_box.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex/os/atomic.hpp"

namespace vx {
namespace app {

#if VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
static os::atomic<size_t> s_message_box_count = 0;
#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX

size_t message_box_count()
{
#if VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
    return s_message_box_count.load();
#else
    return 0;
#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
}

//=============================================================================

bool show_message_box(
    const message_box::config& config,
    message_box::button_id* button
)
{
    return show_message_box_internal(s_app.get(), config, button);
}

bool show_message_box_internal(
    app_instance* this_,
    const message_box::config& config,
    message_box::button_id* button
)
{
#if !VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX

    VX_UNUSED(this_);
    VX_UNUSED(config);
    VX_UNUSED(button);
    VX_UNSUPPORTED("show_message_box()");
    return false;

#else

    ++s_message_box_count;

#if defined(VX_APP_VIDEO_ENABLED)

    video::video_instance* video = this_ ? this_->data.video_ptr.get() : nullptr;
    video::window_instance* current_window = nullptr;
    bool show_cursor_prev = false;

    if (video)
    {
        current_window = video->data.keyboard_ptr->get_focus_instance();
        video->data.mouse_ptr->update_capture(false);
        video->data.mouse_ptr->set_relative_mode(false);
        show_cursor_prev = video->data.mouse_ptr->cursor_visible();
        video->data.mouse_ptr->show_cursor();
        video->data.keyboard_ptr->reset();
    }

#endif // VX_APP_VIDEO_ENABLED

    message_box::button_id dummy_button{};
    if (!button)
    {
        button = &dummy_button;
    }

    err::clear();
    const bool result = false;// show_message_box_impl(this_, config, button);
    err::clear();

    --s_message_box_count;

#if defined(VX_APP_VIDEO_ENABLED)

    if (video)
    {
        if (current_window)
        {
            current_window->raise();
        }

        if (!show_cursor_prev)
        {
            video->data.mouse_ptr->hide_cursor();
        }

        video->data.mouse_ptr->update_relative_mode();
        video->data.mouse_ptr->update_capture(false);
    }

#endif // VX_APP_VIDEO_ENABLED

    return result;

#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
}

//=============================================================================

bool show_simple_message_box(
    message_box::type type,
    const std::string& title,
    const std::string& message,
    id_type parent_window
)
{
    return show_simple_message_box_internal(
        s_app.get(),
        type, title, message, parent_window
    );
}

bool show_simple_message_box_internal(
    app_instance* this_,
    message_box::type type,
    const std::string& title,
    const std::string& message,
    id_type parent_window
)
{
#if !VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX

    VX_UNUSED(this_);
    VX_UNUSED(type);
    VX_UNUSED(title);
    VX_UNUSED(message);
    VX_UNUSED(parent_window);
    VX_UNSUPPORTED("show_simple_message_box()");
    return false;

#else

    message_box::config config;
    config.message_type = type;
    config.layout = message_box::button_layout::left_to_right;
    config.title = title;
    config.message = message;
    config.buttons.emplace_back(
        message_box::button_config
        {
            message_box::default_button_keys::any,
            0,
            "OK"
        }
    );
    config.parent_window = parent_window;

    return show_message_box_internal(this_, config, nullptr);

#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
}

} // namespace app
} // namespace vx