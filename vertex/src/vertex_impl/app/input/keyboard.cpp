#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace keyboard {

//=============================================================================
// hints
//=============================================================================

static void keycode_options_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    keyboard_instance* keyboard = static_cast<keyboard_instance*>(user_data);

    if (new_value && *new_value)
    {
        constexpr int64_t default_options = static_cast<int64_t>(keycode_options::default_options);
        const int64_t hint = hint::parse_integer(new_value, default_options);
        keyboard->data.keycode_options = static_cast<keycode_options>(hint);
    }
    else
    {
        keyboard->data.keycode_options = keycode_options::default_options;
    }
}

//=============================================================================
// keyboard internal
//=============================================================================

keyboard_instance::keyboard_instance() = default;

keyboard_instance::~keyboard_instance()
{
    quit();
}

//=============================================================================
// Initialization
//=============================================================================

void keyboard_instance::init_data()
{
    data.focus = invalid_id;
    data.mod_state = key_mod::none;
    data.auto_release_pending = false;
    data.keycode_options = keycode_options::none;
    data.quitting = false;
}

//=============================================================================

bool keyboard_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    init_data();

    // hints
    {
        video->app->data.hints_ptr->add_hint_callback(
            hint::keyboard_keycode_options,
            keycode_options_hint_watcher,
            this
        );
    }

    return true;
}

//=============================================================================

void keyboard_instance::quit()
{
    data.quitting = true;

    clear_keyboards();
    data.key_map.reset();

    quit_impl();

    // hints
    {
        video->app->data.hints_ptr->remove_hint_callback(
            hint::keyboard_keycode_options,
            keycode_options_hint_watcher,
            this
        );
    }

    data.quitting = false;
}

//=============================================================================
// keymap
//=============================================================================

} // namespace keyboard
} // namespace app
} // namespace vx