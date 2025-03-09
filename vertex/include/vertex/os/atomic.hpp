#pragma once

#include <atomic>

namespace vx {
namespace os {

template <typename T>
using atomic = std::atomic<T>;

}
}