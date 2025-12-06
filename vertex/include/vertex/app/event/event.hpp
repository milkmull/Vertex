#pragma once

#include "vertex/app/id.hpp"
#include "vertex/util/time.hpp"

#if defined(VX_APP_VIDEO_ENABLED)

    #include "vertex/app/input/keyboard.hpp"
    #include "vertex/app/input/mouse.hpp"
    #include "vertex/app/input/pen.hpp"
    #include "vertex/app/input/touch.hpp"
    #include "vertex/app/video/video.hpp"

#endif // VX_APP_VIDEO_ENABLED

namespace vx {
namespace app {
namespace event {

using event_type_t = uint32_t;

//=============================================================================
// event type
//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_events.h#L84

/**
 * @enum event_type
 * @brief Enumerates all supported event types in the application.
 *
 * Each event type corresponds to a specific category and carries
 * data relevant to that category. Events are delivered through
 * the event callback and must be processed promptly to keep
 * the application responsive.
 */
enum event_type : event_type_t
{
    event_last = 0xFFFF,
    event_first = 0,
    invalid_event = event_first,

    // app events
    app_quit,                  // Request to quit the application.
    app_terminating,           // Application is terminating, finalize state.
    app_low_memory,            // System reports low memory; release caches.
    app_will_enter_background, // App about to enter background; save state.
    app_did_enter_background,  // App entered background; pause activities.
    app_will_enter_foreground, // App about to enter foreground; prepare UI.
    app_did_enter_foreground,  // App entered foreground; resume activities.
    app_locale_changed,        // System locale/language changed.
    app_system_theme_changed,  // System theme changed (e.g., light/dark mode).

    _app_event_first = app_quit,
    _app_event_last = app_system_theme_changed,

    // display events
    display_added,                 // New display connected.
    display_removed,               // Display disconnected.
    display_moved,                 // Display position changed.
    display_orientation_changed,   // Display orientation changed.
    display_desktop_mode_changed,  // Desktop mode changed (e.g., resolution).
    display_current_mode_changed,  // Display current mode changed.
    display_content_scale_changed, // Display content scaling changed.

    _display_event_first = display_added,
    _display_event_last = display_content_scale_changed,

    // window events
    window_created,               // Window created.
    window_destroyed,             // Window destroyed.
    window_shown,                 // Window shown.
    window_hidden,                // Window hidden.
    window_exposed,               // Window exposed (needs redraw).
    window_occluded,              // Window occluded (hidden behind others).
    window_moved,                 // Window moved to new position.
    window_resized,               // Window resized.
    window_pixel_size_changed,    // Window pixel size changed (DPI).
    window_minimized,             // Window minimized.
    window_maximized,             // Window maximized.
    window_restored,              // Window restored from minimized/maximized.
    window_enter_fullscreen,      // Window entered fullscreen mode.
    window_leave_fullscreen,      // Window left fullscreen mode.
    window_gained_focus,          // Window gained keyboard focus.
    window_lost_focus,            // Window lost keyboard focus.
    window_mouse_enter,           // Mouse entered window.
    window_mouse_leave,           // Mouse left window.
    window_hit_test,              // Window hit test event.
    window_display_changed,       // Window moved to another display.
    window_display_scale_changed, // Display scale factor for window changed.
    window_safe_area_changed,     // Safe area for window changed.
    window_close_requested,       // Request to close the window.

    _window_event_first = window_created,
    _window_event_last = window_close_requested,

    // keyboard events
    keyboard_added,         // Keyboard device added.
    keyboard_removed,       // Keyboard device removed.
    keymap_changed,         // Keyboard mapping/layout changed.
    key_down,               // Key pressed.
    key_up,                 // Key released.
    screen_keyboard_shown,  // On-screen keyboard shown.
    screen_keyboard_hidden, // On-screen keyboard hidden.

    _keyboard_event_first = keyboard_added,
    _keyboard_event_last = key_up,

    // text events
    text_editing,            // Text being edited (IME).
    text_input,              // Text input from keyboard or IME.
    text_editing_candidates, // Candidates for text editing.

