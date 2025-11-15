#pragma once

#include "vertex/app/input/keyboard.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/math/rect.hpp"
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

VX_FLAGS_UT_DECLARE_BEGIN(key_source, uint8_t)
{
    hardware        = 0,
    auto_release    = 1
}
VX_FLAGS_DECLARE_END(key_source)

//=============================================================================

struct keyboard_info
{
    keyboard_id id;
    std::string name;
};

//=============================================================================

struct keyboard_data
{
    video::window_id focus;

    key_mod mod_state;
    key_state key_source;
    key_state key_state;
    keymap* keymap_ptr;

    keycode_options keycode_options;
    bool auto_release_pending;
    time::time_point hardware_timestamp;

    std::vector<keyboard_info> keyboards;

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

    //=============================================================================
    // devices
    //=============================================================================

    size_t get_keyboard_index(keyboard_id id) const;
    const keyboard_info* get_keyboard(keyboard_id id) const;
    keyboard_info* get_keyboard(keyboard_id id);

    void add_keyboard(keyboard_id id, const char* name);
    void remove_keyboard(keyboard_id id);
    void clear_keyboards();

    static bool is_keyboard(uint16_t vendor, uint16_t product, size_t key_count);
    bool any_connected() const;

    std::vector<keyboard_id> list_keyboards() const;
    const char* get_name(keyboard_id id) const;

    //=============================================================================
    // focus
    //=============================================================================

    video::window_id get_focus() const;
    video::window_instance* get_focus_instance();

    bool set_focus(video::window_id wid);

    //=============================================================================
    // keymap
    //=============================================================================

    void clear_keymap();
    void set_keymap(keymap* map, bool send_event);

    keymap* get_current_keymap(bool ignore_options);
    const keymap* get_current_keymap(bool ignore_options) const;

    keycode get_key_from_scancode(scancode sc, key_mod mod_state, bool key_event) const;
    scancode get_scancode_from_key(keycode key, key_mod* mod_state) const;

    bool set_scancode_name(scancode sc, const char* name);
    const char* get_scancode_name(scancode sc) const;
    scancode get_scancode_from_name(const char* name) const;

    std::string get_key_name(keycode key) const;
    keycode get_key_from_name(const char* name) const;

    //=============================================================================
    // Key State / Modifiers
    //=============================================================================

    const key_state& get_key_state() const;
    void reset();

    key_mod get_mod_state() const;
    void set_mod_state(key_mod mod_state);
    void toggle_mod_state(key_mod mod_state, bool toggle);

    //=============================================================================
    // Key Events
    //=============================================================================

    void send_keymap_changed();

    void send_keyboard_added(keyboard_id id);
    void send_keyboard_removed(keyboard_id id);

    bool send_key(time::time_point t, keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_internal(time::time_point t, key_source source, keyboard_id id, int raw, scancode sc, bool down);

    bool send_key_no_mods(time::time_point t, keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_and_scancode(time::time_point t, keyboard_id id, int raw, scancode sc, keycode key, bool down);

    void send_unicode_key(char32_t c);

    bool send_key_auto_release(scancode sc);
    void release_auto_release_keys() {}

    bool hardware_key_pressed();

    //=============================================================================
    // Text Input / IME
    //=============================================================================

    bool auto_showing_screen_keyboard() const;
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