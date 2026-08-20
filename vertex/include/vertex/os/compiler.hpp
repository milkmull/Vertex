#pragma once

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

template <typename T>
VX_FORCE_INLINE T* compiler_barrier(T* p) noexcept
{
#if defined(_MSC_VER)
    _ReadWriteBarrier();
    return p;
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("" : "+r"(p));
    return p;
#else
    T* volatile vp = p;
    return vp;
#endif
}

//==============================================================================

template <class T>
inline void do_not_optimize(const T& value)
{
#if defined(_MSC_VER)

    (void)value;
    _ReadWriteBarrier();

#else

    asm volatile("" : : "g"(value) : "memory");

#endif
}

} // namespace os
} // namespace vx
