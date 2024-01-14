#pragma once

#include <utility>

#include "base_type_defs.h"

namespace vx {
namespace math {
namespace detail {

// =============== unary ===============

template <length_type L, typename T, vec_t Q, typename R>
struct vec_unary {};

template <typename T, typename R>
struct vec_unary<2, T, vec_t::vec, R>
{
    using vec_type = vec<2, T, vec_t::vec>;
    using vec_return_type = vec<2, R, vec_t::vec>;

    template <auto(*func)(T)>
    static inline constexpr vec_return_type call(const vec_type& v)
    {
        return vec_return_type(func(v.x), func(v.y));
    }
};

template <typename T, typename R>
struct vec_unary<3, T, vec_t::vec, R>
{
    using vec_type = vec<3, T, vec_t::vec>;
    using vec_return_type = vec<3, R, vec_t::vec>;

    template <auto(*func)(T)>
    static inline constexpr vec_return_type call(const vec_type& v)
    {
        return vec_return_type(func(v.x), func(v.y), func(v.z));
    }
};

template <typename T, typename R>
struct vec_unary<4, T, vec_t::vec, R>
{
    using vec_type = vec<4, T, vec_t::vec>;
    using vec_return_type = vec<4, R, vec_t::vec>;

    template <auto(*func)(T)>
    static inline constexpr vec_return_type call(const vec_type& v)
    {
        return vec_return_type(func(v.x), func(v.y), func(v.z), func(v.w));
    }
};

template <typename T, typename R>
struct vec_unary<4, T, vec_t::col, R>
{
    using vec_type = vec<4, T, vec_t::col>;
    using vec_return_type = vec<4, R, vec_t::col>;

    template <auto(*func)(T)>
    static inline constexpr vec_return_type call(const vec_type& v)
    {
        return vec_return_type(func(v.r), func(v.g), func(v.b), func(v.a));
    }
};

// =============== binary scaler ===============

template <length_type L, typename T, vec_t Q, typename R>
struct vec_scaler_binary {};

template <typename T, typename R>
struct vec_scaler_binary<2, T, vec_t::vec, R>
{
    using vec_type = vec<2, T, vec_t::vec>;
    using vec_return_type = vec<2, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v, T s)
    {
        return vec_return_type(func(v.x, s), func(v.y, s));
    }
};

template <typename T, typename R>
struct vec_scaler_binary<3, T, vec_t::vec, R>
{
    using vec_type = vec<3, T, vec_t::vec>;
    using vec_return_type = vec<3, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v, T s)
    {
        return vec_return_type(func(v.x, s), func(v.y, s), func(v.z, s));
    }
};

template <typename T, typename R>
struct vec_scaler_binary<4, T, vec_t::vec, R>
{
    using vec_type = vec<4, T, vec_t::vec>;
    using vec_return_type = vec<4, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v, T s)
    {
        return vec_return_type(func(v.x, s), func(v.y, s), func(v.z, s), func(v.w, s));
    }
};

template <typename T, typename R>
struct vec_scaler_binary<4, T, vec_t::col, R>
{
    using vec_type = vec<4, T, vec_t::col>;
    using vec_return_type = vec<4, R, vec_t::col>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v, T s)
    {
        return vec_return_type(func(v.r, s), func(v.g, s), func(v.b, s), func(v.a, s));
    }
};

// =============== binary ===============

template <length_type L, typename T, vec_t Q, typename R>
struct vec_binary {};

template <typename T, typename R>
struct vec_binary<2, T, vec_t::vec, R>
{
    using vec_type = vec<2, T, vec_t::vec>;
    using vec_return_type = vec<2, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v1, const vec_type& v2)
    {
        return vec_return_type(func(v1.x, v2.x), func(v1.y, v2.y));
    }
};

template <typename T, typename R>
struct vec_binary<3, T, vec_t::vec, R>
{
    using vec_type = vec<3, T, vec_t::vec>;
    using vec_return_type = vec<3, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v1, const vec_type& v2)
    {
        return vec_return_type(func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z));
    }
};

template <typename T, typename R>
struct vec_binary<4, T, vec_t::vec, R>
{
    using vec_type = vec<4, T, vec_t::vec>;
    using vec_return_type = vec<4, R, vec_t::vec>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v1, const vec_type& v2)
    {
        return vec_return_type(func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z), func(v1.w, v2.w));
    }
};

template <typename T, typename R>
struct vec_binary<4, T, vec_t::col, R>
{
    using vec_type = vec<4, T, vec_t::col>;
    using vec_return_type = vec<4, R, vec_t::col>;

    template <auto(*func)(T, T)>
    static inline constexpr vec_return_type call(const vec_type& v1, const vec_type& v2)
    {
        return vec_return_type(func(v1.r, v2.r), func(v1.g, v2.g), func(v1.b, v2.b), func(v1.a, v2.a));
    }
};

}
}
}