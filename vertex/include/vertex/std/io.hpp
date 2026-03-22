#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {

//==============================================================================
// stringify traits
//==============================================================================














enum stream
{
    stdin,
    stderr,
    stdout
};

namespace _priv {

VX_API void write_raw(stream s, const char* data, size_t size);

} // namespace _priv

template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
void print_one(stream s, const T& v)
{
    char buffer[64];
    auto n = std::snprintf(buffer, sizeof buffer, "%g", static_cast<double>(v));
    write_raw(s, buffer, static_cast<std::size_t>(n));
}

inline void print_one(stream s, const char* v)
{
    vx::write_raw(s, v, std::strlen(v));
}

inline void print_one(stream s, char v)
{
    vx::write_raw(s, &v, 1);
}

template <class T>
    requires HasAsString<T>
void print_one(stream s, const T& v)
{
    auto str = as_string(v); // must return std::string or something convertible
    vx::write_raw(s, str.data(), str.size());
}

// fallback: fail at compile time
template <class T>
    requires(!is_trivial_printable<T> && !HasAsString<T>)
void print_one(stream, const T&) = delete;


} // namespace vx
