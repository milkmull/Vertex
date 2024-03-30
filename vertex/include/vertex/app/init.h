#pragma once

#include "error_code.h"

namespace vx {
namespace app {

bool init();
bool is_init();
void terminate();

using error_fn = void (*)(error_code code, const char* description);
void set_error_callback(error_fn callback);

}
}