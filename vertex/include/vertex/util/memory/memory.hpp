#pragma once

#include <cstdint>

namespace vx {
namespace mem {

/**
 * @brief Returns the size of a statically allocated array.
 *
 * This function can be used to determine the size of a statically allocated array.
 * It takes an array as a reference and returns its size as a compile-time constant.
 *
 * @tparam T The type of the array's elements.
 * @tparam N The number of elements in the array.
 * @param arr The array to calculate the size of.
 * @return The size of the array (i.e., the number of elements).
 */
template <typename T, size_t N>
constexpr size_t array_size(const T(&)[N]) noexcept
{
    return N;
}

/**
 * @brief Aligns a pointer to the specified memory alignment.
 *
 * This function adjusts the given pointer so that it satisfies the specified memory alignment.
 * The aligned pointer is returned, which points to the next address that satisfies the given alignment.
 *
 * @tparam T The type of the pointer being aligned.
 * @param ptr The pointer to be aligned.
 * @param alignment The alignment value (must be a power of 2).
 * @return The aligned pointer.
 */
template <typename T>
inline T* align_pointer(T* ptr, size_t alignment) noexcept
{
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    addr = (addr + (alignment - 1)) & ~(alignment - 1);
    return reinterpret_cast<T*>(addr);
}

/**
 * @brief Safely deletes a dynamically allocated object and sets the pointer to nullptr.
 *
 * This function deletes the dynamically allocated object pointed to by `ptr`, then sets `ptr` to `nullptr`
 * to avoid dangling pointers.
 *
 * @tparam T The type of the object being deleted.
 * @param ptr The pointer to the dynamically allocated object.
 */
template <typename T>
inline void safe_delete(T*& ptr) noexcept
{
    delete ptr;
    ptr = nullptr;
}

/**
 * @brief Safely deletes a dynamically allocated array and sets the pointer to nullptr.
 *
 * This function deletes the dynamically allocated array pointed to by `ptr`, then sets `ptr` to `nullptr`
 * to avoid dangling pointers.
 *
 * @tparam T The type of the array's elements.
 * @param ptr The pointer to the dynamically allocated array.
 */
template <typename T>
inline void safe_delete_array(T*& ptr) noexcept
{
    delete[] ptr;
    ptr = nullptr;
}

} // namespace mem
} // namespace vx