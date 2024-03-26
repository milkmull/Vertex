#pragma once

#include "vertex/app/init.h"

namespace vx {
namespace app {

#define VX_REQUIRE_INIT() if (!is_video_init()) { throw_error(::vx::app::error_code::NOT_INITIALIZED); return; }
#define VX_REQUIRE_INIT_OR_RETURN(x) if (!is_video_init()) { throw_error(::vx::app::error_code::NOT_INITIALIZED); return x; }

bool init_video();
bool is_video_init();
void terminate_video();

void throw_error(error_code code);

}
}