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

enum class type
{
    error,
    warning,
    information,
    default_,
};

enum class button_layout
{
    left_to_right,
    right_to_left,
};

VX_FLAGS_DECLARE_BEGIN(default_button_keys)
{
    none        = 0,
    return_     = VX_BIT(0),
    escape      = VX_BIT(1),
    any         = (return_ | escape)
}
VX_FLAGS_DECLARE_END(default_button_keys)

using button_id = id_type;

struct button_config
{
    default_button_keys default_keys;
    button_id id;
    std::string text;
};

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

using color_scheme = math::color8[color_type::_count];

struct config
{
    type message_type = type::error;
    button_layout layout = button_layout::left_to_right;
    std::string title;
    std::string message;
    std::vector<button_config> buttons;
    math::color8* color_scheme = nullptr;
    id_type parent_window = invalid_id;
};

} // namespace message_box

//=============================================================================
// create
//=============================================================================

VX_API bool show_message_box(
    const message_box::config& config,
    message_box::button_id* button
);

VX_API bool show_simple_message_box(
    message_box::type type,
    const std::string& title,
    const std::string& message,
    id_type parent_window = invalid_id
);

} // namespace app
} // namespace vx