#pragma once

#include <unordered_map>

#include "vertex/app/input/keyboard.hpp"

namespace vx {
namespace app {
namespace keyboard {

//=============================================================================
// key_map
//=============================================================================

struct keymap_data
{
    std::unordered_map<uint32_t, uint32_t> scancode_to_keycode;
    std::unordered_map<uint32_t, uint32_t> keycode_to_scancode;
    uint32_t next_reserved_scancode;

    bool layout_determined;

    bool french_numbers;
    bool thai_keyboard;
    bool latin_letters;
};

class keymap
{
public:

    keymap() = default;
    ~keymap() = default;

    keymap(const keymap&) = default;
    keymap& operator=(const keymap&) = default;

    keymap(keymap&&) noexcept = default;
    keymap& operator=(keymap&&) noexcept = default;

public:

    void set_entry(scancode sc, key_mod mod_state, keycode kc);
    keycode get_keycode(scancode sc, key_mod mod_state) const;
    scancode get_scancode(keycode key, key_mod* mod_state) const;
    scancode get_next_reserved_scancode();

public:

    keymap_data data{};
};

//=============================================================================
// name conversions
//=============================================================================

#if !defined(VX_DISABLE_KEYBOARD_NAME_MAPPING)

const char* get_scancode_name_internal(scancode sc);
bool set_scancode_name_internal(scancode sc, const char* name);
scancode get_scancode_from_name_internal(const char* name);

std::string get_key_name_internal(keycode key, const keymap* map);
keycode get_key_from_name_internal(const char* name, const keymap* map);

#endif // VX_DISABLE_KEYBOARD_NAME_MAPPING

} // namespace keyboard
} // namespace app
} // namespace vx