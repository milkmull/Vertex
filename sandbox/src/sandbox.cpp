
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include "vertex/std/string_convert.hpp"
#include "vertex/util/random/rng.hpp"

//static int get_double_base10_exponent_integer(double d)
//{
//    uint64_t bits = vx::bit::bit_cast<uint64_t>(d) & 0x7FFFFFFFFFFFFFFFull;
//
//    // Guard zero, subnormals, and infinities
//    if (bits < 0x0010000000000000ull)
//        return 0;
//    if (bits >= 0x7FF0000000000000ull)
//        return 308;
//
//    // 1. Extract 11-bit biased exponent and 52-bit mantissa
//    int64_t exp2 = static_cast<int64_t>(bits >> 52) - 1023;
//    uint64_t mantissa = bits & 0x000FFFFFFFFFFFFFull;
//
//    // 2. Reconstruct the full implicit fraction: 1.mantissa
//    // We shift it left by 11 so the implicit 1 sits at bit 63.
//    // This gives us a 64-bit fixed-point fraction for the mantissa.
//    uint64_t fraction = (1ull << 52) | mantissa;
//    fraction <<= 11;
//
//    // 3. Define log10(2) and log10(e) properties using a 64-bit multiplier.
//    // To make the math perfectly clean across the entire double range,
//    // we use a capacity-safe scaling factor: 0x4D104D42 = floor(log10(2) * 2^32)
//    uint64_t log10_2_scaled = 0x4D104D42ull; // Approx 1292913986
//
//    // 4. Calculate the base exponent factor
//    int64_t base_exponent = exp2 * log10_2_scaled;
//
//    // 5. High-precision 64-bit multiplication split for the fractional part
//    // We multiply our 64-bit fraction by the fractional component of log10
//    uint64_t frac_hi = fraction >> 32;
//    uint64_t frac_lo = fraction & 0xFFFFFFFFull;
//
//    uint64_t prod_hi = frac_hi * log10_2_scaled;
//    uint64_t prod_lo = frac_lo * log10_2_scaled;
//
//    // Combine the split 64-bit products cleanly
//    uint64_t total_fraction = prod_hi + (prod_lo >> 32);
//
//    // 6. Merge the integer exponent step with the fractional step
//    // Both sides are now perfectly scaled to 32 bits.
//    int64_t final_scaled = base_exponent + (total_fraction >> 32);
//
//    // Shift right by 32 to strip the fixed-point scaling factor away
//    int base10_exp = static_cast<int>(final_scaled >> 32);
//
//    return base10_exp;
//}

constexpr size_t make_float_string(const float f, char* buf, const size_t buf_size)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 10;
    return vx::str::write_float_fixed(f, buf, buf_size, fmt);
}

template <typename F>
static void std_print_scientific_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%e", f);
        std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_fixed_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.100f", f);
        //std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 100;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx");
        const size_t n = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        //std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float_2(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 100;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx2");
        const size_t n = vx::str::write_float_fixed_3(f, buf, sizeof(buf), fmt);
        //std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_scientific_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::scientific;
    fmt.precision = 6;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx");
        const size_t n = vx::str::write_float_scientific_3(f, buf, sizeof(buf), fmt);
        std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

int main()
{
    VX_PROFILE_START("fixed_float.csv");

    vx::random::gen rng;

    //constexpr float f = FLT_MAX;
    //constexpr char buf[100] = {};
    //constexpr size_t n = make_float_string(f, const_cast<char*>(buf), sizeof(buf));



    for (int i = 0; i < 100; ++i)
    {
        const uint32_t bits = rng.randi<uint32_t>();
        const float f = vx::bit::bit_cast<float>(bits);
        //const float f = FLT_TRUE_MIN;
        std_print_scientific_float(f);
        vx_print_scientific_float(f);
    }

    VX_PROFILE_STOP();
    return 0;
}