    _text_event_first = text_editing,
    _text_event_last = text_editing_candidates,

    // mouse events
    mouse_added,       // Mouse device added.
    mouse_removed,     // Mouse device removed.
    mouse_moved,       // Mouse moved.
    mouse_button_down, // Mouse button pressed.
    mouse_button_up,   // Mouse button released.
    mouse_wheel,       // Mouse wheel scrolled.

    _mouse_event_first = mouse_added,
    _mouse_event_last = mouse_wheel,

    // touch events
    finger_moved,    // Finger moved on touch device.
    finger_down,     // Finger touched down.
    finger_up,       // Finger lifted up.
    finger_canceled, // Touch canceled.

    _touch_event_first = finger_moved,
    _touch_event_last = finger_canceled,

    // pinch events
    pinch_begin,  // Pinch gesture started.
    pinch_update, // Pinch gesture updated.
    pinch_end,    // Pinch gesture ended.

    _pinch_event_first = pinch_begin,
    _pinch_event_last = pinch_end,

    // pen events
    pen_proximity_in,  // Pen entered proximity.
    pen_proximity_out, // Pen left proximity.
    pen_moved,         // Pen moved.
    pen_up,            // Pen lifted.
    pen_down,          // Pen touched down.
    pen_button_up,     // Pen button released.
    pen_button_down,   // Pen button pressed.
    pen_axis_changed,  // Pen axis value changed.

    _pen_event_first = pen_proximity_in,
    _pen_event_last = pen_axis_changed,

    // clipboard events
    clipboard_updated, // Clipboard contents updated.

    _clipboard_event_first = clipboard_updated,
    _clipboard_event_last = clipboard_updated,

    // drag and drop events
    drop_file,     // File dropped onto window.
    drop_text,     // Text dropped onto window.
    drop_begin,    // Drag operation started.
    drop_complete, // Drag operation completed.
    drop_position, // Drag position updated.

    _drop_event_first = drop_file,
    _drop_event_last = drop_position,

    // internal events (unchanged because there is no paired last event)
    internal_event_poll_sentinel,

    _internal_event_first = internal_event_poll_sentinel,
    _internal_event_last = internal_event_poll_sentinel,

    // user event (custom user-defined events start here)
    user_event = 0x8000 // Base value for user-defined events.
};

//=============================================================================
// event category
//=============================================================================

/**
 * @enum event_category
 * @brief Categories grouping related event types.
 */
enum event_category
{
    category_none,      // No category or invalid.
    category_app,       // Application-level events.
    category_display,   // Display-related events.
    category_window,    // Window events.
    category_keyboard,  // Keyboard input events.
    category_text,      // Text input and editing events.
    category_mouse,     // Mouse input events.
    category_touch,     // Touch input events.
    category_pinch,     // Pinch gesture events.
    category_pen,       // Pen/stylus input events.
    category_clipboard, // Clipboard update events.
    category_drop,      // Drag and drop events.
    category_internal,  // Internal system events.
    category_user       // User-defined custom events.
};

/**
 * @brief Returns the category corresponding to a given event type.
 *
 * @param type Event type.
 * @return event_category The category the event belongs to.
 */
inline constexpr event_category get_category(event_type_t type) noexcept
{
    if (type > event_last)
    {
        return category_none;
    }
    if (type >= user_event)
    {
        return category_user;
    }
    if (type >= _internal_event_first && type <= _internal_event_last)
    {
        return category_internal;
    }
    if (type >= _drop_event_first && type <= _drop_event_last)
    {
        return category_drop;
    }
    if (type >= _clipboard_event_first && type <= _clipboard_event_last)
    {
        return category_clipboard;
    }
    if (type >= _pen_event_first && type <= _pen_event_last)
    {
        return category_pen;
    }
    if (type >= _pinch_event_first && type <= _pinch_event_last)
    {
        return category_pinch;
    }
    if (type >= _touch_event_first && type <= _touch_event_last)
    {
        return category_touch;
    }
    if (type >= _mouse_event_first && type <= _mouse_event_last)
    {
        return category_mouse;
    }
    if (type >= _text_event_first && type <= _text_event_last)
    {
        return category_text;
    }
    if (type >= _keyboard_event_first && type <= _keyboard_event_last)
    {
        return category_keyboard;
    }
    if (type >= _window_event_first && type <= _window_event_last)
    {
        return category_window;
    }
    if (type >= _display_event_first && type <= _display_event_last)
    {
        return category_display;
    }
    if (type >= _app_event_first && type <= _app_event_last)
    {
        return category_app;
    }

    return category_none;
}

//=============================================================================
// app events
//=============================================================================

struct app_event_common
{
};

#if defined(VX_APP_VIDEO_ENABLED)

struct system_theme_changed_event
{
    video::system_theme system_theme;
};

#endif // VX_APP_VIDEO_ENABLED

struct app_event_type
{
    app_event_common common;

