#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/os/io.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/string_convert.hpp"
#include "vertex/std/utf.hpp"

namespace vx {

// ============================================================
// Print
// ============================================================

namespace _io_priv {

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(os::stream s, const C* v, size_t size)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        os::write_raw(s, v, size);
    }
    else
    {
        using traits = utf::utf_traits<C>;
        constexpr size_t max_width = traits::max_width();
        constexpr size_t buf_size = 128;

        char buf[max_width * buf_size];

        const C* ptr = v;
        const C* end = v + size;

        while (ptr != end)
        {
            // Pick a chunk of input characters
            const C* chunk_end = ptr + std::min<size_t>(buf_size, end - ptr);
            char* out_end = str::string_cast<char>(ptr, chunk_end, buf);
            os::write_raw(s, buf, static_cast<size_t>(out_end - buf));
            ptr = chunk_end;
        }
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(os::stream s, const C* v)
{
    using traits = str::char_traits<C>;
    const size_t size = traits::length(v);
    print_one(s, v, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(os::stream s, C v)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        os::write_raw(s, &v, 1);
    }
    else
    {
        using traits = utf::utf_traits<C>;
        constexpr size_t max_width = traits::max_width();

        char buf[max_width];
        const auto last = str::string_cast<char>(&v, 1, buf);

        os::write_raw(s, buf, last - buf);
    }
}

template <typename S, VX_REQUIRES(str::is_string_like<S>::value)>
inline void print_one(os::stream s, const S& v)
{
    _io_priv::print_one(s, v.data(), v.size());
}

template <typename T, typename = void>
struct has_to_string : std::false_type
{};

template <typename T>
struct has_to_string<
    T,
    typename std::enable_if<
        str::is_string_like<
            decltype(to_string(std::declval<const T&>()))>::value>::type> : std::true_type
{};

template <typename T, VX_REQUIRES(has_to_string<T>::value)>
inline void print_one(os::stream s, const T& v)
{
    const auto str = to_string(v);
    _io_priv::print_one(s, str.data(), str.size());
}

} // namespace _io_priv

// ============================================================
// Public printing API
// ============================================================

template <typename... Args>
inline void print(os::stream s, const Args&... args)
{
    (_io_priv::print_one(s, args), ...);
}

template <typename... Args>
inline void print(const Args&... args)
{
    print(os::stream::out, args...);
}

template <typename... Args>
inline void println(os::stream s, const Args&... args)
{
    print(s, args...);

    const char c = '\n';
    os::write_raw(s, &c, 1);
}

template <typename... Args>
inline void println(const Args&... args)
{
    println(os::stream::out, args...);
}

} // namespace vx
