#pragma once

#include <cstdint>
#include <cstring>

namespace vx {
namespace math {

using half_t = uint16_t;

inline constexpr uint32_t half_to_float_ui(uint16_t h) noexcept
{
    // Sign and exponent/mantissa extraction
    const uint32_t s = static_cast<uint32_t>(h & 0x8000u) << 16; // sign bit to float position
    const int32_t em = static_cast<int32_t>(h & 0x7FFFu);        // exp + mantissa

    // Bias exponent difference (127 - 15 = 112)
    int32_t r = (em + (112 << 10)) << 13;

    // Denormals -> zero
    r = (em < (1 << 10)) ? 0 : r;

    // Inf / NaN
    // When em >= 31 << 10 (half exponent 31), add extra 112 << 23 to convert exp = 31 to exp = 255
    r += (em >= (31 << 10)) ? (112 << 23) : 0;

    return static_cast<uint32_t>(s | static_cast<uint32_t>(r));
}

inline float half_to_float(half_t h) noexcept
{
    const uint32_t ui = half_to_float_ui(h);
    float f;
    std::memcpy(&f, &ui, sizeof(f));
    return f;
}

inline constexpr uint16_t half_from_float_ui(uint32_t ui) noexcept
{
    const int32_t s = (ui >> 16) & 0x8000;     // sign
    const int32_t em = static_cast<int32_t>(ui & 0x7FFFFFFF); // abs(exp+mantissa)

    // Bias exponent and round to nearest (add 0.5 ulp @ bit 12)
    // 112 = 127 - 15
    int32_t h = (em - (112 << 23) + (1 << 12)) >> 13;

    // Underflow -> zero (113 encodes exponent -14)
    h = (em < (113 << 23)) ? 0 : h;

    // Overflow -> infinity (143 encodes exponent 16)
    h = (em >= (143 << 23)) ? 0x7C00 : h;

    // NaN -> quiet NaN (anything beyond 255 << 23)
    h = (em > (255 << 23)) ? 0x7E00 : h;

    return static_cast<uint16_t>(s | h);
}

inline half_t half_from_float(float f) noexcept
{
    uint32_t ui;
    std::memcpy(&ui, &f, sizeof(ui));
    return half_from_float_ui(ui);
}

} // namespace math
} // namespace vx