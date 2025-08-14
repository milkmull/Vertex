#pragma once

#include "vertex/util/memory/friend_unique_ptr.hpp"

namespace vx {
namespace app {

template <typename T>
using owner_ptr = mem::friend_unique_ptr<T>;

} // namespace app
} // namespace vx