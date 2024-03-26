#pragma once

#if defined(VX_APPLICATION)

#include "error_code.h"

namespace vx {
namespace app {

bool init();
bool is_init();
void terminate();

using vx_error_fn = void (*)(error_code code);
void set_error_callback(vx_error_fn callback);

}
}

#endif