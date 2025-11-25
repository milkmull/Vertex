#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex_impl/app/video/_platform/platform_video.hpp"

namespace vx {
namespace app {
namespace video {

//=============================================================================
// count
//=============================================================================

size_t video_instance::message_box_count()
{
    return video_data::message_box_count.load();
}

//=============================================================================
// show_message_box
//=============================================================================

bool video_instance::show_message_box(video_instance* this_, const message_box::config& config, message_box::button_id* button)
{
#if !VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX

    VX_UNUSED(this_);
    VX_UNUSED(config);
    VX_UNUSED(button);
    VX_UNSUPPORTED("show_message_box()");
    return false;

#else

    ++video_data::message_box_count;

    video::window_instance* current_window = nullptr;
    bool show_cursor_prev = false;

    if (this_)
    {
        current_window = this_->data.keyboard_ptr->get_focus_instance();
        this_->data.mouse_ptr->update_capture(false);
        this_->data.mouse_ptr->set_relative_mode(false);
        show_cursor_prev = this_->data.mouse_ptr->cursor_visible();
        this_->data.mouse_ptr->show_cursor();
        this_->data.keyboard_ptr->reset();
    }

    message_box::button_id dummy_button{};
    if (!button)
    {
        button = &dummy_button;
    }

    err::clear();
    const bool result = false;// show_message_box_impl(this_, config, button);
    err::clear();

    --video_data::message_box_count;

    if (this_)
    {
        if (current_window)
        {
            current_window->raise();
        }

        if (!show_cursor_prev)
        {
            this_->data.mouse_ptr->hide_cursor();
        }

        this_->data.mouse_ptr->update_relative_mode();
        this_->data.mouse_ptr->update_capture(false);
    }

    return result;

#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
}

//=============================================================================
// show_simple_message_box
//=============================================================================

bool video_instance::show_simple_message_box(video_instance* this_, message_box::type type, const std::string& title, const std::string& message, window_id w)
{
#if !VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX

    VX_UNUSED(this_);
    VX_UNUSED(type);
    VX_UNUSED(title);
    VX_UNUSED(message);
    VX_UNUSED(w);
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
    config.parent_window = w;

    return show_message_box(this_, config, nullptr);

#endif // VX_VIDEO_BACKEND_HAVE_SHOW_MESSAGE_BOX
}

} // namespace video
} // namespace app
} // namespace vx