    // keep this outside the union until we have other events inside
#if defined(VX_APP_VIDEO_ENABLED)

    union
    {
        system_theme_changed_event system_theme_changed; // New system theme (e.g., light/dark).
    };

#endif // VX_APP_VIDEO_ENABLED
};

#if defined(VX_APP_VIDEO_ENABLED)

//=============================================================================
// display events
//=============================================================================

struct display_event_common
{
    video::display_id display_id; // Display identifier.
};

struct display_orientation_changed_event
{
    video::display_orientation orientation; // New orientation.
};

struct display_content_scale_changed_event
{
    float x, y; // New content scale factors in X and Y.
};

struct display_event_type
{
    display_event_common common;

    union
    {
        display_orientation_changed_event display_orientation_changed;
        display_content_scale_changed_event display_content_scale_changed;
    };
};

//=============================================================================
// window events
//=============================================================================

struct window_event_common
{
    video::window_id window_id; // Window identifier.
};

struct window_moved_event
{
    int32_t x, y; // New window position.
};

struct window_resized_event
{
    int32_t w, h; // New window size.
};

struct window_pixel_size_changed_event
{
    int32_t w, h; // New window pixel size (DPI scaled).
};

struct window_display_changed_event
{
    video::display_id display_id; // New display ID.
};

struct window_display_scale_changed_event
{
    float xscale, yscale; // New scale factors for X and Y.
};

struct window_safe_area_changed_event
{
    int32_t x, y, w, h; // Safe area rectangle within the window.
};

struct window_event_type
{
    window_event_common common;

    union
    {
        window_moved_event window_moved;
        window_resized_event window_resized;
        window_pixel_size_changed_event window_pixel_size_changed;
        window_display_changed_event window_display_changed;
        window_display_scale_changed_event window_display_scale_changed;
        window_safe_area_changed_event window_safe_area_changed;
    };
};

//=============================================================================
// keyboard events
//=============================================================================

struct keyboard_event_common
{
    keyboard::keyboard_id keyboard_id; // Keyboard device ID.
    video::window_id window_id;        // Window receiving keyboard input.
};

struct key_event
{
    keyboard::scancode scancode; // Physical key location.
    keyboard::keycode key;       // Virtual key code.
    keyboard::key_mod mods;      // Modifier keys held.
    uint16_t raw;                // Raw hardware scancode.
    bool repeat;                 // Whether this is a repeat event.
    bool down;                   // True if key down, false if key up.
};

struct keyboard_event_type
{
    keyboard_event_common common;

    union
    {
        key_event key;
    };
};

//=============================================================================
// text events
//=============================================================================

struct text_event_common
{
    video::window_id window_id; // Window receiving text input.
};

struct text_input_event
{
    const char* text; // UTF-8 input text.
};

struct text_editing_event
{
    const char* text; // Editing text buffer.
    size_t start;     // Start of editing selection.
    size_t length;    // Length of editing selection.
};

struct text_editing_candidates_event
{
    const char* const* candidates; // Array of candidate strings.
    size_t count;                  // Number of candidates.
    size_t selected;               // Selected candidate index.
    bool horizontal;               // Whether candidate list is horizontal.
};

struct text_event_type
{
    text_event_common common;

