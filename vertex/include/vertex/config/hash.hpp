#pragma once

#include <functional>

namespace vx {

template <typename T>
inline void hash_combine(size_t& seed, const T& value)
{
    std::hash<T> hasher;
    seed ^= hasher(value) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

} // namespace vx