#pragma once

#include "vertex/config/architecture.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/config/type_traits.hpp"

// These algorithms are part of MSVC library code:
// https://github.com/microsoft/STL/blob/main/stl/src/vector_algorithms.cpp

namespace vx {
namespace _simd {

// Vector algorithms only fully available on x86 with at least SSE2
#if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
    #define _VX_USE_SIMD_ALGORITHMS 1
#else
    #define _VX_USE_SIMD_ALGORITHMS 0
#endif

#if _VX_USE_SIMD_ALGORITHMS

extern "C" {

//=============================================================================
// find functions
//=============================================================================

const void* VX_STDCALL find_trivial_1(const void* first, const void* last, uint8_t val) noexcept;
const void* VX_STDCALL find_trivial_2(const void* first, const void* last, uint16_t val) noexcept;
const void* VX_STDCALL find_trivial_4(const void* first, const void* last, uint32_t val) noexcept;
const void* VX_STDCALL find_trivial_8(const void* first, const void* last, uint64_t val) noexcept;

//=============================================================================

const void* VX_STDCALL find_last_trivial_1(const void* first, const void* last, uint8_t val) noexcept;
const void* VX_STDCALL find_last_trivial_2(const void* first, const void* last, uint16_t val) noexcept;
const void* VX_STDCALL find_last_trivial_4(const void* first, const void* last, uint32_t val) noexcept;
const void* VX_STDCALL find_last_trivial_8(const void* first, const void* last, uint64_t val) noexcept;

//=============================================================================

const void* VX_STDCALL adjacent_find_1(const void* first, const void* last) noexcept;
const void* VX_STDCALL adjacent_find_2(const void* first, const void* last) noexcept;
const void* VX_STDCALL adjacent_find_4(const void* first, const void* last) noexcept;
const void* VX_STDCALL adjacent_find_8(const void* first, const void* last) noexcept;

//=============================================================================

const void* VX_STDCALL search_1(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL search_2(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL search_4(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL search_8(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

//=============================================================================

const void* VX_STDCALL find_end_1(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL find_end_2(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL find_end_4(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

const void* VX_STDCALL find_end_8(
    const void* first1,
    const void* last1,
    const void* first2,
    size_t count2) noexcept;

//=============================================================================
// meow of functions
//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_1(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_2(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_4(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_8(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_last_of_trivial_pos_1(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_last_of_trivial_pos_2(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

//=============================================================================

const void* VX_STDCALL find_not_ch_1(const void* first, const void* last, uint8_t val) noexcept;
const void* VX_STDCALL find_not_ch_2(const void* first, const void* last, uint16_t val) noexcept;
const void* VX_STDCALL find_not_ch_4(const void* first, const void* last, uint32_t val) noexcept;
const void* VX_STDCALL find_not_ch_8(const void* first, const void* last, uint64_t val) noexcept;

//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_1(
    const void* first,
    const void* last,
    uint8_t val) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_2(
    const void* first,
    const void* last,
    uint16_t val) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_4(
    const void* first,
    const void* last,
    uint32_t val) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_8(
    const void* first,
    const void* last,
    uint64_t val) noexcept;

//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_first_not_of_trivial_pos_1(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

VX_NO_ALIAS size_t VX_STDCALL find_first_not_of_trivial_pos_2(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_last_not_of_trivial_pos_1(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

//=============================================================================

VX_NO_ALIAS size_t VX_STDCALL find_last_not_of_trivial_pos_2(
    const void* haystack,
    size_t haystack_length,
    const void* needle,
    size_t needle_length) noexcept;

} // extern "C"

//=============================================================================
// find templates
//=============================================================================

template <typename T, typename V>
T* find_simd(T* const first, T* const last, const V val) noexcept
{
    VX_IF_CONSTEXPR (std::is_pointer<V>::value || std::is_null_pointer<V>::value)
    {
    #if defined(VX_ARCH_WORD_BITS_64)
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_8(first, last, reinterpret_cast<uint64_t>(val))));
    #else
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_4(first, last, reinterpret_cast<uint32_t>(val))));
    #endif
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_1(first, last, static_cast<uint8_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_2(first, last, static_cast<uint16_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_4(first, last, static_cast<uint32_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_trivial_8(first, last, static_cast<uint64_t>(val))));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T, typename V>
T* find_last_simd(T* const first, T* const last, const V val) noexcept
{
    VX_IF_CONSTEXPR (std::is_pointer<V>::value || std::is_null_pointer<V>::value)
    {
    #if defined(VX_ARCH_WORD_BITS_64)
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_8(first, last, reinterpret_cast<uint64_t>(val))));
    #else
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_4(first, last, reinterpret_cast<uint32_t>(val))));
    #endif
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_1(first, last, static_cast<uint8_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_2(first, last, static_cast<uint16_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_4(first, last, static_cast<uint32_t>(val))));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return const_cast<T*>(
            static_cast<const T*>(find_last_trivial_8(first, last, static_cast<uint64_t>(val))));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T>
T* adjacent_find_simd(T* const first, T* const last) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return const_cast<T*>(static_cast<const T*>(adjacent_find_1(first, last)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return const_cast<T*>(static_cast<const T*>(adjacent_find_2(first, last)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return const_cast<T*>(static_cast<const T*>(adjacent_find_4(first, last)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return const_cast<T*>(static_cast<const T*>(adjacent_find_8(first, last)));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

    #define _VX_SIMD_THRESHOLD_FIND_FIRST_OF ptrdiff_t{ 16 }

template <typename T1, typename T2>
T1* search_simd(T1* const first1, T1* const last1, T2* const first2, const size_t count2) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return const_cast<T1*>(static_cast<const T1*>(search_1(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return const_cast<T1*>(static_cast<const T1*>(search_2(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 4)
    {
        return const_cast<T1*>(static_cast<const T1*>(search_4(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 8)
    {
        return const_cast<T1*>(static_cast<const T1*>(search_8(first1, last1, first2, count2)));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T1, typename T2>
T1* find_end_simd(
    T1* const first1,
    T1* const last1,
    T2* const first2,
    const size_t count2) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return const_cast<T1*>(static_cast<const T1*>(find_end_1(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return const_cast<T1*>(static_cast<const T1*>(find_end_2(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 4)
    {
        return const_cast<T1*>(static_cast<const T1*>(find_end_4(first1, last1, first2, count2)));
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 8)
    {
        return const_cast<T1*>(static_cast<const T1*>(find_end_8(first1, last1, first2, count2)));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

//=============================================================================
// find meow of templates
//=============================================================================

template <typename T1, typename T2>
size_t find_first_of_pos_simd(const T1* const haystack, const size_t haystack_length, const T2* const needle, const size_t needle_length) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return find_first_of_trivial_pos_1(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return find_first_of_trivial_pos_2(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 4)
    {
        return find_first_of_trivial_pos_4(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 8)
    {
        return find_first_of_trivial_pos_8(haystack, haystack_length, needle, needle_length);
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T1, typename T2>
size_t find_last_of_pos_simd(const T1* const haystack, const size_t haystack_length, const T2* const needle, const size_t needle_length) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return find_last_of_trivial_pos_1(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return find_last_of_trivial_pos_2(haystack, haystack_length, needle, needle_length);
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T>
const T* find_not_ch_simd(const T* const first, const T* const last, const T ch) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return static_cast<const T*>(find_not_ch_1(first, last, static_cast<uint8_t>(ch)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return static_cast<const T*>(find_not_ch_2(first, last, static_cast<uint16_t>(ch)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return static_cast<const T*>(find_not_ch_4(first, last, static_cast<uint32_t>(ch)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return static_cast<const T*>(find_not_ch_8(first, last, static_cast<uint64_t>(ch)));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T>
size_t find_last_not_ch_pos_simd(const T* const first, const T* const last, const T ch) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return find_last_not_ch_pos_1(first, last, static_cast<uint8_t>(ch));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return find_last_not_ch_pos_2(first, last, static_cast<uint16_t>(ch));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return find_last_not_ch_pos_4(first, last, static_cast<uint32_t>(ch));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return find_last_not_ch_pos_8(first, last, static_cast<uint64_t>(ch));
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}
template <typename T1, typename T2>
size_t find_first_not_of_pos_simd(const T1* const haystack, const size_t haystack_length, const T2* const needle, const size_t needle_length) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return find_first_not_of_trivial_pos_1(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return find_first_not_of_trivial_pos_2(haystack, haystack_length, needle, needle_length);
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

template <typename T1, typename T2>
size_t find_last_not_of_pos_simd(const T1* const haystack, const size_t haystack_length, const T2* const needle, const size_t needle_length) noexcept
{
    VX_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    VX_IF_CONSTEXPR (sizeof(T1) == 1)
    {
        return find_last_not_of_trivial_pos_1(haystack, haystack_length, needle, needle_length);
    }
    else VX_IF_CONSTEXPR (sizeof(T1) == 2)
    {
        return find_last_not_of_trivial_pos_2(haystack, haystack_length, needle, needle_length);
    }
    else
    {
        VX_STATIC_ASSERT(false); // unexpected size
    }
}

#endif // _VX_USE_SIMD_ALGORITHMS

} // namespace _simd
} // namespace vx
