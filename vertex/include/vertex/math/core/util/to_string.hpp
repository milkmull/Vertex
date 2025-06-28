#pragma once

#include <string>

#include "../types.hpp"

namespace vx {
namespace math {

namespace __detail {

///////////////////////////////////////////////////////////////////////////////
// formatter
///////////////////////////////////////////////////////////////////////////////

template <size_t size, typename... Args>
VX_FORCE_INLINE void format(char buffer[size], const char* fmt, Args&&... args)
{
    VX_ASSERT(fmt);
    VX_ASSERT(std::strlen(fmt) < size);
    std::snprintf(buffer, size, fmt, std::forward<Args>(args)...);
}

///////////////////////////////////////////////////////////////////////////////
// literal
///////////////////////////////////////////////////////////////////////////////

template <typename T, bool is_float = false>
struct literal
{
    static VX_FORCE_INLINE constexpr const char* value() noexcept { return "%d"; }
};

template <>
struct literal<int64_t, false>
{
    static VX_FORCE_INLINE constexpr const char* value() noexcept { return "%lld"; }
};

template <>
struct literal<uint64_t, false>
{
    static VX_FORCE_INLINE constexpr const char* value() noexcept { return "%llu"; }
};

template <typename T>
struct literal<T, true>
{
    static VX_FORCE_INLINE constexpr const char* value() noexcept { return "%f"; }
};

template <typename T>
VX_FORCE_INLINE constexpr const char* literal_format() noexcept
{
    return literal<T, is_float<T>::value>::value();
}

///////////////////////////////////////////////////////////////////////////////
// max_print_width
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct max_print_width
{
    // max 10 digits + sign
    static constexpr size_t value = 11; 
};

template <>
struct max_print_width<int64_t>
{
    // max 19 digits + sign
    static constexpr size_t value = 20;
};

template <>
struct max_print_width<uint64_t>
{
    // max 20 digits
    static constexpr size_t value = 20;
};

template <>
struct max_print_width<float>
{
    // worst-case: -1.234567e+38 -> about 15-16 chars
    static constexpr size_t value = 16;
};

template <>
struct max_print_width<double>
{
    // ~ -1.7976931348623157e+308 -> about 24 chars
    static constexpr size_t value = 24;
};

////////////////////////////////////////
// vec
///////////////////////////////////////

template <size_t L, typename T>
VX_FORCE_INLINE constexpr size_t vec_calculate_buffer_size_1() noexcept
{
    // vec + L + {} + (L + 1 spaces) + (L - 1 commas) + %ull + null
    return 3 + 1 + 2 + (L + 1) + (L - 1) + (L * 4) + 1;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr size_t vec_calculate_buffer_size_2() noexcept
{
    // vec + L + {} + (L + 1 spaces) + (L - 1 commas) + (L * max_print_width values) + null
    return 3 + 1 + 2 + (L + 1) + (L - 1) + (L * max_print_width<T>::value) + 1;
}

////////////////////////////////////////
// mat
///////////////////////////////////////

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr size_t mat_calculate_buffer_size_1() noexcept
{
    // mat + MxN + {} + (M + 1 spaces) + (M - 1 commas) + (N * vector_buffer_size) + null
    return 3 + 3 + 2 + (M + 1) + (M - 1) + (N * vec_calculate_buffer_size_1<N, T>()) + 1;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr size_t mat_calculate_buffer_size_2() noexcept
{
    // mat + MxN + {} + (M + 1 spaces) + (M - 1 commas) + (N * vector_buffer_size) + null
    return 3 + 3 + 2 + (M + 1) + (M - 1) + (N * vec_calculate_buffer_size_2<N, T>()) + 1;
}

////////////////////////////////////////
// quat
///////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr size_t quat_calculate_buffer_size_1() noexcept
{
    // quat{ %f, { %f, %f, %f } }
    // (quat) + (2 {) + (2 }) + (7 spaces) + (3 commas) + (4 %f) + null
    return static_cast<size_t>(4) + 2 + 2 + 7 + 3 + (static_cast<size_t>(4) * 2) + 1;
}

template <typename T>
VX_FORCE_INLINE constexpr size_t quat_calculate_buffer_size_2() noexcept
{
    // quat{ %f, { %f, %f, %f } }
    // (quat) + (2 {) + (2 }) + (7 spaces) + (3 commas) + (4 values) + null
    return 4 + 2 + 2 + 7 + 3 + (4 * max_print_width<T>::value) + 1;
}

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////
// vec
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE std::string to_string(const vec<2, T>& v)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::vec_calculate_buffer_size_1<2, T>();
    constexpr size_t buffer_size_2 = __detail::vec_calculate_buffer_size_2<2, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "vec2{ %s, %s }";
    __detail::format<buffer_size_1>(buffer_1, f, fmt, fmt);
    __detail::format<buffer_size_2>(buffer_2, buffer_1, v.x, v.y);

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const vec<3, T>& v)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::vec_calculate_buffer_size_1<3, T>();
    constexpr size_t buffer_size_2 = __detail::vec_calculate_buffer_size_2<3, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "vec3{ %s, %s, %s }";
    __detail::format<buffer_size_1>(buffer_1, f, fmt, fmt, fmt);
    __detail::format<buffer_size_2>(buffer_2, buffer_1, v.x, v.y, v.z);

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const vec<4, T>& v)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::vec_calculate_buffer_size_1<4, T>();
    constexpr size_t buffer_size_2 = __detail::vec_calculate_buffer_size_2<4, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "vec4{ %s, %s, %s, %s }";
    __detail::format<buffer_size_1>(buffer_1, f, fmt, fmt, fmt, fmt);
    __detail::format<buffer_size_2>(buffer_2, buffer_1, v.x, v.y, v.z, v.w);