    union
    {
        text_input_event text_input;
        text_editing_event text_editing;
        text_editing_candidates_event text_editing_candidates;
    };
};

//=============================================================================
// mouse events
//=============================================================================

struct mouse_event_common
{
    mouse::mouse_id mouse_id;   // Mouse device ID.
    video::window_id window_id; // Window receiving mouse input.
    float x, y;                 // Position relative to window.
};

struct mouse_moved_event
{
    float dx, dy; // Movement delta.
};

struct mouse_button_event
{
    uint8_t button; // Mouse button number.
    uint8_t clicks; // Number of clicks (for double-click detection).
    bool down;      // True if button down, false if up.
};

struct mouse_wheel_event
{
    mouse::wheel_direction direction; // Scroll direction.
    float x, y;                       // Scroll amount (floating point).
    int ix, iy;                       // Scroll amount (integer).
};

struct mouse_event_type
{
    mouse_event_common common;

    union
    {
        mouse_moved_event mouse_moved;
        mouse_button_event mouse_button;
        mouse_wheel_event mouse_wheel;
    };
};

//=============================================================================
// touch events
//=============================================================================

struct touch_event_common
{
    touch::touch_id touch_id;   // Touch device ID.
    touch::finger_id finger_id; // Finger ID.
    float x, y;                 // Position relative to window.
    float pressure;             // Touch pressure.
    video::window_id window_id; // Window receiving touch input.
};

struct finger_moved_event
{
    float dx, dy;              // Movement delta.
    float pressure_difference; // Change in pressure.
};

struct touch_event_type
{
    touch_event_common common;

    union
    {
        finger_moved_event finger_moved;
    };
};

//=============================================================================
// pinch events
//=============================================================================

struct pinch_event_common
{
    float scale;                // Pinch scale factor.
    video::window_id window_id; // Window receiving pinch input.
};

struct pinch_event_type
{
    pinch_event_common common;
};

//=============================================================================
// pen events
//=============================================================================

struct pen_event_common
{
    pen::pen_id pen_id;         // Pen device ID.
    float x, y;                 // Position relative to window.
    pen::input_state state;     // Pen input state.
    video::window_id window_id; // Window receiving pen input.
};

struct pen_touch_event
{
    bool eraser; // True if pen is using eraser tip.
};

struct pen_button_event
{
    uint8_t button; // Pen button number.
};

struct pen_axis_changed_event
{
    pen::axis_type axis; // Axis type (e.g., pressure, tilt).
    float value;         // Axis value.
};

struct pen_event_type
{
    pen_event_common common;

    union
    {
        pen_touch_event pen_touch;
        pen_button_event pen_button;
        pen_axis_changed_event pen_axis_changed;
    };
};

//=============================================================================
// clipboard events
//=============================================================================

struct clipboard_updated_event
{
    bool owner;              // True if this app owns clipboard.
    const char** mime_types; // Supported MIME types.
    size_t mime_type_count;  // Number of MIME types.
};

struct clipboard_event_type
{
    union
    {
        clipboard_updated_event clipboard_updated;
    };
};

#endif // VX_APP_VIDEO_ENABLED

//=============================================================================
// drop events
//=============================================================================

struct drop_event_common
{
    float x, y; // Drop position.

#if defined(VX_APP_VIDEO_ENABLED)

    video::window_id window_id; // Window receiving drop.

#endif // VX_APP_VIDEO_ENABLED
};

struct drop_file_event
{
    const char* source; // Source of the drop.
    const char* file;   // Path of the dropped file.
};

struct drop_text_event
{
    const char* text; // Dropped text.
};

struct drop_event_type
{
    drop_event_common common;

    union
    {
        drop_file_event drop_file;
        drop_text_event drop_text;
    };
};

//=============================================================================
// user events
//=============================================================================

struct user_event_type
{
    void* user_data; // Pointer to user data.
};

//=============================================================================
// event
//=============================================================================

/**
 * @struct event
 * @brief Generic event structure holding any type of event data.
 *
 * Use the `type` field to determine the event category and interpret
 * the union accordingly. The `time` field represents the timestamp
 * when the event was generated.
 */
struct event
{
public:

