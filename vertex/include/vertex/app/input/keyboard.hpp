#pragma once

#include "vertex/app/input/scancode.hpp"
#include "vertex/app/input/keycode.hpp"

namespace vx {
namespace app {
namespace keyboard {

///////////////////////////////////////////////////////////////////////////////
// keyboard
///////////////////////////////////////////////////////////////////////////////

namespace _priv { class keyboard_internal; }
using keyboard_id = size_t;

///////////////////////////////////////////////////////////////////////////////
// keyboard
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_keyboard();
VX_API std::vector<keyboard_id> list_keyboard_ids();
VX_API const char* get_keyboard_name(keyboard_id id);
VX_API video::window* get_keyboard_focus();

VX_API std::vector<bool> get_keyboard_state();
VX_API void reset_keyboard();

VX_API key_mod get_mod_state();
VX_API void set_mod_state(key_mod mod_state);

VX_API keycode get_key_from_scancode(scancode sc);
VX_API scancode get_scancode_from_key(keycode key);

VX_API bool set_scancode_name(scancode sc, const char* name);
VX_API const char* get_scancode_name(scancode sc);
VX_API scancode get_scancode_from_name(const char* name);

VX_API const char* get_key_name(keycode key);
VX_API keycode get_key_from_name(const char* name);

///////////////////////////////////////////////////////////////////////////////
// text input
///////////////////////////////////////////////////////////////////////////////

enum class text_input_type
{
    TEXT,
    TEXT_NAME,
    TEXT_EMAIL,
    TEXT_USERNAME,
    TEXT_PASSWORD_HIDDEN,
    TEXT_PASSWORD_VISIBLE,

    NUMBER,
    NUMBER_PASSWORD_HIDDEN,
    NUMBER_PASSWORD_VISIBLE
};

enum class text_input_capitalization
{
    NONE,
    SENTENCES,
    WORDS,
    LETTERS
};

struct text_input_options
{
    text_input_type type;
    text_input_capitalization capitalization;
    bool auto_correct;
    bool multiline;
};

VX_API bool start_text_input(video::window* w);
VX_API bool start_text_input(video::window* w, text_input_options& options);
VX_API bool text_input_active(const video::window* w);
VX_API bool stop_text_input(video::window* w);

VX_API bool clear_composition(video::window* w);

VX_API bool set_text_input_area(video::window* w, const math::recti& rect, int cursor_offset);
VX_API bool get_text_input_area(const video::window* w, math::recti* rect, int* cursor_offset);

VX_API bool has_screen_keyboard_support();
VX_API bool screen_keyboard_shown();

} // namespace keyboard
} // namespace app
} // namespace vx