    return buffer_2;
}

///////////////////////////////////////////////////////////////////////////////
// mat
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<2, 2, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<2, 2, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<2, 2, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat2{ { %s, %s }, { %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt,
        fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y,
        m.columns[1].x, m.columns[1].y
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<3, 2, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<3, 2, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<3, 2, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat3x2{ { %s, %s }, { %s, %s }, { %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt,
        fmt, fmt,
        fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y,
        m.columns[1].x, m.columns[1].y,
        m.columns[2].x, m.columns[2].y
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<4, 2, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<4, 2, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<4, 2, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat4x2{ { %s, %s }, { %s, %s }, { %s, %s }, { %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt,
        fmt, fmt,
        fmt, fmt,
        fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y,
        m.columns[1].x, m.columns[1].y,
        m.columns[2].x, m.columns[2].y,
        m.columns[3].x, m.columns[3].y
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<2, 3, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<2, 3, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<2, 3, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat2x3{ { %s, %s, %s }, { %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt,
        fmt, fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z,
        m.columns[1].x, m.columns[1].y, m.columns[1].z
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<3, 3, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<3, 3, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<3, 3, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat3{ { %s, %s, %s }, { %s, %s, %s }, { %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt,
        fmt, fmt, fmt,
        fmt, fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z,
        m.columns[1].x, m.columns[1].y, m.columns[1].z,
        m.columns[2].x, m.columns[2].y, m.columns[2].z
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<4, 3, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<4, 3, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<4, 3, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat4x3{ { %s, %s, %s }, { %s, %s, %s }, { %s, %s, %s }, { %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt,
        fmt, fmt, fmt,
        fmt, fmt, fmt,
        fmt, fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z,
        m.columns[1].x, m.columns[1].y, m.columns[1].z,
        m.columns[2].x, m.columns[2].y, m.columns[2].z,
        m.columns[3].x, m.columns[3].y, m.columns[3].z
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<2, 4, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<2, 4, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<2, 4, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat2x4{ { %s, %s, %s, %s }, { %s, %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z, m.columns[0].w,
        m.columns[1].x, m.columns[1].y, m.columns[1].z, m.columns[1].w
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<3, 4, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<3, 4, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<3, 4, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat3x4{ { %s, %s, %s, %s }, { %s, %s, %s, %s }, { %s, %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt
    );

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z, m.columns[0].w,
        m.columns[1].x, m.columns[1].y, m.columns[1].z, m.columns[1].w,
        m.columns[2].x, m.columns[2].y, m.columns[2].z, m.columns[2].w
    );

    return buffer_2;
}

template <typename T>
VX_FORCE_INLINE std::string to_string(const mat<4, 4, T>& m)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::mat_calculate_buffer_size_1<4, 4, T>();
    constexpr size_t buffer_size_2 = __detail::mat_calculate_buffer_size_2<4, 4, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "mat4{ { %s, %s, %s, %s }, { %s, %s, %s, %s }, { %s, %s, %s, %s }, { %s, %s, %s, %s } }";

    __detail::format<buffer_size_1>(
        buffer_1, f,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt,
        fmt, fmt, fmt, fmt);

    __detail::format<buffer_size_2>(
        buffer_2, buffer_1,
        m.columns[0].x, m.columns[0].y, m.columns[0].z, m.columns[0].w,
        m.columns[1].x, m.columns[1].y, m.columns[1].z, m.columns[1].w,
        m.columns[2].x, m.columns[2].y, m.columns[2].z, m.columns[2].w,
        m.columns[3].x, m.columns[3].y, m.columns[3].z, m.columns[3].w
    );

    return buffer_2;
}

///////////////////////////////////////////////////////////////////////////////
// quat
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE std::string to_string(const quat_t<T>& q)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::quat_calculate_buffer_size_1<T>();
    constexpr size_t buffer_size_2 = __detail::quat_calculate_buffer_size_2<T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "quat{ %s, { %s, %s, %s } }";
    const size_t x = std::strlen(f);
    __detail::format<buffer_size_1>(buffer_1, f, fmt, fmt, fmt, fmt);
    __detail::format<buffer_size_2>(buffer_2, buffer_1, q.w, q.x, q.y, q.z);

    return buffer_2;
}

///////////////////////////////////////////////////////////////////////////////
// ostream operators
///////////////////////////////////////////////////////////////////////////////

template <typename Elem, typename Traits, size_t L, typename T>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const vec<L, T>& v)
{
    return (os << to_string(v));
}

template <typename Elem, typename Traits, size_t M, size_t N, typename T>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const mat<M, N, T>& m)
{
    return (os << to_string(m));
}

template <typename Elem, typename Traits, typename T>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const quat_t<T>& q)
{
    return (os << to_string(q));
}

} // namespace math
} // namespace vx