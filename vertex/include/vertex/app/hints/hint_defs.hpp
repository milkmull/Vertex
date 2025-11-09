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
// hints
//=============================================================================

enum hint : hint_t
{
    // Prevents screen saver activation while the video system is active.
    video_allow_screen_saver,

    // Ensures window operations are synchronous (can be slower, useful for debugging).
    video_sync_window_operations,

    // Specifies default usable display bounds (for headless or debug setups).
    video_display_usable_bounds,

    // Use raw keyboard input (bypass Windows message translation).
    video_windows_use_raw_keyboard,

    // Whether to create and manage a Windows message loop.
    video_windows_enable_message_loop,

    // Enable Windows-style menu mnemonics (Alt+key shortcuts).
    video_windows_enable_menu_mnemonics,

    // Enable a specific dpi configuration for windows ("unaware", "system", "permonitor")
    video_windows_dpi_awareness, // default is no dpi change

    // Allow interacting with window frame even when cursor is hidden (for games, etc.).
    video_window_frame_usable_while_cursor_hidden,

    // Minimize window when losing focus (useful for exclusive fullscreen apps).
    video_window_minimize_on_focus_loss,

    // Keep exclusive mode consistent when moving the window between monitors.
    video_window_match_exclusive_mode_on_move,

    // Automatically quit the app when the last window is closed.
    video_quit_on_last_window_close,

    //=============================================================================
    // input / mouse / touch / pen hints
    //=============================================================================

    // Maximum time interval (in milliseconds) between two clicks to register as a double click.
    mouse_double_click_time,

    // Maximum distance (in pixels) between two clicks to count as a double click.
    mouse_double_click_radius,

    // Scale factor for normal mouse movement speed.
    mouse_normal_speed_scale,

    // Scale factor for mouse movement speed in relative mode.
    mouse_relative_speed_scale,

    // Apply system scaling to relative mode (1 = enabled).
    mouse_relative_system_scale,

    // Keep cursor centered in relative mode.
    mouse_relative_mode_center,

    // Emulate warp with relative motion events when warping is unavailable.
    mouse_emulate_warp_with_relative,

    // Generate mouse events from touch input.
    touch_mouse_events,

    // Generate touch events from mouse input.
    mouse_touch_events,

    // Generate mouse events from pen input.
    pen_mouse_events,

    // Generate touch events from pen input.
    pen_touch_events,

    // Automatically capture mouse when button pressed inside window (Windows).
    mouse_auto_capture,

    // Generate motion events when cursor is warped in relative mode.
    mouse_relative_warp_motion,

    // Keep cursor visible in relative mode.
    mouse_relative_cursor_visible,

    // Integer motion mode (non-standard, experimental).
    mouse_integer_mode
};

} // namespace hint
} // namespace app
} // namespace vx