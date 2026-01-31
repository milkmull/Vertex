#pragma once

#include <cstdint>

#if defined(min)
    #undef min
#endif

#if defined(max)
    #undef max
#endif

namespace vx {
namespace _priv {

// https://github.com/microsoft/STL/blob/stl/inc/__msvc_minmax.hpp

extern "C" {

struct min_max_element_t
{
    const void* min;
    const void* max;
};

struct min_max_1i
{
    int8_t min;
    int8_t max;
};

struct min_max_1u
{
    uint8_t min;
    uint8_t max;
};

struct min_max_2i
{
    int16_t min;
    int16_t max;
};

struct min_max_2u
{
    uint16_t min;
    uint16_t max;
};

struct min_max_4i
{
    int32_t min;
    int32_t max;
};

struct min_max_4u
{
    uint32_t min;
    uint32_t max;
};

struct min_max_8i
{
    int64_t min;
    int64_t max;
};

struct min_max_8u
{
    uint64_t min;
    uint64_t max;
};

struct min_max_f
{
    float min;
    float max;
};

struct min_max_d
{
    double min;
    double max;
};

struct min_max_p
{
    void* min;
    void* max;
};

} // extern "C"

} // namespace _priv
} // namespace vx
