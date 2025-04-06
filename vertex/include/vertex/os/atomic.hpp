#pragma once

#include "vertex/config/language_config.hpp"

// https://github.com/libsdl-org/SDL/blob/b8381b3a2ea9873c6baedb8586da8bb5a2775d24/CMakeLists.txt#L580
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic#L464

/*

///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

 // https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/platform.hpp

#if defined(__GNUC__) && defined(__arm__)

// Newer gcc versions define __ARM_ARCH. Older ones don't, so we have to deduce ARM arch version from a bunch of version-specific macros.
#   if defined(__ARM_ARCH)

#       define BOOST_ATOMIC_DETAIL_ARM_ARCH __ARM_ARCH

#   elif defined(__ARM_ARCH_8A__)

#       define BOOST_ATOMIC_DETAIL_ARM_ARCH 8

#   elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
        defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
        defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7S__)

#       define BOOST_ATOMIC_DETAIL_ARM_ARCH 7

#   elif defined(__ARM_ARCH_6__)  || defined(__ARM_ARCH_6J__) || \
        defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
        defined(__ARM_ARCH_6ZK__)

#       define BOOST_ATOMIC_DETAIL_ARM_ARCH 6

#   else
// We are not interested in older versions - they don't support atomic ops
#   define BOOST_ATOMIC_DETAIL_ARM_ARCH 0

#   endif

#endif // defined(__GNUC__) && defined(__arm__)

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#   include <intrin.h>
#   define HAVE_MSC_ATOMICS
#endif

#if !defined(HAVE_GCC_ATOMICS) && defined(VX_PLATFORM_MACOS)
#   include <libkern/OSAtomic.h>
#endif

#if defined(__clang__) && (VX_HAS_BUILTIN(__atomic_load_n) || defined(HAVE_GCC_ATOMICS))
#   define HAVE_ATOMIC_LOAD_N
#elif defined(__GNCU__) && (__GNCU__ >= 5)
#   define HAVE_ATOMIC_LOAD_N
#endif

#if !defined(HAVE_MSC_ATOMICS) && \
    !defined(HAVE_GCC_ATOMICS) && \
    !defined(SDL_PLATFORM_MACOS) && \
    !defined(SDL_PLATFORM_SOLARIS) && \
    !defined(HAVE_WATCOM_ATOMICS)

#   define EMULATE_CAS

#endif

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Wait
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// barrier
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/ops_msvc_common.hpp#L28

#if defined(_MSC_VER)

#if defined(__INTEL_COMPILER)

#define VX_ATOMIC_BARRIER() __memory_barrier()

#elif defined(__clang__) && VX_HAS_BUILTIN(__atomic_signal_fence)

#define VX_ATOMIC_BARRIER() __atomic_signal_fence(__ATOMIC_SEQ_CST)

#elif defined(_MSC_VER) && !defined(_WIN32_WCE)

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
#define VX_ATOMIC_BARRIER() _ReadWriteBarrier()

#endif

#endif // _MSC_VER

#if !defined(VX_ATOMIC_BARRIER)
#   define VX_ATOMIC_BARRIER()
#endif

///////////////////////////////////////////////////////////////////////////////
// atomic base
///////////////////////////////////////////////////////////////////////////////

template <size_t size, bool _signed, typename parent>
struct core_arch_operations
{

};

template <typename T>
class atomic
{
public:

    using type = T;

    T load() const noexcept
    {

    }

    void store(T value) noexcept
    {

    }

private:

    friend __detail::atomic_impl;

    T m_value;
};

}
}

*/