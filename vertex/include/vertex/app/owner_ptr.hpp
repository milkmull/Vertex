#pragma once

#include "vertex/util/memory/friend_unique_ptr.hpp"

namespace vx {
namespace app {

template <typename T, typename Deleter = mem::default_delete<T>>
using owner_ptr = mem::friend_unique_ptr<T, Deleter>;

} // namespace app
} // namespace vx