#pragma once

#include <unordered_map>

#include "vertex/app/input/keyboard.hpp"
#include "vertex/util/bit_field.hpp"
#include "vertex/math/rect.hpp"

namespace vx {
namespace app {
namespace keyboard {

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

    void set_entry(scancode sc, key_mod mod_state, keycode kc);
    keycode get_keycode(scancode sc, key_mod mod_state) const;
    scancode get_scancode(keycode kc, key_mod mod_state) const;
    scancode get_next_reserved_scancode();

    bool french_numbers() const noexcept { return m_french_numbers; }
    bool thai_keyboard() const noexcept { return m_thai_keyboard; }
    bool latin_letters() const noexcept { return m_latin_letters; }

private:

    friend keyboard_internal;

    std::unordered_map<scancode, keycode> m_scancode_to_keycode;
    std::unordered_map<keycode, scancode> m_keycode_to_scancode;
    scancode m_next_reserved_scancode = scancode::SCANCODE_COUNT;
    bool m_french_numbers;
    bool m_thai_keyboard;
    bool m_latin_letters;
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
    keyboard_id id;
    const char* name;
};

struct keyboard_data
{
    video::window* focus;
    key_mod mod_state;
    bit_field<scancode::SCANCODE_COUNT, bool> key_state;
    keymap map;
    bool map_configures;
    bool autorelease_pending;
};

///////////////////////////////////////////////////////////////////////////////
// keyboard internal
///////////////////////////////////////////////////////////////////////////////

class keyboard_internal
{
public:

    static bool init();
    static void quit();

    //-------------------------------------------------------------------------
    // Device Management
    //-------------------------------------------------------------------------

    static void add_keyboard(keyboard_id id, const char* name, bool send_event);
    static void remove_keyboard(keyboard_id id, bool send_event);

    static bool is_keyboard(uint16_t vendor, uint16_t product, size_t key_count);
    static bool has_keyboard();

    static std::vector<keyboard_id> list_keyboard_ids();
    static const char* get_keyboard_name(keyboard_id id);

    //-------------------------------------------------------------------------
    // Focus / Window Association
    //-------------------------------------------------------------------------

    static video::window* get_focus();
    static bool set_focus(const video::window& w);

    //-------------------------------------------------------------------------
    // Keymap / Scancode <-> Keycode mapping
    //-------------------------------------------------------------------------

    static void set_keymap(const keymap& map, bool send_event);

    static keycode get_key_from_scancode(scancode sc);
    static scancode get_scancode_from_key(keycode key);

    static bool set_scancode_name(scancode sc, const char* name);
    static const char* get_scancode_name(scancode sc);
    static scancode get_scancode_from_name(const char* name);

    static const char* get_key_name(keycode key);
    static keycode get_key_from_name(const char* name);

    //-------------------------------------------------------------------------
    // Key Events
    //-------------------------------------------------------------------------

    static void send_unicode_key(char32_t c);

    static bool send_key(keyboard_id id, int raw, scancode sc, bool down);
    static bool send_key_no_mods(keyboard_id id, int raw, scancode sc, bool down);
    static bool send_key_and_scancode(keyboard_id id, int raw, scancode sc, keycode key, bool down);

    static bool send_key_auto_release(scancode sc);
    static void release_auto_release_keys();

    static bool hardware_key_pressed();

    //-------------------------------------------------------------------------
    // Key State / Modifiers
    //-------------------------------------------------------------------------

    static std::vector<bool> get_keyboard_state();
    static void reset_keyboard();

    static key_mod get_mod_state();
    static void set_mod_state(key_mod mod_state);
    static void toggle_mod_state(key_mod state, bool toggle);

    //-------------------------------------------------------------------------
    // Text Input / IME
    //-------------------------------------------------------------------------

    static bool start_text_input(video::window* w);
    static bool start_text_input(video::window* w, text_input_options& options);
    static bool text_input_active(const video::window* w);
    static bool stop_text_input(video::window* w);

    static bool clear_composition(video::window* w);

    static bool set_text_input_area(video::window* w, const math::recti& rect, int cursor_offset);
    static bool get_text_input_area(const video::window* w, math::recti* rect, int* cursor_offset);

    static bool has_screen_keyboard_support();
    static bool screen_keyboard_shown();

    static void send_text(const char* text);
    static void send_editing_text(const char* text, size_t start, size_t size);
    static void send_editing_text_candidates(const std::vector<const char*>& candidates, size_t selected_candidate, bool horizontal);
};

} // namespace keyboard
} // namespace app
} // namespace vx