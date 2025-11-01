#pragma once

#include <cstdint>

namespace vx {
namespace app {
namespace hint {

//=============================================================================
// hint type
//=============================================================================

using hint_t = uint64_t;

//=============================================================================
// helpers
//=============================================================================

template <hint_t hint>
struct hint_traits
{
    VX_STATIC_ASSERT(hint != hint, "invalid hint");
};

//=============================================================================

#define CREATE_HINT_DEF(name, id) enum : hint_t { name = id }

#define CREATE_HINT_TRAITS(name, dv) \
template <> \
struct hint_traits<name> \
{ \
    static constexpr hint_t hint = name; \
    static constexpr const char* default_value = dv; \
}

#define CREATE_HINT(name, id, value) CREATE_HINT_DEF(name, id); CREATE_HINT_TRAITS(name, value)

//=============================================================================
// hints
//=============================================================================

// Prevents screen saver activation while the video system is active.
CREATE_HINT(HINT_VIDEO_ALLOW_SCREEN_SAVER, 1, "0");

// Ensures window operations are synchronous (can be slower, useful for debugging).
CREATE_HINT(HINT_VIDEO_SYNC_WINDOW_OPERATIONS, 2, "0");

// Specifies default usable display bounds (for headless or debug setups).
CREATE_HINT(HINT_VIDEO_DISPLAY_USABLE_BOUNDS, 3, "0,0,0,0");

// Use raw keyboard input (bypass Windows message translation).
CREATE_HINT(HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD, 4, "0");

// Whether to create and manage a Windows message loop.
CREATE_HINT(HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP, 5, "1");

// Enable Windows-style menu mnemonics (Alt+key shortcuts).
CREATE_HINT(HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS, 6, "0");

// Enable a specific dpi configuration for windows ("unaware", "system", "permonitor")
CREATE_HINT(HINT_VIDEO_WINDOWS_DPI_AWARENESS, 7, ""); // default is no dpi change

// Allow interacting with window frame even when cursor is hidden (for games, etc.).
CREATE_HINT(HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN, 8, "1");

// Minimize window when losing focus (useful for exclusive fullscreen apps).
CREATE_HINT(HINT_VIDEO_WINDOW_MINIMIZE_ON_FOCUS_LOSS, 9, "0");

// Keep exclusive mode consistent when moving the window between monitors.
CREATE_HINT(HINT_VIDEO_WINDOW_MATCH_EXCLUSIVE_MODE_ON_MOVE, 10, "1");

// Automatically quit the app when the last window is closed.
CREATE_HINT(HINT_VIDEO_QUIT_ON_LAST_WINDOW_CLOSE, 11, "1");

//=============================================================================
// input / mouse / touch / pen hints
//=============================================================================

// Apply system scaling to relative mode (1 = enabled).
CREATE_HINT(HINT_MOUSE_RELATIVE_SYSTEM_SCALE, 12, "1");

// Keep cursor centered in relative mode.
CREATE_HINT(HINT_MOUSE_RELATIVE_MODE_CENTER, 13, "0");

// Emulate warp with relative motion events when warping is unavailable.
CREATE_HINT(HINT_MOUSE_EMULATE_WARP_WITH_RELATIVE, 14, "1");

// Generate mouse events from touch input.
CREATE_HINT(HINT_TOUCH_MOUSE_EVENTS, 15, "1");

// Generate touch events from mouse input.
CREATE_HINT(HINT_MOUSE_TOUCH_EVENTS, 16, "0");

// Generate mouse events from pen input.
CREATE_HINT(HINT_PEN_MOUSE_EVENTS, 17, "1");

// Generate touch events from pen input.
CREATE_HINT(HINT_PEN_TOUCH_EVENTS, 18, "0");

// Automatically capture mouse when button pressed inside window (Windows).
CREATE_HINT(HINT_MOUSE_AUTO_CAPTURE, 19, "1");

// Generate motion events when cursor is warped in relative mode.
CREATE_HINT(HINT_MOUSE_RELATIVE_WARP_MOTION, 20, "0");

// Keep cursor visible in relative mode.
CREATE_HINT(HINT_MOUSE_RELATIVE_CURSOR_VISIBLE, 21, "0");

// Integer motion mode (non-standard, experimental).
CREATE_HINT(HINT_MOUSE_INTEGER_MODE, 22, "0");

//=============================================================================

#define VX_HINT_GET_NAME(hint_) ::vx::app::hint::hint_traits<(hint_)>::hint
#define VX_HINT_GET_DEFAULT_VALUE(hint_) ::vx::app::hint::hint_traits<(hint_)>::default_value
#define HINT_AND_DEFAULT_VALUE(hint_) (hint_), VX_HINT_GET_DEFAULT_VALUE(hint_)

//=============================================================================

#undef CREATE_HINT_DEF
#undef CREATE_HINT_TRAITS
#undef CREATE_HINT

} // namespace hint
} // namespace app
} // namespace vx