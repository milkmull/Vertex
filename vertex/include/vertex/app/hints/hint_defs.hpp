#pragma once

#include <cstdint>

namespace vx {
namespace app {
namespace hint {

//=============================================================================
// hint type
//=============================================================================

using hint_t = uint32_t;

//=============================================================================
// hints
//=============================================================================

enum hint : hint_t
{
    //=============================================================================
    // Application Behavior Hints
    //=============================================================================

    /**
     * @brief Main callback rate in Hertz (Hz) for the app's update loop.
     */
    app_main_callback_rate,

    /**
     * @brief Disable installation of default signal handlers (e.g., SIGINT).
     */
    app_no_signal_handlers,

    /**
     * @brief Enables the poll-sentinel mechanism to prevent infinite polling loops.
     *
     * When polling events, continuous high-frequency input devices (such as a
     * rapidly moving mouse) may generate new events between pump cycles. This can
     * cause a polling loop to never terminate because each pump produces more
     * events before the loop finishes consuming them.
     *
     * When this hint is enabled (default), the event pump inserts an internal
     * sentinel marking the end of the current batch of device events. Poll loops
     * can detect this sentinel, ensuring they always terminate even under heavy
     * input.
     *
     * Disabling this hint removes the sentinel, placing full responsibility on
     * the application to avoid infinite poll loops.
     */
    events_enable_poll_sentinel,

    //=============================================================================
    // video hints
    //=============================================================================

    /**
     * @brief Allow the operating system screen saver to activate while the video subsystem is active.
     *
     * By default, the application suppresses screen saver activation whenever the
     * video subsystem is initialized or windows are present. This ensures that
     * rendering, playback, and interactive applications (such as games) are not
     * interrupted.
     *
     * Enabling this hint permits the system screen saver to activate normally.
     * This may be useful for non-interactive display applications or tools that
     * should not inhibit system idle behavior.
     *
     * This should be set before the video subsystem is initialized.
     */
    video_allow_screen_saver,

    /**
     * @brief Makes window operations synchronous (may reduce performance but useful for debugging).
     */
    video_sync_window_operations,

    /**
     * @brief Specifies default usable display bounds (for headless or debug environments).
     */
    video_display_usable_bounds,

    /**
     * @brief Enables raw keyboard input on Windows, bypassing message translation.
     */
    video_windows_use_raw_keyboard,

    /**
     * @brief Enable or disable the creation and management of the Windows message loop.
     */
    video_windows_enable_message_loop,

    /**
     * @brief Enable Windows-style menu mnemonics (Alt+key shortcuts).
     */
    video_windows_enable_menu_mnemonics,

    /**
     * @brief Windows DPI awareness setting: "unaware", "system", or "permonitor".
     * Default is no DPI awareness change.
     */
    video_windows_dpi_awareness,

    /**
     * @brief Allow interaction with the window frame even when the cursor is hidden
     * (useful for games or immersive applications).
     */
    video_window_frame_usable_while_cursor_hidden,

    /**
     * @brief Minimize the window automatically when it loses focus (useful for exclusive fullscreen apps).
     */
    video_window_minimize_on_focus_loss,

    /**
     * @brief Keep exclusive fullscreen mode consistent when moving the window between monitors.
     */
    video_window_match_exclusive_mode_on_move,

    /**
     * @brief Automatically quit the application when the last window is closed.
     */
    video_quit_on_last_window_close,

    //=============================================================================
    // Input: Mouse, Touch, Pen Hints
    //=============================================================================

    /**
     * @brief Maximum time (ms) between two clicks to register a double click.
     */
    mouse_double_click_time_ms,

    /**
     * @brief Maximum distance (pixels) between two clicks to count as a double click.
     */
    mouse_double_click_radius,

    /**
     * @brief Scale factor applied to normal mouse movement speed.
     */
    mouse_normal_speed_scale,

    /**
     * @brief Scale factor applied to mouse movement speed in relative mode.
     */
    mouse_relative_speed_scale,

    /**
     * @brief Apply system display scaling to relative mouse mode (1 = enabled).
     */
    mouse_relative_system_scale,

    /**
     * @brief Keep cursor centered when in relative mouse mode.
     */
    mouse_relative_mode_center,

    /**
     * @brief Emulate mouse warp with relative motion events if warping is unavailable.
     */
    mouse_emulate_warp_with_relative,

    /**
     * @brief Generate mouse events from touch input.
     */
    touch_mouse_events,

    /**
     * @brief Generate touch events from mouse input.
     */
    mouse_touch_events,

    /**
     * @brief Generate mouse events from pen input.
     */
    pen_mouse_events,

    /**
     * @brief Generate touch events from pen input.
     */
    pen_touch_events,

    /**
     * @brief Automatically capture mouse input when a button is pressed inside the window (Windows only).
     */
    mouse_auto_capture,

    /**
     * @brief Generate motion events when the cursor is warped in relative mode.
     */
    mouse_relative_warp_motion,

    /**
     * @brief Keep the cursor visible when in relative mode.
     */
    mouse_relative_cursor_visible,

    /**
     * @brief Enable integer motion mode for mouse input.
     */
    mouse_integer_mode,

    /**
     * @brief Default system cursor ID to use.
     */
    mouse_default_system_cursor,

    //=============================================================================
    // Input: Keyboard Hints
    //=============================================================================

    /**
     * @brief Keyboard keycode options to customize keycode handling.
     */
    keyboard_keycode_options,

    /**
     * @brief Allow Alt+Tab while the keyboard is grabbed (focused exclusively).
     */
    keyboard_allow_alt_tab_while_grabbed,

    /**
     * @brief Enable on-screen keyboard support.
     */
    keyboard_enable_screen_keyboard
};

} // namespace hint
} // namespace app
} // namespace vx
