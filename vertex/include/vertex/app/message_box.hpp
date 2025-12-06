#pragma once

#include <string>
#include <vector>

#include "vertex/app/id.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/math/color/types/color8.hpp"

#if defined(VX_APP_VIDEO_ENABLED)
    #include "vertex/app/video/window.hpp"
#endif

namespace vx {
namespace app {
namespace message_box {

/**
 * @brief Type of message box to display.
 *
 * Determines the icon and presentation style used by the operating system.
 */
enum class type
{
    error,
    warning,
    information,
    default_,
};

enum class button_layout
{
    left_to_right, // Buttons are ordered left-to-right (platform default on most systems).
    right_to_left, // Buttons are ordered right-to-left (used in some locales).
};

VX_FLAGS_DECLARE_BEGIN(default_button_keys){
    none = 0,
    return_ = VX_BIT(0),
    escape = VX_BIT(1),
    any = (return_ | escape)
} VX_FLAGS_DECLARE_END(default_button_keys)

    /**
 * @brief Identifier used to distinguish which button the user pressed.
 */
    using button_id = id_type;

struct button_config
{
    default_button_keys default_keys; // Which keyboard shortcuts activate this button (Return/Escape).
    button_id id;                     // Application-defined button ID returned to the caller.
    std::string text;                 // Text label displayed on the button.
};

/**
 * @brief Color indices for a message-box color scheme.
 */
struct color_type
{
    using underlying_type = int;

    enum : underlying_type
    {
        background,
        text,
        button_border,
        button_background,
        button_selected,
        _count
    };
};

/**
 * @brief Message box color scheme array.
 *
 * If supplied, the UI will attempt to honor the provided colors.  
 * If nullptr, system defaults are used.
 */
using color_scheme = math::color8[color_type::_count];

/**
 * @brief Full configuration for a message box.
 *
 * Provides complete control over message box appearance, behavior, and buttons.
 */
struct config
{
    type message_type = type::error;                     // Message-type icon and semantic meaning.
    button_layout layout = button_layout::left_to_right; // Button ordering direction.
    std::string title;                                   // Message box window title.
    std::string message;                                 // Main message body text.

    /**
     * @brief List of buttons to present.
     *
     * If empty, a default "OK" button is created automatically.
     */
    std::vector<button_config> buttons;

    /**
     * @brief Optional color scheme override.
     *
     * If nullptr, the platform default styling is used.
     */
    math::color8* color_scheme = nullptr;

    /**
     * @brief Optional parent window ID.
     *
     * If invalid_id, the box may be centered on the screen or handled by the OS default.
     */
    id_type parent_window = invalid_id;
};

} // namespace message_box

//=============================================================================
// create
//=============================================================================

/**
 * @brief Displays a fully configurable message box and waits for the user to respond.
 *
 * The call **blocks until the user closes the message box** by pressing one of the
 * configured buttons.  
 *
 * This function does **not** require the app or video subsystems to be initialized.
 * It is always safe to call at any point in the program's lifetime, including before
 * initialization or after shutdown.
 *
 * @param config Full configuration describing the message box.
 * @param button Optional output parameter that receives the ID of the button pressed.
 *               If nullptr, the result is ignored.
 *
 * @return true if the system was able to display a message box;  
 *         false if an error occurred before presentation.
 *
 * @note Button IDs are defined by the caller through message_box::button_config.
 * @note If no buttons are provided in config.buttons, an implicit "OK" button is used.
 */
VX_API bool show_message_box(
    const message_box::config& config,
    message_box::button_id* button);

/**
 * @brief Displays a simple message box with a type, title, and message.
 *
 * This is a convenience wrapper around show_message_box() that automatically
 * creates a single "OK" button and uses default styling.
 *
 * Like show_message_box(), this call **blocks until the user dismisses the box**  
 * and does **not** depend on the app or video subsystems being initialized.
 *
 * @param type     Message box type (error, warning, information, default).
 * @param title    Window title string.
 * @param message  Main message text.
 * @param parent_window Optional parent window ID (invalid_id = no parent).
 *
 * @return true if the message box was successfully shown, false otherwise.
 */
VX_API bool show_simple_message_box(
    message_box::type type,
    const std::string& title,
    const std::string& message,
    id_type parent_window = invalid_id);

} // namespace app
} // namespace vx
