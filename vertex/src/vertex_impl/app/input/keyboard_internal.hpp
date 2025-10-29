#pragma once

#include <unordered_map>

#include "vertex/app/video/video.hpp"
#include "vertex/app/input/keyboard.hpp"
#include "vertex/util/bit_field.hpp"
#include "vertex/math/rect.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace keyboard {

class keyboard_instance;

///////////////////////////////////////////////////////////////////////////////
// keycode options
///////////////////////////////////////////////////////////////////////////////

enum keycode_options
{
    KEYCODE_OPTION_NONE             = 0,
    KEYCODE_OPTION_HIDE_NUMPAD      = (1 << 0),
    KEYCODE_OPTION_FRENCH_NUMBERS   = (1 << 1),
    KEYCODE_OPTION_LATIN_LETTERS    = (1 << 2),
    KEYCODE_OPTION_DEFAULT_OPTIONS  = (KEYCODE_OPTION_FRENCH_NUMBERS | KEYCODE_OPTION_LATIN_LETTERS),
};

///////////////////////////////////////////////////////////////////////////////
// keymap
///////////////////////////////////////////////////////////////////////////////

class keymap
{
public:

    keymap() = default;
    ~keymap() = default;

    keymap(const keymap&) = default;
    keymap& operator=(const keymap&) = default;

    keymap(keymap&&) noexcept = default;
    keymap& operator=(keymap&&) noexcept = default;

    void set_entry(scancode sc, key_mod mod_state, keycode kc) {}
    keycode get_keycode(scancode sc, key_mod mod_state) const { return (keycode)0; }
    scancode get_scancode(keycode kc, key_mod mod_state) const { return (scancode)0; }
    scancode get_next_reserved_scancode() { return (scancode)0; }

    bool french_numbers() const noexcept { return m_french_numbers; }
    bool thai_keyboard() const noexcept { return m_thai_keyboard; }
    bool latin_letters() const noexcept { return m_latin_letters; }

private:

    friend keyboard_instance;

    std::unordered_map<scancode, keycode> m_scancode_to_keycode;
    std::unordered_map<keycode, scancode> m_keycode_to_scancode;
    scancode m_next_reserved_scancode = scancode::SCANCODE_COUNT;
    bool m_french_numbers = false;
    bool m_thai_keyboard = false;
    bool m_latin_letters = false;
};

///////////////////////////////////////////////////////////////////////////////
// keyboard data
///////////////////////////////////////////////////////////////////////////////

enum key_source : uint8_t
{
    KEY_SOURCE_HARDWARE,
    KEY_SOURCE_AUTORELEASE
};

struct input_source
{
    keyboard_id id = INVALID_ID;
    const char* name = nullptr;
};

struct keyboard_data
{
    video::window* focus = nullptr;
    key_mod mod_state = MOD_NONE;
    bit_field<scancode::SCANCODE_COUNT, bool> key_state;
    keymap map;
    bool map_configures = false;
    bool autorelease_pending = false;
    keycode_options keycode_options_cache = keycode_options::KEYCODE_OPTION_DEFAULT_OPTIONS;
};

///////////////////////////////////////////////////////////////////////////////
// keyboard internal
///////////////////////////////////////////////////////////////////////////////

class keyboard_instance
{
public:

    //-------------------------------------------------------------------------
    // Initialization
    //-------------------------------------------------------------------------

    bool init(video::video_instance* owner);
    void quit();

    //-------------------------------------------------------------------------
    // Device Management
    //-------------------------------------------------------------------------

    void add_keyboard(keyboard_id id, const char* name, bool send_event) {}
    void remove_keyboard(keyboard_id id, bool send_event) {}

    bool is_keyboard(uint16_t vendor, uint16_t product, size_t key_count) { return false; }
    bool has_keyboard() { return false; }

    std::vector<keyboard_id> list_keyboard_ids() { return {}; }
    const char* get_keyboard_name(keyboard_id id) { return nullptr; }

    //-------------------------------------------------------------------------
    // Focus / Window Association
    //-------------------------------------------------------------------------

    video::window_id get_focus() { return INVALID_ID; }
    bool set_focus(video::window_id id) { return false; }

    //-------------------------------------------------------------------------
    // Keymap / Scancode <-> Keycode mapping
    //-------------------------------------------------------------------------

    void set_keymap(const keymap& map, bool send_event);

    keycode get_key_from_scancode(scancode sc);
    scancode get_scancode_from_key(keycode key);

    bool set_scancode_name(scancode sc, const char* name);
    const char* get_scancode_name(scancode sc);
    scancode get_scancode_from_name(const char* name);

    const char* get_key_name(keycode key);
    keycode get_key_from_name(const char* name);

    //-------------------------------------------------------------------------
    // Key Events
    //-------------------------------------------------------------------------

    void send_unicode_key(char32_t c);

    bool send_key(keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_no_mods(keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_and_scancode(keyboard_id id, int raw, scancode sc, keycode key, bool down);

    bool send_key_auto_release(scancode sc);
    void release_auto_release_keys() {}

    bool hardware_key_pressed();

    //-------------------------------------------------------------------------
    // Key State / Modifiers
    //-------------------------------------------------------------------------

    const bit_field<scancode::SCANCODE_COUNT, bool>& get_keyboard_state();
    void reset_keyboard();

    key_mod get_mod_state();
    void set_mod_state(key_mod mod_state);
    void toggle_mod_state(key_mod state, bool toggle);

    //-------------------------------------------------------------------------
    // Text Input / IME
    //-------------------------------------------------------------------------

    bool start_text_input(video::window* w);
    bool start_text_input(video::window* w, text_input_options& options);
    bool text_input_active(const video::window* w);
    bool stop_text_input(video::window* w);

    bool clear_composition(video::window* w);

    bool set_text_input_area(video::window* w, const math::recti& rect, int cursor_offset);
    bool get_text_input_area(const video::window* w, math::recti* rect, int* cursor_offset);

    bool has_screen_keyboard_support();
    bool screen_keyboard_shown();

    void send_text(const char* text);
    void send_editing_text(const char* text, size_t start, size_t size);
    void send_editing_text_candidates(const std::vector<const char*>& candidates, size_t selected_candidate, bool horizontal);

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    video::video_instance* video = nullptr;
    keyboard_data data;
};

} // namespace keyboard
} // namespace app
} // namespace vx