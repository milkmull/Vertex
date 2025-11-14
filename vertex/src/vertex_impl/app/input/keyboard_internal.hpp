#pragma once

#include "vertex/app/input/keyboard.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/util/bit_field.hpp"
#include "vertex_impl/app/input/keymap.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }
namespace video { class window_instance; }

namespace keyboard {

class keyboard_instance;

//=============================================================================
// keyboard data
//=============================================================================

enum class key_source
{
    hardware,
    autorelease
};

//=============================================================================

struct input_source_data
{
    keyboard_id id;
    const char* name;
};

class input_source
{
public:

    input_source_data data{};
};

//=============================================================================

struct keyboard_data
{
    video::window_id focus;

    key_mod mod_state;
    bit_field<scancode_count, bool> key_state;

    std::unique_ptr<keymap> key_map;
    bool key_map_configured;

    bool auto_release_pending;
    keycode_options keycode_options;

    bool quitting;
};

//=============================================================================
// keyboard internal
//=============================================================================

class keyboard_instance
{
public:

    keyboard_instance();
    ~keyboard_instance();

    keyboard_instance(const keyboard_instance&) = delete;
    keyboard_instance& operator=(const keyboard_instance&) = delete;

    keyboard_instance(keyboard_instance&&) noexcept = delete;
    keyboard_instance& operator=(keyboard_instance&&) noexcept = delete;

public:

    //=============================================================================
    // Initialization
    //=============================================================================

    void init_data();

    bool init(video::video_instance* owner);
    void quit();

    bool init_impl() {}
    void quit_impl() {}

    //=============================================================================
    // devices
    //=============================================================================

    void add_keyboard(keyboard_id id, const char* name, bool send_event) {}
    void remove_keyboard(keyboard_id id, bool send_event) {}
    void clear_keyboards() {}

    bool is_keyboard(uint16_t vendor, uint16_t product, size_t key_count) { return false; }
    bool has_keyboard() { return false; }

    std::vector<keyboard_id> list_keyboard_ids() { return {}; }
    const char* get_keyboard_name(keyboard_id id) { return nullptr; }

    //=============================================================================
    // Focus / Window Association
    //=============================================================================

    video::window_id get_focus() { return invalid_id; }
    video::window_instance* get_focus_instance() { return nullptr; }
    bool set_focus(video::window_id id) { return false; }

    //=============================================================================
    // keymap
    //=============================================================================

    void set_keymap(const keymap& map, bool send_event);

    keycode get_key_from_scancode(scancode sc);
    scancode get_scancode_from_key(keycode key);

    bool set_scancode_name(scancode sc, const char* name);
    const char* get_scancode_name(scancode sc);
    scancode get_scancode_from_name(const char* name);

    const char* get_key_name(keycode key);
    keycode get_key_from_name(const char* name);

    //=============================================================================
    // Key Events
    //=============================================================================

    void send_unicode_key(char32_t c);

    bool send_key(keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_no_mods(keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_and_scancode(keyboard_id id, int raw, scancode sc, keycode key, bool down);

    bool send_key_auto_release(scancode sc);
    void release_auto_release_keys() {}

    bool hardware_key_pressed();

    //=============================================================================
    // Key State / Modifiers
    //=============================================================================

    const bit_field<scancode_count, bool>& get_keyboard_state();
    void reset_keyboard();

    key_mod get_mod_state();
    void set_mod_state(key_mod mod_state);
    void toggle_mod_state(key_mod state, bool toggle);

    //=============================================================================
    // Text Input / IME
    //=============================================================================

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

    //=============================================================================
    // Data
    //=============================================================================

    video::video_instance* video = nullptr;
    keyboard_data data;
};

} // namespace keyboard
} // namespace app
} // namespace vx