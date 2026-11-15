#pragma once

#include "vertex/config/endian.hpp"

namespace vx {
namespace cpu {

enum class endian
{
    big = VX_ORDER_BIG_ENDIAN,
    little = VX_ORDER_LITTLE_ENDIAN,
    native = VX_ORDER_NATIVE_ENDIAN
};

} // namespace cpu
} // namespace vx
