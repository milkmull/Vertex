#include "vertex_impl/app/event/event_internal.hpp"
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
    clear_keymap();

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
// devices
//=============================================================================

size_t keyboard_instance::get_keyboard_index(keyboard_id id) const
{
    if (!is_valid_id(id))
    {
        return VX_INVALID_INDEX;
    }

    for (size_t i = 0; i < data.keyboards.size(); ++i)
    {
        if (data.keyboards[i].id == id)
        {
            return i;
        }
    }

    return VX_INVALID_INDEX;
}

//=============================================================================

keyboard_info* keyboard_instance::get_keyboard(keyboard_id id)
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (keyboard_info& kb : data.keyboards)
    {
        if (kb.id == id)
        {
            return &kb;
        }
    }

    return nullptr;
}

const keyboard_info* keyboard_instance::get_keyboard(keyboard_id id) const
{
    return const_cast<keyboard_instance*>(this)->get_keyboard(id);
}

//=============================================================================

void keyboard_instance::add_keyboard(keyboard_id id, const char* name)
{
    const size_t i = get_keyboard_index(id);
    if (i == VX_INVALID_INDEX)
    {
        // we already know about this keyboard
        return;
    }

    VX_ASSERT(is_valid_id(id));

    keyboard_info info{};
    info.id = id;
    info.name = name ? name : "Keyboard";

    data.keyboards.push_back(info);
    send_keyboard_added(id);
}

//=============================================================================

void keyboard_instance::remove_keyboard(keyboard_id id)
{
    for (auto it = data.keyboards.begin(); it != data.keyboards.end(); ++it)
    {
        if (it->id == id)
        {
            data.keyboards.erase(it);
            break;
        }
    }

    if (!data.quitting)
    {
        send_keyboard_removed(id);
    }
}

//=============================================================================

void keyboard_instance::clear_keyboards()
{
    while (!data.keyboards.empty())
    {
        remove_keyboard(data.keyboards[0].id);
    }
}

//=============================================================================

bool keyboard_instance::is_keyboard(uint16_t vendor, uint16_t product, size_t key_count)
{
    enum { real_keyboard_key_count = 50 };

    if (key_count < real_keyboard_key_count)
    {
        return false;
    }

    return true;
}

//=============================================================================

bool keyboard_instance::any_connected() const
{
    return !data.keyboards.empty();
}

//=============================================================================

std::vector<keyboard_id> keyboard_instance::list_keyboards() const
{
    std::vector<keyboard_id> keyboards(data.keyboards.size());

    for (size_t i = 0; i < data.keyboards.size(); ++i)
    {
        keyboards[i] = data.keyboards[i].id;
    }

    return keyboards;
}

//=============================================================================

const char* keyboard_instance::get_name(keyboard_id id) const
{
    const keyboard_info* kb = get_keyboard(id);
    return kb ? kb->name.c_str() : nullptr;
}

//=============================================================================
// focus
//=============================================================================

video::window_id keyboard_instance::get_focus() const
{
    return data.focus;
}

//=============================================================================

