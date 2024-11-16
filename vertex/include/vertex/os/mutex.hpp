#pragma once

#include <mutex>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

using mutex = std::mutex;
using lock_guard = std::lock_guard<mutex>;

} // namespace os
} // namespace vx
