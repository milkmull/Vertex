#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/os/io.hpp"
#include "vertex/std/format.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/utf.hpp"

namespace vx {
namespace io {

// ============================================================
// Print
// ============================================================

namespace _io_priv {

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_one_base(os::stream s, const C* v, size_t size)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        os::write_raw(s, v, size);
    }
    else
    {
        using traits = utf::utf_traits<C>;

        const C* ptr = v;
        const C* end = v + size;

        while (ptr != end)
        {
            const char c = static_cast<char>(*ptr);
            os::write_raw(s, &c, 1);
            ++ptr;
        }
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_one(os::stream s, const C* v)
{
    using traits = str::char_traits<C>;
    const size_t size = traits::length(v);
    print_one_base(s, v, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_one(os::stream s, C v)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        os::write_raw(s, &v, 1);
    }
    else
    {
        const char c = static_cast<char>(v);
        os::write_raw(s, &c, 1);
    }
}

template <typename S, VX_REQUIRES(str::is_string_like<S>::value && (sizeof(typename S::value_type) == sizeof(char)))>
void print_one(os::stream s, const S& v)
{
    print_one_base(s, v.data(), v.size());
}

template <typename T>
void print_one(os::stream s, const T& v)
{
    const char fmt[] = { '{', '}' };
    string out;

    const auto res = fmt::format_string(fmt, 2, out, v);
    if (res.err == fmt::format_error::none)
    {
        print_one_base(s, out.data(), out.size());
    }
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

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_raw(os::stream s, const C* data, size_t size)
{
    _io_priv::print_one_base(s, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_raw(const C* data, size_t size)
{
    print_raw(os::stream::out, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void print_raw_err(const C* data, size_t size)
{
    print_raw(os::stream::err, data, size);
}

// ============================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void println_raw(os::stream s, const C* data, size_t size)
{
    _io_priv::print_one_base(s, data, size);

    const char c = '\n';
    os::write_raw(s, &c, 1);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
void println_raw(const C* data, size_t size)
{
    println_raw(os::stream::out, data, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value && (sizeof(C) == sizeof(char)))>
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