video::window_instance* keyboard_instance::get_focus_instance()
{
    return video->get_window_instance(data.focus);
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_keyboard.c#L326

bool keyboard_instance::set_focus(video::window_id wid)
{
    if (data.focus == wid)
    {
        return;
    }

    video::window_instance* new_focus = video->get_window_instance(wid);
    if (new_focus && new_focus->data.destroying)
    {
        return false;
    }

    video::window_instance* old_focus = get_focus_instance();

    // See if the current window has lost focus
    if (old_focus)
    {
        old_focus->send_lost_focus();

#if !defined(VX_PLATFORM_MOBILE)

        // Ensures IME compositions are committed
        if (old_focus->text_input_active())
        {
            old_focus->stop_text_input();
        }

#endif // !VX_PLATFORM_MOBILE
    }

    mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

    if (old_focus && !new_focus)
    {
        // We won't get anymore keyboard messages, so reset keyboard state
        reset();

        // Also leave mouse relative mode
        if (mouse->data.relative_mode_enabled)
        {
            mouse->set_relative_mode(false);

            if (old_focus->data.flags & video::window_flags::minimized)
            {
                // We can't warp the mouse within minimized windows, so manually restore the position
                const float x = old_focus->data.position.x + mouse->data.x;
                const float y = old_focus->data.position.y + mouse->data.y;
                mouse->warp_global(x, y);
            }
        }
    }

    data.focus = wid;

    if (new_focus)
    {
        new_focus->send_gained_focus();

#if !defined(VX_PLATFORM_MOBILE)

        if (new_focus->text_input_active())
        {
            new_focus->start_text_input();
        }

#endif // !VX_PLATFORM_MOBILE
    }

    mouse->update_relative_mode();
    return true;
}

//=============================================================================
// keymap
//=============================================================================

void keyboard_instance::clear_keymap()
{
    if (data.keymap_ptr && data.keymap_ptr->data.auto_release)
    {
        delete data.keymap_ptr;
    }

    data.keymap_ptr = nullptr;
}

//=============================================================================

void keyboard_instance::set_keymap(keymap* map, bool send_event)
{
    clear_keymap();
    data.keymap_ptr = map;

    if (map && !map->data.layout_determined)
    {
        // Detect whether the number row behaves like French AZERTY:
        // - Unshifted keys produce symbols (not digits)
        // - Shifted keys produce digits
        // This is used because French layouts invert the number row.
        {
            map->data.french_numbers = true;

            for (uint32_t i = scancode_1; i <= scancode_0; ++i)
            {
                if ((str::is_digit(map->get_keycode(static_cast<scancode>(i), key_mod::none)) ||
                    !str::is_digit(map->get_keycode(static_cast<scancode>(i), key_mod::shift))))
                {
                    // If unshifted is a digit OR shifted is not a digit,
                    // then it is *not* a French-style inverted number row.
                    map->data.french_numbers = false;
                    break;
                }
            }
        }

        // Detect whether the keymap uses Latin characters or Thai script.
        // We sample only a subset (scancode_a -> scancode_d),
        // because any Latin-based layout will produce ASCII/Latin-1 letters (<= 0xFF),
        // and Thai keyboards produce characters in U+0E00–U+0E7F.
        {
            map->data.thai_keyboard = false;
            map->data.latin_letters = false;

            for (uint32_t i = scancode_a; i <= scancode_d; ++i)
            {
                const keycode key = map->get_keycode(static_cast<scancode>(i), key_mod::none);

                // Latin layouts map alphabetic keys to ASCII or Latin-1.
                if (key <= 0xFF)
                {
                    map->data.latin_letters = true;
                    break;
                }

                // Thai layouts produce characters in the Thai Unicode block.
                if (key >= 0x0E00 && key <= 0x0E7F)
                {
                    map->data.thai_keyboard = true;
                    break;
                }
            }
        }

        map->data.layout_determined = true;
    }

    if (send_event)
    {
        send_keymap_changed();
    }
}

//=============================================================================

keymap* keyboard_instance::get_current_keymap(bool ignore_options)
{
    keymap* map = data.keymap_ptr;

    if (!ignore_options)
    {
        if (map && map->data.thai_keyboard)
        {
            // Thai keyboards are QWERTY plus Thai characters, use the default QWERTY keymap
            return nullptr;
        }

        if (data.keycode_options & keycode_options::latin_letters)
        {
            // We'll use the default QWERTY keymap
            return nullptr;
        }
    }

    return map;
}

const keymap* keyboard_instance::get_current_keymap(bool ignore_options) const
{
    return const_cast<keyboard_instance*>(this)->get_current_keymap(ignore_options);
}

//=============================================================================

static keycode convert_numpad_keycode(keycode kc, bool numlock) noexcept
{
    // Fast path: numpad A–F (contiguous linear range)
    if (key_kp_a <= kc && kc <= key_kp_f)
    {
        return static_cast<keycode>(key_a + (kc - key_kp_a));
    }

    switch (kc)
    {
        case key_kp_divide:         return key_slash;
        case key_kp_multiply:       return key_asterisk;
        case key_kp_minus:          return key_minus;
        case key_kp_plus:           return key_plus;
        case key_kp_enter:          return key_return;
        case key_kp_1:              return numlock ? key_1 : key_end;
        case key_kp_2:              return numlock ? key_2 : key_down;
        case key_kp_3:              return numlock ? key_3 : key_pagedown;
        case key_kp_4:              return numlock ? key_4 : key_left;
        case key_kp_5:              return numlock ? key_5 : key_clear;
        case key_kp_6:              return numlock ? key_6 : key_right;
        case key_kp_7:              return numlock ? key_7 : key_home;
        case key_kp_8:              return numlock ? key_8 : key_up;
        case key_kp_9:              return numlock ? key_9 : key_pageup;
        case key_kp_0:              return numlock ? key_0 : key_insert;
        case key_kp_period:         return numlock ? key_period : key_delete;
        case key_kp_equals:         return key_equals;
        case key_kp_comma:          return key_comma;
        case key_kp_equalsas400:    return key_equals;
        case key_kp_leftparen:      return key_leftparen;
        case key_kp_rightparen:     return key_rightparen;
        case key_kp_leftbrace:      return key_leftbrace;
        case key_kp_rightbrace:     return key_rightbrace;
        case key_kp_tab:            return key_tab;
        case key_kp_backspace:      return key_backspace;
        case key_kp_percent:        return key_percent;
        case key_kp_less:           return key_less;
        case key_kp_greater:        return key_greater;
        case key_kp_ampersand:      return key_ampersand;
        case key_kp_colon:          return key_colon;
        case key_kp_hash:           return key_hash;
        case key_kp_space:          return key_space;
        case key_kp_at:             return key_at;
        case key_kp_exclam:         return key_exclaim;
        case key_kp_plusminus:      return key_plusminus;
        default:                    return kc;
    }
}

//=============================================================================

keycode keyboard_instance::get_key_from_scancode(scancode sc, key_mod mod_state, bool key_event) const
{
    if (key_event)
    {
        const keymap* map = get_current_keymap(false);

        // no mods by default
        mod_state = key_mod::none;

        if ((data.keycode_options & keycode_options::french_numbers) &&
            (map && map->data.french_numbers) &&
            ((scancode_1 <= sc) && (sc <= scancode_0)))
        {
            // Add the shift state to generate a numeric keycode
            mod_state |= key_mod::shift;
        }

        keycode kc = map->get_keycode(sc, mod_state);

        if (data.keycode_options & keycode_options::hide_numpad)
        {
            const bool numlock = (mod_state & key_mod::num);
            kc = convert_numpad_keycode(kc, numlock);
        }

        return kc;
    }

    if (data.keymap_ptr)
    {
        return data.keymap_ptr->get_keycode(sc, mod_state);
    }

    return key_unknown;
}

//=============================================================================

scancode keyboard_instance::get_scancode_from_key(keycode key, key_mod* mod_state) const
{
    return data.keymap_ptr ? data.keymap_ptr->get_scancode(key, mod_state) : scancode_unknown;
}

//=============================================================================

bool keyboard_instance::set_scancode_name(scancode sc, const char* name)
{
    return set_scancode_name_internal(sc, name);
}

//=============================================================================

const char* keyboard_instance::get_scancode_name(scancode sc) const
{
    return get_scancode_name_internal(sc);
}

//=============================================================================

scancode keyboard_instance::get_scancode_from_name(const char* name) const
{
    return get_scancode_from_name_internal(name);
}

//=============================================================================

std::string keyboard_instance::get_key_name(keycode key) const
{
    return get_key_name_internal(key, data.keymap_ptr);
}

//=============================================================================

keycode keyboard_instance::get_key_from_name(const char* name) const
{
    return get_key_from_name_internal(name, data.keymap_ptr);
}

//=============================================================================
// Key State / Modifiers
//=============================================================================

const key_state& keyboard_instance::get_key_state() const
{
    return data.key_state;
}

//=============================================================================

void keyboard_instance::reset()
{
    for (uint32_t sc = scancode_unknown; sc < scancode_count; ++sc)
    {
        if (data.key_state[sc])
        {
            send_key(time::zero(), global_keyboard_id, 0, static_cast<scancode>(sc), false);
        }
    }
}

//=============================================================================

key_mod keyboard_instance::get_mod_state() const
{
    return data.mod_state;
}

//=============================================================================

void keyboard_instance::set_mod_state(key_mod mod_state)
{
    data.mod_state = mod_state;
}

//=============================================================================

void keyboard_instance::toggle_mod_state(key_mod mod_state, bool toggle)
{
    if (toggle)
    {
        data.mod_state |= mod_state;
    }
    else
    {
        data.mod_state &= ~mod_state;
    }
}

//=============================================================================
// Key Events
//=============================================================================

#define events_ptr video->app->data.events_ptr

//=============================================================================

void keyboard_instance::send_keymap_changed()
{
    event::event e{};
    e.type = event::keymap_changed;
    events_ptr->push_event(e);
}

//=============================================================================

void keyboard_instance::send_keyboard_added(keyboard_id id)
{
    event::event e{};
    e.type = event::keyboard_added;
    e.key_event.common.keyboard_id = id;
    events_ptr->push_event(e);
}

//=============================================================================

void keyboard_instance::send_keyboard_removed(keyboard_id id)
{
    event::event e{};
    e.type = event::keyboard_removed;
    e.key_event.common.keyboard_id = id;
    events_ptr->push_event(e);
}

//=============================================================================

bool keyboard_instance::send_key(time::time_point t, keyboard_id id, int raw, scancode sc, bool down)
{
    return send_key_internal(t, key_source::hardware, id, raw, sc, down);
}

bool keyboard_instance::send_key_internal(time::time_point t, key_source source, keyboard_id id, int raw, scancode sc, bool down)
{
    if (scancode_unknown < sc && sc < scancode_count)
    {
        // drop events that don't change state
        if (data.key_state[sc])
        {
            if (data.key_source[sc] != static_cast<bool>(source))
            {

            }
        }
    }
}

//=============================================================================

bool keyboard_instance::send_key_no_mods(time::time_point t, keyboard_id id, int raw, scancode sc, bool down);

//=============================================================================

bool keyboard_instance::send_key_and_scancode(time::time_point t, keyboard_id id, int raw, scancode sc, keycode key, bool down);

//=============================================================================

void keyboard_instance::send_unicode_key(char32_t c)
{

}

//=============================================================================

bool keyboard_instance::send_key_auto_release(scancode sc);

//=============================================================================

void keyboard_instance::release_auto_release_keys() {}

//=============================================================================

bool keyboard_instance::hardware_key_pressed();

} // namespace keyboard
} // namespace app
} // namespace vx