#pragma once

#include <sstream>
#include <iomanip>

#include "../type/base/base_types.h"

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// scalar
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline std::string to_string(T x)
{
    return std::to_string(x);
}

///////////////////////////////////////////////////////////////////////////////
// vector
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(const math::vec<2, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << " }";
    return oss.str();
}

template <typename T>
inline std::string to_string(const math::vec<3, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return oss.str();
}

template <typename T>
inline std::string to_string(const math::vec<4, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " }";
    return oss.str();
}

template <size_t L, typename T>
inline std::ostream& operator<<(std::ostream& os, const math::vec<L, T>& v)
{
    os << to_string(v);
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// quaternion
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(const math::quat_t<T>& q)
{
    std::ostringstream oss;
    oss << "{ " << q.w << ", " << q.x << ", " << q.y << ", " << q.z << " }";
    return oss.str();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const math::quat_t<T>& q)
{
    os << to_string(q);
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// matrix
///////////////////////////////////////////////////////////////////////////////

template <size_t M, size_t N, typename T>
inline std::string to_string(const math::mat<M, N, T>& m)
{
    size_t wmax = 0;
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < M; j++)
        {
            std::ostringstream oss;
            oss << m.columns[j][i];
            const size_t w = oss.str().length();
            if (w > wmax) wmax = w;
        }
    }

    std::ostringstream oss;
    for (size_t i = 0; i < N; i++)
    {
        oss << '{';
        for (size_t j = 0; j < M; j++)
        {
            oss << std::setw(wmax + 1) << m.columns[j][i];
        }
        oss << " }";
        if (i < N - 1) oss << '\n';
    }

    return oss.str();
}

template <size_t M, size_t N, typename T>
inline std::ostream& operator<<(std::ostream& os, const math::mat<M, N, T>& m)
{
    os << to_string(m);
    return os;
}

}