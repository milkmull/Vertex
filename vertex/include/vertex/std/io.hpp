#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/string_convert.hpp"
#include "vertex/std/utf.hpp"

namespace vx {

enum class stream
{
    in,
    out,
    err
};

namespace _io_priv {

VX_API void write_raw(stream s, const char* data, size_t size)
{
    // for now this is fine
    FILE* file = nullptr;

    switch (s)
    {
        case stream::out:
            file = stdout;
            break;

        case stream::err:
            file = stderr;
            break;

        case stream::in:
            return; // cannot write to input
    }

    if (file)
    {
        std::fwrite(data, 1, size, file);
        std::fflush(file);
    }
}

VX_API size_t read_raw(stream s, char* data, size_t size);

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(stream s, const C* v, size_t size)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        _io_priv::write_raw(s, v, size);
    }
    else
    {
        // convert to char and write
        const auto tmp = str::string_cast<char>(v, v + size);
        _io_priv::write_raw(s, tmp.data(), tmp.size());
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(stream s, const C* v)
{
    using traits = str::char_traits<C>;
    const size_t size = traits::length(v);
    print_one(s, v, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
inline void print_one(stream s, C v)
{
    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        _io_priv::write_raw(s, v, 1);
    }
    else
    {
        using traits = utf::utf_traits<C>;
        constexpr size_t max_width = traits::max_width();

        char buf[max_width];
        const auto last = str::string_cast<char>(&v, 1, buf);

        _io_priv::write_raw(s, buf, last - buf);
    }
}

template <typename S, VX_REQUIRES(str::is_string_like<S>::value)>
inline void print_one(stream s, const S& v)
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
inline void print_one(stream s, const T& v)
{
    const auto str = to_string(v);
    _io_priv::print_one(s, str.data(), str.size());
}

} // namespace _io_priv

template <typename... Args>
inline void print(stream s, const Args&... args)
{
    (_io_priv::print_one(s, args), ...);
}

template <typename... Args>
inline void print(const Args&... args)
{
    print(stream::out, args...);
}

template <typename... Args>
inline void println(stream s, const Args&... args)
{
    print(s, args...);
    const char c = '\n';
    _io_priv::write_raw(s, &c, 1);
}

template <typename... Args>
inline void println(const Args&... args)
{
    println(stream::out, args...);
}

} // namespace vx
