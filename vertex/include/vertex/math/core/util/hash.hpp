#pragma once

#include "vertex/config/hash.hpp"
#include "vertex/math/core/types/base.hpp"

namespace std {

///////////////////////////////////////////////////////////////////////////////
// vec
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct hash<vx::math::vec<2, T>>
{
    size_t operator()(const vx::math::vec<2, T>& v) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, v.x);
        hash_combine(seed, v.y);

        return seed;
    }
};

template <typename T>
struct hash<vx::math::vec<3, T>>
{
    size_t operator()(const vx::math::vec<3, T>& v) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, v.x);
        hash_combine(seed, v.y);
        hash_combine(seed, v.z);

        return seed;
    }
};

template <typename T>
struct hash<vx::math::vec<4, T>>
{
    size_t operator()(const vx::math::vec<4, T>& v) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, v.x);
        hash_combine(seed, v.y);
        hash_combine(seed, v.z);
        hash_combine(seed, v.w);

        return seed;
    }
};

///////////////////////////////////////////////////////////////////////////////
// mat
///////////////////////////////////////////////////////////////////////////////

template <size_t N, typename T>
struct hash<vx::math::mat<2, N, T>>
{
    size_t operator()(const vx::math::mat<2, N, T>& m) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[0]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[1]));

        return seed;
    }
};

template <size_t N, typename T>
struct hash<vx::math::mat<3, N, T>>
{
    size_t operator()(const vx::math::mat<3, N, T>& m) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[0]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[1]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[2]));

        return seed;
    }
};

template <size_t N, typename T>
struct hash<vx::math::mat<4, N, T>>
{
    size_t operator()(const vx::math::mat<4, N, T>& m) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[0]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[1]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[2]));
        hash_combine(seed, std::hash<vx::math::vec<N, T>>{}(m.columns[3]));

        return seed;
    }
};

///////////////////////////////////////////////////////////////////////////////
// quat
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct hash<vx::math::quat_t<T>>
{
    size_t operator()(const vx::math::quat_t<T>& q) const noexcept
    {
        size_t seed = 0;

        hash_combine(seed, q.w);
        hash_combine(seed, q.x);
        hash_combine(seed, q.y);
        hash_combine(seed, q.z);

        return seed;
    }
};

} // namespace std