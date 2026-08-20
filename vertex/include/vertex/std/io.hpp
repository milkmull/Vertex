#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/os/io.hpp"
#include "vertex/std/format.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/utf.hpp"

namespace vx {
namespace io {

//=============================================================
// Print
//=============================================================

namespace _io_priv {

template <typename C>
struct compatible_char
{
    static constexpr bool value = (sizeof(C) == sizeof(char));
};

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& compatible_char<C>::value)>
void print_one(os::stream s, const C* v)
{
    using traits = str::char_traits<C>;
    const size_t size = traits::length(v);
    os::write_raw(s, v, size);
}

//template <typename C, size_t N, VX_REQUIRES(type_traits::is_char<C>::value&& compatible_char<C>::value)>
//void print_one(os::stream s, const C (&v)[N])
//{
//    os::write_raw(s, v, N - 1);
//}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& compatible_char<C>::value)>
void print_one(os::stream s, C v)
{
    const char c = static_cast<char>(v);
    os::write_raw(s, &c, 1);
}

template <typename S, VX_REQUIRES(str::is_string_like<S>::value&& compatible_char<typename S::value_type>::value)>
void print_one(os::stream s, const S& v)
{
    os::write_raw(s, v.data(), v.size());
}

// ============================================================

template <typename I>
void print_integer(os::stream s, I v)
{
    using traits = strconv::integer_buffer_traits<I>;

    constexpr size_t buffer_size = traits::buffer_size;
    char buffer_type[buffer_size];

    const auto res = strconv::write_integer_base10(v, buffer_type, buffer_size);
    VX_ASSERT(res.err == strconv::to_string_error::none);
    os::write_raw(s, buffer_type, res.count);
}

template <typename I, VX_REQUIRES(std::is_integral<I>::value && !type_traits::is_char<I>::value)>
void print_one(os::stream s, I v)
{
    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        print_integer(s, static_cast<int64_t>(v));
    }
    else
    {
        print_integer(s, static_cast<uint64_t>(v));
    }
}

// ============================================================

template <typename F, VX_REQUIRES(std::is_floating_point<F>::value)>
void print_one(os::stream s, F v)
{
    using traits = strconv::float_buffer_traits<F>;

    constexpr size_t buffer_size = traits::buffer_size;
    char buffer_type[buffer_size];

    const auto res = strconv::write_float(v, buffer_type, buffer_size);
    VX_ASSERT(res.err == strconv::to_string_error::none);
    os::write_raw(s, buffer_type, res.count);
}

// ============================================================

inline void print_pointer(os::stream s, const void* v)
{
    // 2 hex digits per byte, no sign + prefix
    constexpr size_t buffer_size = (sizeof(uintptr_t) * 2) + 2;
    char buffer_type[buffer_size];

    buffer_type[0] = '0';
    buffer_type[1] = 'x';

    strconv::integer_to_string_format_options fmt{ 16 };

    const auto addr = reinterpret_cast<uintptr_t>(v);
    const auto res = strconv::write_integer(addr, buffer_type + 2, buffer_size - 2, fmt);
    VX_ASSERT(res.err == strconv::to_string_error::none);

    os::write_raw(s, buffer_type, res.count);
}

template <typename T, VX_REQUIRES(!type_traits::is_char<T>::value)>
void print_one(os::stream s, const T* v)
{
    print_pointer(s, v);
}

// ============================================================

template <
    typename T,
    VX_REQUIRES(
        type_traits::is_container_like<T>::value &&
        !str::is_string_like<T>::value &&
        !type_traits::is_char_iter<T>::value)>
void print_one(os::stream s, const T& v)
{
    const char open = '[';
    const char close = ']';
    const char separator = ',';

    os::write_raw(s, &open, 1);

    bool first = true;

    for (const auto& element : v)
    {
        if (!first)
        {
            os::write_raw(s, &separator, 1);
            const char space = ' ';
            os::write_raw(s, &space, 1);
        }

        first = false;
        print_one(s, element);
    }

    os::write_raw(s, &close, 1);
}

} // namespace _io_priv

// ============================================================
// Public printing API
// ============================================================

template <typename... Args>
void print(os::stream s, const Args&... args)
{
    (_io_priv::print_one(s, args), ...);
}

template <typename... Args>
void print(const Args&... args)
{
    print(os::stream::out, args...);
}

template <typename... Args>
void print_err(const Args&... args)
{
    print(os::stream::err, args...);
}

// ============================================================

template <typename... Args>
void println(os::stream s, const Args&... args)
{
    print(s, args...);

    const char c = '\n';
    os::write_raw(s, &c, 1);
}

template <typename... Args>
void println(const Args&... args)
{
    println(os::stream::out, args...);
}

template <typename... Args>
void println_err(const Args&... args)
{
    println(os::stream::err, args...);
}

// ============================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void print_raw(os::stream s, const C* data, size_t size)
{
    os::write_raw(s, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void print_raw(const C* data, size_t size)
{
    print_raw(os::stream::out, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void print_raw_err(const C* data, size_t size)
{
    print_raw(os::stream::err, data, size);
}

// ============================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void println_raw(os::stream s, const C* data, size_t size)
{
    os::write_raw(s, data, size);

    const char c = '\n';
    os::write_raw(s, &c, 1);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void println_raw(const C* data, size_t size)
{
    println_raw(os::stream::out, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value&& _io_priv::compatible_char<C>::value)>
void println_raw_err(const C* data, size_t size)
{
    println_raw(os::stream::err, data, size);
}

} // namespace io

template <typename... Args>
void print(const Args&... args)
{
    io::print(os::stream::out, args...);
}

template <typename... Args>
void println(const Args&... args)
{
    io::println(os::stream::out, args...);
}

} // namespace vx
