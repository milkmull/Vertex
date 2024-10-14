#pragma once

#if defined(VX_LOG_MEM)

#include <iostream>

void* operator new(std::size_t size)
{
    std::cout << "Allocating " << size << " bytes\n";
    return std::malloc(size);
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    std::cout << "Deallocating " << size << " bytes\n";
    std::free(ptr);
}

#endif

namespace vx {

template <typename T, size_t N>
inline constexpr size_t array_size(const T(&)[N]) noexcept
{
    return N;
}

template <typename T>
inline constexpr T* align_ptr(T* ptr, size_t alignment) noexcept
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

} // namespace vx