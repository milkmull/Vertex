#pragma once

#include "vertex/app/input/keyboard.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/util/time/time.hpp"
#include "vertex_impl/app/input/keymap.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }
namespace video { class window_instance; }

namespace keyboard {

//=============================================================================
// keyboard data
//=============================================================================

VX_FLAGS_UT_DECLARE_BEGIN(key_flags, uint8_t)
{
    none                = 0,
    hardware            = VX_BIT(0),
    auto_release        = VX_BIT(1),
    source_mask         = (hardware | auto_release),
    virtual_            = VX_BIT(2),
    ignore_modifiers    = VX_BIT(3)
}
VX_FLAGS_DECLARE_END(key_flags)

//=============================================================================

struct keyboard_info
{
    keyboard_id id;
    std::string name;
};

//=============================================================================

struct keyboard_manager_data
{
    video::window_id focus = invalid_id;

    key_mod mod_state = key_mod::none;
    key_flags key_source[scancode_count] = {};
    key_state state;
    keymap* keymap_ptr = nullptr;

    keycode_options options = keycode_options::none;
    bool auto_release_pending = false;
    time::time_point hardware_timestamp;

    std::vector<keyboard_info> keyboards;

    bool screen_keyboard_shown = false;
};

//=============================================================================
// keyboard instance
//=============================================================================

class keyboard_manager
{
public:

    keyboard_manager();
    ~keyboard_manager();

    keyboard_manager(const keyboard_manager&) = delete;
    keyboard_manager& operator=(const keyboard_manager&) = delete;

    keyboard_manager(keyboard_manager&&) noexcept = delete;
    keyboard_manager& operator=(keyboard_manager&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

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
    bool create_keymap();

    keymap* get_current_keymap(bool ignore_options);
    const keymap* get_current_keymap(bool ignore_options) const;

    void set_keymap_entry(scancode sc, key_mod mod_state, keycode kc);
    keycode get_key_from_scancode(scancode sc, key_mod mod_state, bool key_event) const;
    scancode get_scancode_from_key(keycode key, key_mod* mod_state) const;
    scancode get_next_reserved_scancode();

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
    bool send_key_no_mods(time::time_point t, keyboard_id id, int raw, scancode sc, bool down);
    bool send_key_and_keycode(time::time_point t, keyboard_id id, int raw, scancode sc, keycode kc, bool down);
    bool send_key_auto_release(time::time_point t, scancode sc);
    void send_unicode_key(time::time_point t, char32_t c);

    bool send_key_internal(time::time_point t, key_flags flags, keyboard_id id, int raw, scancode sc, bool down);

    bool hardware_key_pressed() const;
    void release_auto_release_keys();

    //=============================================================================
    // Screen Keyboard
    //=============================================================================

    bool auto_showing_screen_keyboard() const;
    bool has_screen_keyboard_support() const;
    bool screen_keyboard_shown() const;

    void send_screen_keyboard_shown();
    void send_screen_keyboard_hidden();

    //=============================================================================
    // Text Input / IME
    //=============================================================================

    void send_text(const char* text);
    void send_editing_text(const char* text, size_t start, size_t length);
    void send_editing_text_candidates(char** candidates, size_t count, size_t selected, bool horizontal);

    //=============================================================================
    // Data
    //=============================================================================

    video::video_instance* video = nullptr;
    keyboard_manager_data data;
};

} // namespace keyboard
} // namespace app
} // namespace vx
