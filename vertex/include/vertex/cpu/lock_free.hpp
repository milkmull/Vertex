#pragma once

// vertex/cpu/atomic/lock_free.hpp
//
// Compile-time lock-free queries for the primary backend, by byte size.
// The values come from atomic_config.hpp. "Lock-free" here means always
// lock-free for suitably aligned storage.

#include <cstddef>

#include "vertex/config/atomic.hpp"

namespace vx {
namespace cpu {

template <size_t Size>
constexpr bool is_lock_free_size() noexcept
{
    return Size == 1 ? (VX_ATOMIC_LOCK_FREE_1 != 0)
        : Size == 2  ? (VX_ATOMIC_LOCK_FREE_2 != 0)
        : Size == 4  ? (VX_ATOMIC_LOCK_FREE_4 != 0)
        : Size == 8  ? (VX_ATOMIC_LOCK_FREE_8 != 0)
        : Size == 16 ? (VX_ATOMIC_LOCK_FREE_16 != 0)
                     : false;
}

template <typename T>
constexpr bool is_lock_free_type() noexcept
{
    return is_lock_free_size<sizeof(T)>();
}

} // namespace cpu
} // namespace vx
