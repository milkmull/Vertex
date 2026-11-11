//#include "vertex/std/time/time.hpp"
//
//namespace vx {
//namespace time {
//
////=============================================================================
//// Ticks
////=============================================================================
//
//static inline uint32_t calculate_gcd(uint32_t a, uint32_t b)
//{
//    if (b == 0)
//    {
//        return a;
//    }
//
//    return calculate_gcd(b, (a % b));
//}
//
//struct ticks_data
//{
//    int64_t start_ticks;
//    int64_t ticks_per_second;
//    uint32_t num_ns;
//    uint32_t den_ns;
//
//    ticks_data()
//    {
//        start_ticks = get_performance_counter();
//        ticks_per_second = get_performance_frequency();
//
//        const uint32_t gcd = calculate_gcd(
//            static_cast<uint32_t>(nanoseconds_per_second),
//            static_cast<uint32_t>(ticks_per_second));
//
//        num_ns = static_cast<uint32_t>(nanoseconds_per_second) / gcd;
//        den_ns = static_cast<uint32_t>(ticks_per_second / gcd);
//    }
//};
//
//duration get_ticks()
//{
//    // initialize on first call
//    static const ticks_data s_ticks_data{};
//
//    const int64_t elapsed_ticks = get_performance_counter() - s_ticks_data.start_ticks;
//
//    int64_t value = (elapsed_ticks * s_ticks_data.num_ns);
//    VX_ASSERT(value >= elapsed_ticks);
//    value /= s_ticks_data.den_ns;
//
//    return nanoseconds(value);
//}
//
//} // namespace time
//} // namespace vx