    //=============================================================================
    // event data
    //=============================================================================

    event_type_t type = invalid_event; // Event type identifier.
    time::time_point time;           // Event generation time.

    union
    {
        // app events
        app_event_type app_event{};

#if defined(VX_APP_VIDEO_ENABLED)

        // display events
        display_event_type display_event;

        // window events
        window_event_type window_event;

        // keyboard events
        keyboard_event_type keyboard_event;

        // text events
        text_event_type text_event;

        // mouse events
        mouse_event_type mouse_event;

        // touch events
        touch_event_type touch_event;

        // pinch events
        pinch_event_type pinch_event;

        // pen events
        pen_event_type pen_event;

        // clipboard events
        clipboard_event_type clipboard_event;

#endif // VX_APP_VIDEO_ENABLED

        // drop events
        drop_event_type drop_event;

        // user event
        user_event_type user_event;

        // padding (ensure union size is always the same)
        alignas(4) uint8_t _padding[48]; // aligned to 4 bytes, size rounded to 48 bytes
    };
};

//=============================================================================
// user events
//=============================================================================

/**
 * @brief Reserves a contiguous range of user-defined event types.
 * 
 * This function allocates a block of `count` user event IDs from the static pool of available user events.
 * It can be called at any time during runtime to reserve user events dynamically.
 * 
 * @param count The number of consecutive user event IDs to reserve.
 * 
 * @return The starting event_type_t ID of the reserved range if successful.
 *         Returns `invalid_event` (0) if:
 *          - `count` is zero,
 *          - or there are not enough user events left to fulfill the request.
 * 
 * @note The reserved range is `[base, base + count)`. It is the caller's responsibility
 *       to ensure no overlapping usage of reserved ranges.
 * 
 * @note The internal tracking of user events uses an atomic variable, making this function
 *       safe to call concurrently from multiple threads without external synchronization.
 */
VX_API event_type_t register_user_events(size_t count);

/**
 * @brief Checks if an event type is a user event.
 *
 * @param type Event type to check.
 * @return true if the event is a user event, false otherwise.
 */
inline constexpr bool is_user_event(event_type_t type) noexcept
{
    return (user_event <= type) && (type < event_last);
}

//=============================================================================
// processing
//=============================================================================

/**
 * @brief Pumps the event system to process and queue new events.
 *
 * This function is the heartbeat of the application's event handling.
 * It queries and updates the internal state of all active subsystems,
 * including OS-level input, window changes, and other event sources,
 * then enqueues any generated events for the application to consume.
 *
 * Note that `pump_events()` does NOT return any events directly; it only
 * updates the internal event queue.
 *
 * - Functions like `poll_event()` and `wait_event()` call this internally
 *   to ensure the queue is up to date before returning events.
 * - If you are performing custom event filtering, dispatching, or
 *   processing without calling poll or wait functions, you must call
 *   `pump_events()` regularly to keep events flowing.
 *
 * Failing to call this function (or indirectly calling poll/wait) will
 * result in your application not receiving new input or system events.
 *
 * @note This function should only be called from the thread that initialized
 * the video subsystem, as it interacts with OS-level window and input events
 * that are thread-bound.
 */
VX_API void pump_events();

//=============================================================================

/**
 * @brief Event filter callback type.
 *
 * Return true to keep the event, false to discard it.
 */
using event_filter = bool (*)(event&, void*);

//=============================================================================

/**
 * @brief Matches and optionally copies or removes events from the event queue.
 *
 * This function scans the internal event queue and matches events using
 * a user-provided filter callback.
 *
 * @param matcher A callback function used to test each event. If `nullptr`,
 *                all events are matched.
 * @param user_data A pointer passed to the matcher function for context.
 * @param events If non-null, matched events are copied into this buffer.
 * @param count The maximum number of matched events to process.
 *              If zero, all matching events in the queue will be processed.
 * @param remove If true, matched events are removed from the queue.
 * @return The number of matched events processed.
 *
 * @remarks
 * - If `events` is non-null and `count` > 0, matched events will be copied
 *   to the `events` array up to `count`.
 * - If `events` is null but `count` is non-zero, the function will iterate
 *   through events and count matches up to `count` without copying.
 * - If `count` is zero, the function will process all matching events.
 * - To copy all events, first call this function with `events` null and
 *   `count` 0 to get the total count, then allocate a buffer and call again
 *   with that buffer and count.
 * - Thread-safe: internally locks the event queue during iteration and modification.
 */
VX_API size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove);

/**
 * @brief Helper matcher to compare event type.
 *
 * @param e Event.
 * @param user_data Pointer to event_type to match.
 * @return true if event type matches.
 */
inline bool type_matcher(event& e, void* user_data) noexcept
{
    const event_type_t type = *reinterpret_cast<event_type_t*>(user_data);
    return e.type == type;
}

/**
 * @brief Helper matcher to compare event category.
 *
 * @param e Event.
 * @param user_data Pointer to event_category to match.
 * @return true if event category matches.
 */
inline bool category_matcher(const event& e, void* user_data) noexcept
{
    const event_category category = *reinterpret_cast<event_category*>(user_data);
    return get_category(e.type) == category;
}

//=============================================================================

/**
 * @brief Checks if there are events matching a filter in the event queue.
 *
 * This function inspects the event queue for events that satisfy the given filter
 * without removing them.
 *
 * @param matcher Filter callback to test events. If `nullptr`, matches all events.
 * @param user_data User data passed to the filter callback.
 * @return true if one or more matching events exist in the event queue.
 */
inline bool has_events(event_filter matcher, void* user_data = nullptr)
{
    return match_events(matcher, user_data, nullptr, 0, false) > 0;
}

/**
 * @brief Checks if any events of a specific type exist in the event queue.
 *
 * This function queries the event queue for the presence of events matching
 * the specified event type without removing them.
 *
 * @param type Event type to check for.
 * @return true if at least one event of the given type exists in the event queue.
 */
inline bool has_event(event_type_t type)
{
    return has_events(type_matcher, &type);
}

//=============================================================================

/**
 * @brief Retrieves events matching the filter without removing them from the queue.
 *
 * This function scans the event queue and copies matched events into the
 * provided buffer without removing them from the queue.
 *
 * @param matcher A callback function used to test each event. If `nullptr`,
 *                all events are matched.
 * @param user_data A pointer passed to the matcher function for context.
 * @param out Buffer to receive matched events. Must have capacity for at least `count` events.
 *            If `nullptr`, no events are copied but the function still counts matches.
 * @param count Maximum number of events to copy or count. If zero, all matching events are processed.
 * @return The number of matched events copied or counted.
 *
 * @remarks
 * - Events remain in the queue; this function only copies matching events.
 * - To count matching events without copying, pass `out` as `nullptr`.
 * - If `count` is zero, all matching events in the queue are processed.
 * - For copying all events, call once with `out` as `nullptr` and `count` 0 to get the total,
 *   then allocate a buffer and call again with the buffer and count.
 * - This function is thread-safe.
 */
inline size_t peek_events(event_filter matcher, void* user_data, event* out, size_t count)
{
    return match_events(matcher, user_data, out, count, false);
}

//=============================================================================

/**
 * @brief Retrieves and removes events matching the filter from the event queue.
 *
 * This function scans the event queue, copies matched events into the provided buffer,
 * and removes them from the queue.
 *
 * @param matcher A callback function used to test each event. If `nullptr`,
 *                all events are matched.
 * @param user_data A pointer passed to the matcher function for context.
 * @param out Buffer to receive matched events. Must have capacity for at least `count` events.
 *            If `nullptr`, no events are copied but matched events are removed.
 * @param count Maximum number of events to copy/remove. If zero, all matching events are processed.
 * @return The number of matched events copied/removed.
 *
 * @remarks
 * - Matched events are removed from the queue.
 * - To remove matching events without copying, pass `out` as `nullptr`.
 * - If `count` is zero, all matching events in the queue are removed.
 * - For removing all events, you can first call to count, then allocate buffer and call again,
 *   or call directly with `out` as `nullptr` and `count` 0 to remove all matching events.
 * - This function is thread-safe.
 */
inline size_t get_events(event_filter matcher, void* user_data, event* out, size_t count)
{
    return match_events(matcher, user_data, out, count, true);
}

/**
 * @brief Retrieves and removes a single event of the specified type from the event queue.
 *
 * This function searches the event queue for one event matching the given type,
 * copies it into the provided event reference, and removes it from the queue.
 *
 * @param e Reference to an event object to be filled with the matched event data.
 * @param type The event type to search for in the event queue.
 * @return true if an event of the specified type was found and removed; false otherwise.
 */
inline bool get_event(event& e, event_type_t type)
{
    return get_events(type_matcher, &type, &e, 1) == 1;
}

//=============================================================================

/**
 * @brief Removes all events from the event queue matching the given filter.
 *
 * This function iterates over the event queue and removes all events
 * for which the provided filter callback returns true.
 *
 * @param matcher Filter callback used to select events for removal.
 * @param user_data User data passed to the filter callback.
 * @return The number of events removed from the event queue.
 */
inline size_t flush_events(event_filter matcher, void* user_data)
{
    return match_events(matcher, user_data, nullptr, 0, true);
}

/**
 * @brief Removes all events of the specified type from the event queue.
 *
 * This function removes every event from the queue that matches the given event type.
 *
 * @param type The event type to remove from the event queue.
 */
inline void flush_events(event_type_t type)
{
    flush_events(type_matcher, &type);
}

//=============================================================================

/**
 * @brief Waits for the next event up to a specified timeout, optionally removing it from the queue.
 *
 * This function blocks until either an event becomes available or the specified timeout expires.
 * If the pointer `e` is not null, the event data will be copied to `e` and removed from the event queue.
 * If `e` is null, the function simply waits for an event without removing it.
 *
 * Internally, this function pumps events if no events are immediately available, and then waits,
 * sleeping as necessary until an event arrives or the timeout occurs.
 *
 * @param e Pointer to an event struct to receive the event data, or nullptr to only wait without removing.
 * @param t Timeout time point to wait until. Use time::max() for an indefinite wait.
 * @return true if an event was received within the timeout, false if timed out without an event.
 *
 * @note This function should only be called from the thread that initialized the video subsystem,
 *       since it may interact with OS-level components requiring thread affinity.
 */
VX_API bool wait_event_timeout(event* e, time::time_point t);

/**
 * @brief Waits indefinitely for the next event, optionally removing it from the queue.
 *
 * Equivalent to calling wait_event_timeout() with an infinite timeout.
 *
 * @param e Pointer to an event struct to receive the event data, or nullptr to only wait without removing.
 * @return true if an event was received, false if the wait was interrupted (rare).
 *
 * @note This function should only be called from the thread that initialized the video subsystem,
 *       due to potential OS-level thread affinity requirements.
 */
inline bool wait_event(event* e)
{
    return wait_event_timeout(e, time::max());
}

//=============================================================================

/**
 * @brief Adds multiple events directly to the event queue without any filtering or event watch callbacks.
 *
 * This function inserts the provided events straight into the internal event queue.
 * No event filters or event watch callbacks are invoked.
 *
 * Use this function if you need to batch-insert events bypassing the usual event processing pipeline.
 *
 * @param e Pointer to an array of events to add.
 * @param count Number of events to add.
 * @return Number of events successfully added.
 */
VX_API size_t add_events(const event* e, size_t count);

/**
 * @brief Pushes a single event into the event queue, invoking event filters and watches.
 *
 * This function inserts the event into the queue, but unlike add_events, it processes
 * the event through the standard event pipeline, including calling event filters and event watch callbacks.
 *
 * Note that pushing input events like mouse or keyboard events through this function
 * does NOT modify the internal system state — it simply queues the event.
 *
 * @param e The event to push into the queue.
 * @return true if the event was accepted and queued, false otherwise.
 */
VX_API bool push_event(event& e);

/**
 * @brief Polls for the next event without blocking, optionally removing it from the queue.
 *
 * This function checks if there is an event available in the queue.
 * If the pointer `e` is not null, the next event will be copied into `e` and removed from the queue.
 * If `e` is null, the function only checks if any events are available without removing any.
 *
 * Returns true if an event is available (and copied if `e` is not null), or false if no events are in the queue.
 *
 * Internally, this function implicitly pumps events to update the event queue.
 *
 * @param e Pointer to receive the next event, or nullptr to only check for event availability.
 * @return true if an event is available, false if none are available.
 *
 * @note This function should only be called from the thread that initialized the video subsystem,
 *       as it may call pump_events() which interacts with OS-level components requiring thread affinity.
 *
 * @note On some operating systems (notably Windows), polling for events may block during window resizing
 *       or dragging operations. This can cause your main loop to freeze temporarily, preventing video frames,
 *       game logic, or networking from updating.
 *
 *       To mitigate this, this engine's preferred setup is to use callback-based event handling (e.g., init and iterate callbacks)
 *       that the OS can call into during blocking operations, ensuring your application stays responsive.
 */
VX_API bool poll_event(event* e);

//=============================================================================

/**
 * @brief Sets a global event filter callback.
 *
 * The event filter is called for every event before it is added to the event queue.
 * If the filter returns false, the event will be dropped and not added to the queue.
 * When a new filter is set, it immediately filters the existing events in the queue,
 * removing any that do not pass the filter, and then applies to all future incoming events.
 *
 * If no filter is set (nullptr), all events are allowed by default.
 *
 * @param filter Function pointer to the event filter callback.
 * @param user_data User-defined data pointer passed to the filter callback.
 *
 * @note Setting an event filter affects all subsequent event processing,
 *       so it can be used to block or modify incoming events globally.
 *       Filters should be lightweight to avoid performance issues.
 */
VX_API void set_event_filter(event_filter filter, void* user_data);

/**
 * @brief Retrieves the current global event filter and user data.
 *
 * @param filter Reference to receive the current event filter callback.
 * @param user_data Reference to receive the current user data pointer.
 *
 * @note If no filter is set, `filter` will be nullptr and `user_data` will be nullptr.
 */
VX_API void get_event_filter(event_filter& filter, void*& user_data);

/**
 * @brief Clears the global event filter, allowing all events to be processed.
 *
 * Equivalent to calling `set_event_filter(nullptr, nullptr)`.
 */
inline void clear_event_filter()
{
    set_event_filter(nullptr, nullptr);
}

//=============================================================================

/**
 * @brief Adds an event watch callback that is called for every event processed by the system.
 *
 * The event watch callback is invoked **after** the global event filter has processed the event,
 * and only for events that pass the filter.
 *
 * Watches are useful for logging, debugging, or intercepting events globally without
 * modifying the event queue.
 *
 * @param callback Function pointer to the event watch callback.
 * @param user_data User-defined data pointer passed to the callback.
 *
 * @note The watch callback **may be called from a different thread** than the main event thread,
 *       so users must ensure thread safety when accessing shared resources.
 *
 * @note Events generated by signals (e.g., SIGINT quit event) bypass the event queue and
 *       are delivered immediately to the watch callback, then arrive at the next event poll.
 *
 * @note The event watch callback will **not** be called for events added directly via `add_events`.
 *       It only observes events going through the standard event push/filter process.
 *
 * @note The main event filter runs first and can block events; events filtered out
 *       will not be sent to the watch callbacks.
 */
VX_API void add_event_watch(event_filter callback, void* user_data);

/**
 * @brief Removes a previously added event watch callback.
 *
 * @param callback Function pointer to the event watch callback to remove.
 * @param user_data User-defined data pointer associated with the callback.
 *
 * @note Both callback and user_data must match the pair used when adding the watch.
 */
VX_API void remove_event_watch(event_filter callback, void* user_data);

} // namespace event
} // namespace app
} // namespace vx
