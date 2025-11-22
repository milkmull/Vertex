#include "vertex/system/error.hpp"
#include "vertex/util/encode/utf.hpp"
#include "vertex/util/memory/memory.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex_impl/app/input/keymap.hpp"

namespace vx {
namespace app {
namespace keyboard {

//=============================================================================
// keymap
//=============================================================================

#define key_to_scancode(key) (key & 0x00FFFF)
#define scancode_to_key(mod_state, sc) ((static_cast<uint32_t>((mod_state).raw()) << 16) | (sc))
#define get_mod(sc) (static_cast<key_mod>(sc >> 16))

#define check_scancode_range(sc, ret) \
do \
{ \
    if ((sc) >= scancode_count) \
    { \
        err::set(err::invalid_argument, "sc"); \
        return (ret); \
    } \
} while (VX_NULL_WHILE_LOOP_CONDITION)

//=============================================================================

static key_mod normalize_mod_state(key_mod state)
{
    // The modifiers that affect the keymap are: SHIFT, CAPS, ALT, MODE, and LEVEL5
    state &= (key_mod::shift | key_mod::caps | key_mod::alt | key_mod::mode | key_mod::level5);

    // If either right or left Shift are set, set both in the output
    if (state & key_mod::shift)
    {
        state |= key_mod::shift;
    }

    // If either right or left Alt are set, set both in the output
    if (state & key_mod::alt)
    {
        state |= key_mod::alt;
    }

    return state;
}

//=============================================================================

void keymap::set_entry(scancode sc, key_mod mod_state, keycode kc)
{
    mod_state = normalize_mod_state(mod_state);

    const uint32_t key = scancode_to_key(mod_state, sc);

    {
        const auto it = data.scancode_to_keycode.find(key);
        if (it != data.scancode_to_keycode.end() && it->second == kc)
        {
            // already exists
            return;
        }
    }

    // insert into map
    data.scancode_to_keycode[key] = kc;

    bool update_keycode = true;

    const auto it = data.keycode_to_scancode.find(kc);
    if (it != data.keycode_to_scancode.end())
    {
        const uint32_t existing_value = it->second;
        const key_mod existing_state = get_mod(existing_value);

        // Keep the simplest combo of scancode and mods to generate this keycode
        if (existing_state <= mod_state)
        {
            update_keycode = false;
        }
    }

    if (update_keycode)
    {
        data.keycode_to_scancode[kc] = key;
    }
}

//=============================================================================

static const keycode normal_default_symbols[] =
{
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,
    key_0,
    key_return,
    key_escape,
    key_backspace,
    key_tab,
    key_space,
    key_minus,
    key_equals,
    key_leftbracket,
    key_rightbracket,
    key_backslash,
    key_hash,
    key_semicolon,
    key_apostrophe,
    key_grave,
    key_comma,
    key_period,
    key_slash,
};

//=============================================================================

static const keycode shifted_default_symbols[] =
{
    key_exclaim,
    key_at,
    key_hash,
    key_dollar,
    key_percent,
    key_caret,
    key_ampersand,
    key_asterisk,
    key_leftparen,
    key_rightparen,
    key_return,
    key_escape,
    key_backspace,
    key_tab,
    key_space,
    key_underscore,
    key_plus,
    key_leftbrace,
    key_rightbrace,
    key_pipe,
    key_hash,
    key_colon,
    key_dblapostrophe,
    key_tilde,
    key_less,
    key_greater,
    key_question
};

//=============================================================================

static keycode get_default_key_from_scancode(scancode sc, key_mod mod_state)
{
    check_scancode_range(sc, key_unknown);

    if (sc < scancode_a)
    {
        return key_unknown;
    }

    if (sc < scancode_1)
    {
        if (mod_state & key_mod::mode)
        {
            return key_unknown;
        }

#if defined(VX_OS_APPLE)

        // Apple maps to upper case for either shift or capslock inclusive
        const bool shifted = mod_state & (key_mod::shift | key_mod::caps);

#else

        // We are shifted if either shift or caps is present, but not both
        const bool shifted = (mod_state & key_mod::shift) ^ (mod_state & key_mod::caps);

#endif // VX_OS_APPLE

        const char start = !shifted ? 'a' : 'A';
        return static_cast<keycode>(start + sc - scancode_a);
    }

    if (sc < scancode_capslock)
    {
        if (mod_state & key_mod::mode)
        {
            return key_unknown;
        }

        const bool shifted = mod_state & key_mod::shift;

        if (!shifted)
        {
            return normal_default_symbols[sc - scancode_1];
        }
        else
        {
            return shifted_default_symbols[sc - scancode_1];
        }
    }

    // These scancodes are not mapped to printable keycodes
    switch (sc)
    {
        case scancode_delete:               return key_delete;
        case scancode_capslock:             return key_capslock;
        case scancode_f1:                   return key_f1;
        case scancode_f2:                   return key_f2;
        case scancode_f3:                   return key_f3;
        case scancode_f4:                   return key_f4;
        case scancode_f5:                   return key_f5;
        case scancode_f6:                   return key_f6;
        case scancode_f7:                   return key_f7;
        case scancode_f8:                   return key_f8;
        case scancode_f9:                   return key_f9;
        case scancode_f10:                  return key_f10;
        case scancode_f11:                  return key_f11;
        case scancode_f12:                  return key_f12;
        case scancode_printscreen:          return key_printscreen;
        case scancode_scrolllock:           return key_scrolllock;
        case scancode_pause:                return key_pause;
        case scancode_insert:               return key_insert;
        case scancode_home:                 return key_home;
        case scancode_pageup:               return key_pageup;
        case scancode_end:                  return key_end;
        case scancode_pagedown:             return key_pagedown;
        case scancode_right:                return key_right;
        case scancode_left:                 return key_left;
        case scancode_down:                 return key_down;
        case scancode_up:                   return key_up;
        case scancode_numlock_clear:        return key_numlock_clear;
        case scancode_kp_divide:            return key_kp_divide;
        case scancode_kp_multiply:          return key_kp_multiply;
        case scancode_kp_minus:             return key_kp_minus;
        case scancode_kp_plus:              return key_kp_plus;
        case scancode_kp_enter:             return key_kp_enter;
        case scancode_kp_1:                 return key_kp_1;
        case scancode_kp_2:                 return key_kp_2;
        case scancode_kp_3:                 return key_kp_3;
        case scancode_kp_4:                 return key_kp_4;
        case scancode_kp_5:                 return key_kp_5;
        case scancode_kp_6:                 return key_kp_6;
        case scancode_kp_7:                 return key_kp_7;
        case scancode_kp_8:                 return key_kp_8;
        case scancode_kp_9:                 return key_kp_9;
        case scancode_kp_0:                 return key_kp_0;
        case scancode_kp_period:            return key_kp_period;
        case scancode_application:          return key_application;
        case scancode_power:                return key_power;
        case scancode_kp_equals:            return key_kp_equals;
        case scancode_f13:                  return key_f13;
        case scancode_f14:                  return key_f14;
        case scancode_f15:                  return key_f15;
        case scancode_f16:                  return key_f16;
        case scancode_f17:                  return key_f17;
        case scancode_f18:                  return key_f18;
        case scancode_f19:                  return key_f19;
        case scancode_f20:                  return key_f20;
        case scancode_f21:                  return key_f21;
        case scancode_f22:                  return key_f22;
        case scancode_f23:                  return key_f23;
        case scancode_f24:                  return key_f24;
        case scancode_execute:              return key_execute;
        case scancode_help:                 return key_help;
        case scancode_menu:                 return key_menu;
        case scancode_select:               return key_select;
        case scancode_stop:                 return key_stop;
        case scancode_again:                return key_again;
        case scancode_undo:                 return key_undo;
        case scancode_cut:                  return key_cut;
        case scancode_copy:                 return key_copy;
        case scancode_paste:                return key_paste;
        case scancode_find:                 return key_find;
        case scancode_mute:                 return key_mute;
        case scancode_volumeup:             return key_volumeup;
        case scancode_volumedown:           return key_volumedown;
        case scancode_kp_comma:             return key_kp_comma;
        case scancode_kp_equalsas400:       return key_kp_equalsas400;
        case scancode_alterase:             return key_alterase;
        case scancode_sysreq:               return key_sysreq;
        case scancode_cancel:               return key_cancel;
        case scancode_clear:                return key_clear;
        case scancode_prior:                return key_prior;
        case scancode_return2:              return key_return2;
        case scancode_separator:            return key_separator;
        case scancode_out:                  return key_out;
        case scancode_oper:                 return key_oper;
        case scancode_clearagain:           return key_clearagain;
        case scancode_crsel:                return key_crsel;
        case scancode_exsel:                return key_exsel;
        case scancode_kp_00:                return key_kp_00;
        case scancode_kp_000:               return key_kp_000;
        case scancode_thousandsseparator:   return key_thousandsseparator;
        case scancode_decimalseparator:     return key_decimalseparator;
        case scancode_currencyunit:         return key_currencyunit;
        case scancode_currencysubunit:      return key_currencysubunit;
        case scancode_kp_leftparen:         return key_kp_leftparen;
        case scancode_kp_rightparen:        return key_kp_rightparen;
        case scancode_kp_leftbrace:         return key_kp_leftbrace;
        case scancode_kp_rightbrace:        return key_kp_rightbrace;
        case scancode_kp_tab:               return key_kp_tab;
        case scancode_kp_backspace:         return key_kp_backspace;
        case scancode_kp_a:                 return key_kp_a;
        case scancode_kp_b:                 return key_kp_b;
        case scancode_kp_c:                 return key_kp_c;
        case scancode_kp_d:                 return key_kp_d;
        case scancode_kp_e:                 return key_kp_e;
        case scancode_kp_f:                 return key_kp_f;
        case scancode_kp_xor:               return key_kp_xor;
        case scancode_kp_power:             return key_kp_power;
        case scancode_kp_percent:           return key_kp_percent;
        case scancode_kp_less:              return key_kp_less;
        case scancode_kp_greater:           return key_kp_greater;
        case scancode_kp_ampersand:         return key_kp_ampersand;
        case scancode_kp_dblampersand:      return key_kp_dblampersand;
        case scancode_kp_verticalbar:       return key_kp_verticalbar;
        case scancode_kp_dblverticalbar:    return key_kp_dblverticalbar;
        case scancode_kp_colon:             return key_kp_colon;
        case scancode_kp_hash:              return key_kp_hash;
        case scancode_kp_space:             return key_kp_space;
        case scancode_kp_at:                return key_kp_at;
        case scancode_kp_exclam:            return key_kp_exclam;
        case scancode_kp_memstore:          return key_kp_memstore;
        case scancode_kp_memrecall:         return key_kp_memrecall;
        case scancode_kp_memclear:          return key_kp_memclear;
        case scancode_kp_memadd:            return key_kp_memadd;
        case scancode_kp_memsubtract:       return key_kp_memsubtract;
        case scancode_kp_memmultiply:       return key_kp_memmultiply;
        case scancode_kp_memdivide:         return key_kp_memdivide;
        case scancode_kp_plusminus:         return key_kp_plusminus;
        case scancode_kp_clear:             return key_kp_clear;
        case scancode_kp_clearentry:        return key_kp_clearentry;
        case scancode_kp_binary:            return key_kp_binary;
        case scancode_kp_octal:             return key_kp_octal;
        case scancode_kp_decimal:           return key_kp_decimal;
        case scancode_kp_hexadecimal:       return key_kp_hexadecimal;
        case scancode_lctrl:                return key_lctrl;
        case scancode_lshift:               return key_lshift;
        case scancode_lalt:                 return key_lalt;
        case scancode_lgui:                 return key_lgui;
        case scancode_rctrl:                return key_rctrl;
        case scancode_rshift:               return key_rshift;
        case scancode_ralt:                 return key_ralt;
        case scancode_rgui:                 return key_rgui;
        case scancode_mode:                 return key_mode;
        case scancode_sleep:                return key_sleep;
        case scancode_wake:                 return key_wake;
        case scancode_channel_increment:    return key_channel_increment;
        case scancode_channel_decrement:    return key_channel_decrement;
        case scancode_media_play:           return key_media_play;
        case scancode_media_pause:          return key_media_pause;
        case scancode_media_record:         return key_media_record;
        case scancode_media_fast_forward:   return key_media_fast_forward;
        case scancode_media_rewind:         return key_media_rewind;
        case scancode_media_next_track:     return key_media_next_track;
        case scancode_media_previous_track: return key_media_previous_track;
        case scancode_media_stop:           return key_media_stop;
        case scancode_media_eject:          return key_media_eject;
        case scancode_media_play_pause:     return key_media_play_pause;
        case scancode_media_select:         return key_media_select;
        case scancode_ac_new:               return key_ac_new;
        case scancode_ac_open:              return key_ac_open;
        case scancode_ac_close:             return key_ac_close;
        case scancode_ac_exit:              return key_ac_exit;
        case scancode_ac_save:              return key_ac_save;
        case scancode_ac_print:             return key_ac_print;
        case scancode_ac_properties:        return key_ac_properties;
        case scancode_ac_search:            return key_ac_search;
        case scancode_ac_home:              return key_ac_home;
        case scancode_ac_back:              return key_ac_back;
        case scancode_ac_forward:           return key_ac_forward;
        case scancode_ac_stop:              return key_ac_stop;
        case scancode_ac_refresh:           return key_ac_refresh;
        case scancode_ac_bookmarks:         return key_ac_bookmarks;
        case scancode_softleft:             return key_softleft;
        case scancode_softright:            return key_softright;
        case scancode_call:                 return key_call;
        case scancode_endcall:              return key_endcall;
        default:                            return key_unknown;
    }
}

//=============================================================================

static const struct
{
    keycode first;
    scancode second;
}
extended_default_symbols[] =
{
    { key_left_tab,             scancode_tab            },
    { key_multi_key_compose,    scancode_application    }, // sun keyboards
    { key_lmeta,                scancode_lgui           },
    { key_rmeta,                scancode_rgui           },
    { key_rhyper,               scancode_application    }

};

//=============================================================================

static scancode get_default_scancode_from_key(keycode key, key_mod* mod_state)
{
    if (mod_state)
    {
        *mod_state = key_mod::none;
    }

    if (key == key_unknown)
    {
        return scancode_unknown;
    }

    if (key & key_extended_mask)
    {
        for (const auto& pair : extended_default_symbols)
        {
            if (key == pair.first)
            {
                return pair.second;
            }
        }
    }

    if (key & key_scancode_mask)
    {
        return static_cast<scancode>(key & ~key_scancode_mask);
    }

    if (key >= key_a && key <= key_z)
    {
        return static_cast<scancode>(scancode_a + key - key_a);
    }

    if (key >= 'A' && key <= 'Z')
    {
        if (mod_state)
        {
            *mod_state = key_mod::shift;
        }

        return static_cast<scancode>(scancode_a + key - 'A');
    }

    for (int i = 0; i < mem::array_size(normal_default_symbols); ++i)
    {
        if (key == normal_default_symbols[i])
        {
            return static_cast<scancode>(scancode_1 + i);
        }
    }

    for (int i = 0; i < mem::array_size(shifted_default_symbols); ++i)
    {
        if (key == shifted_default_symbols[i])
        {
            if (mod_state)
            {
                *mod_state = key_mod::shift;
            }

            return static_cast<scancode>(scancode_1 + i);
        }
    }

    if (key == key_delete)
    {
        return scancode_delete;
    }

    return scancode_unknown;
}

//=============================================================================

keycode keymap::get_keycode(scancode sc, key_mod mod_state) const
{
    const key_mod normalized_mod_state = normalize_mod_state(mod_state);

    // First: direct lookup with the given modifiers
    {
        const uint32_t key = scancode_to_key(normalized_mod_state, sc);
        const auto it = data.scancode_to_keycode.find(key);
        if (it != data.scancode_to_keycode.end())
        {
            return static_cast<keycode>(it->second);
        }
    }

    // Build fallback search order.
    // Order matches the original priority:
    const key_mod mod_level[] =
    {
        key_mod::level5,                    // shift level 5
        key_mod::mode | key_mod::shift,     // Shift level 4 (level 3 + shift)
        key_mod::mode,                      // Shift level 3
        key_mod::shift,                     // Shift level 2
        key_mod::none                       // Shift level 1 (unmodified)
    };

    // Determine whether capslock should be included in fallback variants
    key_mod caps_mask = normalized_mod_state & key_mod::caps;

    // Try the fallback sequences twice:
    // First with caps (if applicable), then without caps
    for (int i = 0; i < (caps_mask ? 2 : 1); ++i)
    {
        for (key_mod base_mods : mod_level)
        {
            const key_mod shifted_mod_state = base_mods | caps_mask;

            // Skip the combination already tested
            if (shifted_mod_state == normalized_mod_state)
            {
                continue;
            }

            const uint32_t key = scancode_to_key(shifted_mod_state, sc);
            auto it = data.scancode_to_keycode.find(key);
            if (it != data.scancode_to_keycode.end())
            {
                return static_cast<keycode>(it->second);
            }
        }

        // Second pass: remove caps
        caps_mask = key_mod::none;
    }

    return get_default_key_from_scancode(sc, mod_state);
}

//=============================================================================

scancode keymap::get_scancode(keycode key, key_mod* mod_state) const
{
    const auto it = data.keycode_to_scancode.find(key);
    if (it != data.keycode_to_scancode.end())
    {
        if (mod_state)
        {
            *mod_state = get_mod(it->second);
        }

        return static_cast<scancode>(key_to_scancode(it->second));
    }
    
    return get_default_scancode_from_key(key, mod_state);
}

//=============================================================================

scancode keymap::get_next_reserved_scancode()
{
    scancode sc;

    if (data.next_reserved_scancode && data.next_reserved_scancode < scancode_reserved_last)
    {
        sc = static_cast<scancode>(data.next_reserved_scancode);
    }
    else
    {
        sc = scancode_reserved;
    }

    data.next_reserved_scancode = sc + 1;

    return sc;
}

//=============================================================================

#if !defined(VX_DISABLE_KEYBOARD_NAME_MAPPING)

static const char* scancode_names[scancode_count] =
{
    /* 0 */ nullptr,
    /* 1 */ nullptr,
    /* 2 */ nullptr,
    /* 3 */ nullptr,
    /* 4 */ "A",
    /* 5 */ "B",
    /* 6 */ "C",
    /* 7 */ "D",
    /* 8 */ "E",
    /* 9 */ "F",
    /* 10 */ "G",
    /* 11 */ "H",
    /* 12 */ "I",
    /* 13 */ "J",
    /* 14 */ "K",
    /* 15 */ "L",
    /* 16 */ "M",
    /* 17 */ "N",
    /* 18 */ "O",
    /* 19 */ "P",
    /* 20 */ "Q",
    /* 21 */ "R",
    /* 22 */ "S",
    /* 23 */ "T",
    /* 24 */ "U",
    /* 25 */ "V",
    /* 26 */ "W",
    /* 27 */ "X",
    /* 28 */ "Y",
    /* 29 */ "Z",
    /* 30 */ "1",
    /* 31 */ "2",
    /* 32 */ "3",
    /* 33 */ "4",
    /* 34 */ "5",
    /* 35 */ "6",
    /* 36 */ "7",
    /* 37 */ "8",
    /* 38 */ "9",
    /* 39 */ "0",
    /* 40 */ "Return",
    /* 41 */ "Escape",
    /* 42 */ "Backspace",
    /* 43 */ "Tab",
    /* 44 */ "Space",
    /* 45 */ "-",
    /* 46 */ "=",
    /* 47 */ "[",
    /* 48 */ "]",
    /* 49 */ "\\",
    /* 50 */ "#",
    /* 51 */ ";",
    /* 52 */ "'",
    /* 53 */ "`",
    /* 54 */ ",",
    /* 55 */ ".",
    /* 56 */ "/",
    /* 57 */ "CapsLock",
    /* 58 */ "F1",
    /* 59 */ "F2",
    /* 60 */ "F3",
    /* 61 */ "F4",
    /* 62 */ "F5",
    /* 63 */ "F6",
    /* 64 */ "F7",
    /* 65 */ "F8",
    /* 66 */ "F9",
    /* 67 */ "F10",
    /* 68 */ "F11",
    /* 69 */ "F12",
    /* 70 */ "PrintScreen",
    /* 71 */ "ScrollLock",
    /* 72 */ "Pause",
    /* 73 */ "Insert",
    /* 74 */ "Home",
    /* 75 */ "PageUp",
    /* 76 */ "Delete",
    /* 77 */ "End",
    /* 78 */ "PageDown",
    /* 79 */ "Right",
    /* 80 */ "Left",
    /* 81 */ "Down",
    /* 82 */ "Up",
    /* 83 */ "Numlock",
    /* 84 */ "Keypad /",
    /* 85 */ "Keypad *",
    /* 86 */ "Keypad -",
    /* 87 */ "Keypad +",
    /* 88 */ "Keypad Enter",
    /* 89 */ "Keypad 1",
    /* 90 */ "Keypad 2",
    /* 91 */ "Keypad 3",
    /* 92 */ "Keypad 4",
    /* 93 */ "Keypad 5",
    /* 94 */ "Keypad 6",
    /* 95 */ "Keypad 7",
    /* 96 */ "Keypad 8",
    /* 97 */ "Keypad 9",
    /* 98 */ "Keypad 0",
    /* 99 */ "Keypad .",
    /* 100 */ "NonUSBackslash",
    /* 101 */ "Application",
    /* 102 */ "Power",
    /* 103 */ "Keypad =",
    /* 104 */ "F13",
    /* 105 */ "F14",
    /* 106 */ "F15",
    /* 107 */ "F16",
    /* 108 */ "F17",
    /* 109 */ "F18",
    /* 110 */ "F19",
    /* 111 */ "F20",
    /* 112 */ "F21",
    /* 113 */ "F22",
    /* 114 */ "F23",
    /* 115 */ "F24",
    /* 116 */ "Execute",
    /* 117 */ "Help",
    /* 118 */ "Menu",
    /* 119 */ "Select",
    /* 120 */ "Stop",
    /* 121 */ "Again",
    /* 122 */ "Undo",
    /* 123 */ "Cut",
    /* 124 */ "Copy",
    /* 125 */ "Paste",
    /* 126 */ "Find",
    /* 127 */ "Mute",
    /* 128 */ "VolumeUp",
    /* 129 */ "VolumeDown",
    /* 130 */ nullptr,
    /* 131 */ nullptr,
    /* 132 */ nullptr,
    /* 133 */ "Keypad ,",
    /* 134 */ "Keypad = (AS400)",
    /* 135 */ "International 1",
    /* 136 */ "International 2",
    /* 137 */ "International 3",
    /* 138 */ "International 4",
    /* 139 */ "International 5",
    /* 140 */ "International 6",
    /* 141 */ "International 7",
    /* 142 */ "International 8",
    /* 143 */ "International 9",
    /* 144 */ "Language 1",
    /* 145 */ "Language 2",
    /* 146 */ "Language 3",
    /* 147 */ "Language 4",
    /* 148 */ "Language 5",
    /* 149 */ "Language 6",
    /* 150 */ "Language 7",
    /* 151 */ "Language 8",
    /* 152 */ "Language 9",
    /* 153 */ "AltErase",
    /* 154 */ "SysReq",
    /* 155 */ "Cancel",
    /* 156 */ "Clear",
    /* 157 */ "Prior",
    /* 158 */ "Return",
    /* 159 */ "Separator",
    /* 160 */ "Out",
    /* 161 */ "Oper",
    /* 162 */ "Clear / Again",
    /* 163 */ "CrSel",
    /* 164 */ "ExSel",
    /* 165 */ nullptr,
    /* 166 */ nullptr,
    /* 167 */ nullptr,
    /* 168 */ nullptr,
    /* 169 */ nullptr,
    /* 170 */ nullptr,
    /* 171 */ nullptr,
    /* 172 */ nullptr,
    /* 173 */ nullptr,
    /* 174 */ nullptr,
    /* 175 */ nullptr,
    /* 176 */ "Keypad 00",
    /* 177 */ "Keypad 000",
    /* 178 */ "ThousandsSeparator",
    /* 179 */ "DecimalSeparator",
    /* 180 */ "CurrencyUnit",
    /* 181 */ "CurrencySubUnit",
    /* 182 */ "Keypad (",
    /* 183 */ "Keypad )",
    /* 184 */ "Keypad {",
    /* 185 */ "Keypad }",
    /* 186 */ "Keypad Tab",
    /* 187 */ "Keypad Backspace",
    /* 188 */ "Keypad A",
    /* 189 */ "Keypad B",
    /* 190 */ "Keypad C",
    /* 191 */ "Keypad D",
    /* 192 */ "Keypad E",
    /* 193 */ "Keypad F",
    /* 194 */ "Keypad XOR",
    /* 195 */ "Keypad ^",
    /* 196 */ "Keypad %",
    /* 197 */ "Keypad <",
    /* 198 */ "Keypad >",
    /* 199 */ "Keypad &",
    /* 200 */ "Keypad &&",
    /* 201 */ "Keypad |",
    /* 202 */ "Keypad ||",
    /* 203 */ "Keypad :",
    /* 204 */ "Keypad #",
    /* 205 */ "Keypad Space",
    /* 206 */ "Keypad @",
    /* 207 */ "Keypad !",
    /* 208 */ "Keypad MemStore",
    /* 209 */ "Keypad MemRecall",
    /* 210 */ "Keypad MemClear",
    /* 211 */ "Keypad MemAdd",
    /* 212 */ "Keypad MemSubtract",
    /* 213 */ "Keypad MemMultiply",
    /* 214 */ "Keypad MemDivide",
    /* 215 */ "Keypad +/-",
    /* 216 */ "Keypad Clear",
    /* 217 */ "Keypad ClearEntry",
    /* 218 */ "Keypad Binary",
    /* 219 */ "Keypad Octal",
    /* 220 */ "Keypad Decimal",
    /* 221 */ "Keypad Hexadecimal",
    /* 222 */ nullptr,
    /* 223 */ nullptr,
    /* 224 */ "Left Ctrl",
    /* 225 */ "Left Shift",
    /* 226 */ "Left Alt",
    /* 227 */ "Left GUI",
    /* 228 */ "Right Ctrl",
    /* 229 */ "Right Shift",
    /* 230 */ "Right Alt",
    /* 231 */ "Right GUI",
    /* 232 */ nullptr,
    /* 233 */ nullptr,
    /* 234 */ nullptr,
    /* 235 */ nullptr,
    /* 236 */ nullptr,
    /* 237 */ nullptr,
    /* 238 */ nullptr,
    /* 239 */ nullptr,
    /* 240 */ nullptr,
    /* 241 */ nullptr,
    /* 242 */ nullptr,
    /* 243 */ nullptr,
    /* 244 */ nullptr,
    /* 245 */ nullptr,
    /* 246 */ nullptr,
    /* 247 */ nullptr,
    /* 248 */ nullptr,
    /* 249 */ nullptr,
    /* 250 */ nullptr,
    /* 251 */ nullptr,
    /* 252 */ nullptr,
    /* 253 */ nullptr,
    /* 254 */ nullptr,
    /* 255 */ nullptr,
    /* 256 */ nullptr,
    /* 257 */ "ModeSwitch",
    /* 258 */ "Sleep",
    /* 259 */ "Wake",
    /* 260 */ "ChannelUp",
    /* 261 */ "ChannelDown",
    /* 262 */ "MediaPlay",
    /* 263 */ "MediaPause",
    /* 264 */ "MediaRecord",
    /* 265 */ "MediaFastForward",
    /* 266 */ "MediaRewind",
    /* 267 */ "MediaTrackNext",
    /* 268 */ "MediaTrackPrevious",
    /* 269 */ "MediaStop",
    /* 270 */ "Eject",
    /* 271 */ "MediaPlayPause",
    /* 272 */ "MediaSelect",
    /* 273 */ "AC New",
    /* 274 */ "AC Open",
    /* 275 */ "AC Close",
    /* 276 */ "AC Exit",
    /* 277 */ "AC Save",
    /* 278 */ "AC Print",
    /* 279 */ "AC Properties",
    /* 280 */ "AC Search",
    /* 281 */ "AC Home",
    /* 282 */ "AC Back",
    /* 283 */ "AC Forward",
    /* 284 */ "AC Stop",
    /* 285 */ "AC Refresh",
    /* 286 */ "AC Bookmarks",
    /* 287 */ "SoftLeft",
    /* 288 */ "SoftRight",
    /* 289 */ "Call",
    /* 290 */ "EndCall"
};

static const char* extended_key_names[] =
{
    "LeftTab",         /* 0x01 key_left_tab */
    "Level5Shift",     /* 0x02 key_level5_shift */
    "MultiKeyCompose", /* 0x03 key_multi_key_compose */
    "Left Meta",       /* 0x04 key_lmeta */
    "Right Meta",      /* 0x05 key_rmeta */
    "Left Hyper",      /* 0x06 key_lhyper */
    "Right Hyper"      /* 0x07 key_rhyper */
};

//=============================================================================

const char* get_scancode_name_internal(scancode sc)
{
    check_scancode_range(sc, "");
    const char* name = scancode_names[sc];
    return name ? name : "";
}

//=============================================================================

bool set_scancode_name_internal(scancode sc, const char* name)
{
    check_scancode_range(sc, false);
    scancode_names[sc] = name;
    return true;
}

//=============================================================================

scancode get_scancode_from_name_internal(const char* name)
{
    if (!name || !*name)
    {
        err::set(err::invalid_argument, "name");
        return scancode_unknown;
    }

    for (uint32_t i = 0; i < mem::array_size(scancode_names); ++i)
    {
        if (scancode_names[i] && str::casecmp(scancode_names[i], name))
        {
            return static_cast<scancode>(i);
        }
    }

    err::set(err::invalid_argument, "name");
    return scancode_unknown;
}

//=============================================================================

std::string get_key_name_internal(keycode key, const keymap* map)
{
    constexpr bool uppercase = true;

    if (key & key_scancode_mask)
    {
        return get_scancode_name_internal(static_cast<scancode>(key & ~key_scancode_mask));
    }

    if (key & key_extended_mask)
    {
        const size_t i = static_cast<size_t>(key & ~key_extended_mask);
        if ((i - 1) < mem::array_size(extended_key_names))
        {
            return extended_key_names[i - 1];
        }

        err::set(err::invalid_argument, "key");
        return "";
    }

    switch (key)
    {
        case key_return:    return get_scancode_name_internal(scancode_return);
        case key_escape:    return get_scancode_name_internal(scancode_escape);
        case key_backspace: return get_scancode_name_internal(scancode_backspace);
        case key_tab:       return get_scancode_name_internal(scancode_tab);
        case key_space:     return get_scancode_name_internal(scancode_space);
        case key_delete:    return get_scancode_name_internal(scancode_delete);
        default:            break;
    }

    VX_IF_CONSTEXPR(uppercase)
    {
        if (map)
        {
            // keycode is defined as the unshifted key on the keyboard,
            // but the key name is defined as the letter printed on that key,
            // which is usually the shifted capital letter.

            if (key > 0x7F || ('a' < key && key <= 'z'))
            {
                key_mod mod_state;
                const scancode sc = map->get_scancode(key, &mod_state);

                if (sc != scancode_unknown && !(mod_state & key_mod::shift))
                {
                    const keycode capital = map->get_keycode(sc, key_mod::shift);
                    if (0x7F < capital || ('A' <= capital && capital <= 'Z'))
                    {
                        key = capital;
                    }
                }
            }
        }
    }

    const char32_t* ptr = reinterpret_cast<char32_t*>(&key);
    return str::string_cast<char>(ptr, ptr + 1);
}

//=============================================================================

keycode get_key_from_name_internal(const char* name, const keymap* map)
{
    constexpr bool uppercase = true;

    if (!name || !*name)
    {
        return key_unknown;
    }

    keycode key = key_unknown;

    // If it's a single UTF-8 character, then that's the keycode itself
    using utf8 = utf::utf_traits<uint8_t>;
    const uint8_t lead = static_cast<uint8_t>(name[0]);

    if (utf8::is_lead(lead))
    {
        const size_t size = std::strlen(name);
        const size_t trail_count = utf8::trail_count(lead);

        if (size == 1 + trail_count)
        {
            utf::code_point cp;
            const char* end = name + size;
            
            if (utf8::decode(name, end, cp) == end)
            {
                key = static_cast<keycode>(cp);
            }
        }
    }

    if (key != key_unknown)
    {
        VX_IF_CONSTEXPR(uppercase)
        {
            // keycode is defined as the unshifted key on the keyboard,
            // but the key name is defined as the letter printed on that key,
            // which is usually the shifted capital letter.

            if (map)
            {
                key_mod mod_state;
                const scancode sc = map->get_scancode(key, &mod_state);

                if (sc != scancode_unknown && (mod_state & (key_mod::shift | key_mod::caps)))
                {
                    key = map->get_keycode(sc, key_mod::none);
                }
            }
        }
    }

    return key;
}

#endif // VX_DISABLE_KEYBOARD_NAME_MAPPING

} // namespace keyboard
} // namespace app
} // namespace vx