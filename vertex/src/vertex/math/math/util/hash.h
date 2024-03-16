#pragma once

#include <functional>

#include "../../detail/hash.h"
#include "../detail/base_types.h"

namespace std {

template <typename T>
struct hash<vx::math::vec<2, T>>
{
    inline constexpr size_t operator()(const vx::math::vec<2, T>& v) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(v.x));
        vx::math::detail::hash_combine(seed, hasher(v.y));
        return seed;
    }
};

template <typename T>
struct hash<vx::math::vec<3, T>>
{
    inline constexpr size_t operator()(const vx::math::vec<3, T>& v) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(v.x));
        vx::math::detail::hash_combine(seed, hasher(v.y));
        vx::math::detail::hash_combine(seed, hasher(v.z));
        return seed;
    }
};

template <typename T>
struct hash<vx::math::vec<4, T>>
{
    inline constexpr size_t operator()(const vx::math::vec<4, T>& v) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(v.x));
        vx::math::detail::hash_combine(seed, hasher(v.y));
        vx::math::detail::hash_combine(seed, hasher(v.z));
        vx::math::detail::hash_combine(seed, hasher(v.w));
        return seed;
    }
};

template <typename T>
struct hash<vx::math::quat_t<T>>
{
    inline constexpr size_t operator()(const vx::math::quat_t<T>& q) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(q.w));
        vx::math::detail::hash_combine(seed, hasher(q.x));
        vx::math::detail::hash_combine(seed, hasher(q.y));
        vx::math::detail::hash_combine(seed, hasher(q.z));
        return seed;
    }
};

template <size_t N, typename T>
struct hash<vx::math::mat<2, N, T>>
{
    inline constexpr size_t operator()(const vx::math::mat<2, N, T>& m) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(m.columns[0]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[1]));
        return seed;
    }
};

template <size_t N, typename T>
struct hash<vx::math::mat<3, N, T>>
{
    inline constexpr size_t operator()(const vx::math::mat<3, N, T>& m) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(m.columns[0]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[1]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[2]));
        return seed;
    }
};

template <size_t N, typename T>
struct hash<vx::math::mat<4, N, T>>
{
    inline constexpr size_t operator()(const vx::math::mat<4, N, T>& m) const noexcept
    {
        size_t seed = 0;
        hash<T> hasher;
        vx::math::detail::hash_combine(seed, hasher(m.columns[0]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[1]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[2]));
        vx::math::detail::hash_combine(seed, hasher(m.columns[3]));
        return seed;
    }
};

}