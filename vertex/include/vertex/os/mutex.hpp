#pragma once

#include <mutex>

#include "vertex/core/compiler.hpp"

namespace vx {
namespace os {

using mutex = std::mutex;
using lock_guard = std::lock_guard<mutex>;

} // namespace os
} // namespace vx
