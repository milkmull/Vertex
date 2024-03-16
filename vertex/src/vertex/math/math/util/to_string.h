#pragma once

#include <sstream>
#include <iomanip>

#include "../detail/base_types.h"

namespace vx {
namespace math {

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline std::string to_string(T x)
{
    return std::to_string(x);
}

template <typename T>
inline std::string to_string(const vec<2, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << " }";
    return oss.str();
}

template <typename T>
inline std::string to_string(const vec<3, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return oss.str();
}

template <typename T>
inline std::string to_string(const vec<4, T>& v)
{
    std::ostringstream oss;
    oss << "{ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " }";
    return oss.str();
}

template <typename T>
inline std::string to_string(const quat_t<T>& q)
{
    std::ostringstream oss;
    oss << "{ " << q.w << ", " << q.x << ", " << q.y << ", " << q.z << " }";
    return oss.str();
}

template <size_t M, size_t N, typename T>
inline std::string to_string(const mat<M, N, T>& m)
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

}
}