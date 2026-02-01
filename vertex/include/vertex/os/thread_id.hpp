#pragma once

#include <cstdint>

namespace vx {
namespace os {

using thread_id = uint64_t;
constexpr thread_id invalid_thread_id = thread_id{ 0 };

} // namespace os
} // namespace vx
