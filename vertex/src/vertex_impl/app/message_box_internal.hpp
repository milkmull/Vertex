#pragma once

#include "vertex/app/message_box.hpp"
#include "vertex_impl/app//app_internal.hpp"

namespace vx {
namespace app {

size_t message_box_count();

bool show_message_box_internal(
    app_instance* this_,
    const message_box::config& config,
    message_box::button_id* button
);

bool show_simple_message_box_internal(
    app_instance* this_,
    message_box::type type,
    const std::string& title,
    const std::string& message,
    id_type parent_window
);

} // namespace app
} // namespace vx