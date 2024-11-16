#pragma once

#include <cstdint>

namespace vx {
namespace mem {

template <typename T, size_t N>
constexpr size_t array_size(const T(&)[N]) noexcept
{
    return N;
}

template <typename T>
inline T* align_pointer(T* ptr, size_t alignment) noexcept
{
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    addr = (addr + (alignment - 1)) & ~(alignment - 1);
    return reinterpret_cast<T*>(addr);
}

template <typename T>
inline void safe_delete(T*& ptr) noexcept
{
    delete ptr;
    ptr = nullptr;
}

template <typename T>
inline void safe_delete_array(T*& ptr) noexcept
{
    delete[] ptr;
    ptr = nullptr;
}

} // namespace mem
} // namespace vx