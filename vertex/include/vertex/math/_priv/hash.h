#pragma once

#include "../config.h"

namespace vx {
namespace math {
namespace _priv {

VX_FORCE_INLINE constexpr void hash_combine(size_t& seed, size_t hash)
{
    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
}

}
}
}