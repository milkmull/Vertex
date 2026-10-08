#include <cstddef>
#include <cstdint>
#include <limits>

#include "vertex/config/assert.hpp"
#include "vertex/os/shared_library.hpp"
#include "vertex/std/algorithm.hpp"
#include "vertex/std/error.hpp"

#include "vertex_impl/os/_platform/windows/windows_random.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

namespace {

// Primary: CNG (Vista+, system-preferred RNG flag needs Win7+)
using BCryptGenRandom_t = LONG(WINAPI*)(
    void* hAlgorithm,
    PUCHAR pbBuffer,
    ULONG cbBuffer,
    ULONG dwFlags);

// Fallback: RtlGenRandom, exported as SystemFunction036 from advapi32.dll
using RtlGenRandom_t = BOOLEAN(WINAPI*)(
    PVOID RandomBuffer,
    ULONG RandomBufferLength);

#if !defined(BCRYPT_USE_SYSTEM_PREFERRED_RNG)
    #define BCRYPT_USE_SYSTEM_PREFERRED_RNG 0x00000002
#endif

// Resolves exactly one backend at construction. RtlGenRandom is only
// considered if CNG is unavailable (DLL or export missing); once a backend
// is selected it is never swapped out on a runtime failure.
struct entropy_tools
{
    os::shared_library library;
    BCryptGenRandom_t BCryptGenRandom = nullptr;
    RtlGenRandom_t RtlGenRandom = nullptr;

    entropy_tools()
    {
        if (!try_bcrypt())
        {
            try_rtl();
        }
    }

    bool available() const noexcept
    {
        return BCryptGenRandom != nullptr || RtlGenRandom != nullptr;
    }

    bool fill(uint8_t* bytes, ULONG count) const noexcept
    {
        if (BCryptGenRandom != nullptr)
        {
            if (!windows::nt_success(BCryptGenRandom(
                    nullptr,
                    reinterpret_cast<PUCHAR>(bytes),
                    count,
                    BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
            {
                // BCryptGenRandom does not set ::GetLastError
                return false;
            }

            return true;
        }

        VX_ASSERT(RtlGenRandom);
        if (RtlGenRandom(bytes, count) == FALSE)
        {
            err::set_last_os_error("RtlGenRandom");
            return false;
        }

        return true;
    }

private:

    bool try_bcrypt()
    {
        if (!library.load("bcrypt.dll"))
        {
            return false;
        }

        BCryptGenRandom = library.get<BCryptGenRandom_t>("BCryptGenRandom");
        if (BCryptGenRandom == nullptr)
        {
            library.free();
            return false;
        }

        // Capability probe: Vista exports BCryptGenRandom but rejects
        // BCRYPT_USE_SYSTEM_PREFERRED_RNG with a null algorithm handle.
        uint8_t probe = 0;
        if (!windows::nt_success(BCryptGenRandom(
                nullptr,
                &probe,
                1,
                BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
        {
            BCryptGenRandom = nullptr;
            library.free();
            return false;
        }

        return true;
    }

    bool try_rtl()
    {
        if (!library.load("advapi32.dll"))
        {
            return false;
        }

        RtlGenRandom = library.get<RtlGenRandom_t>("SystemFunction036");
        if (RtlGenRandom == nullptr)
        {
            library.free();
            return false;
        }

        return true;
    }
};

static entropy_tools& get_entropy_tools() noexcept
{
    static entropy_tools tools;
    return tools;
}

} // namespace

error get_entropy_impl(uint8_t* bytes, size_t count)
{
    VX_ASSERT(bytes != nullptr);

    if (count == 0)
    {
        return success{};
    }

    const auto& tools = get_entropy_tools();

    if (!tools.available())
    {
        return err::unsupported_operation;
    }

    constexpr size_t max_chunk = static_cast<size_t>((std::numeric_limits<ULONG>::max)());

    while (count != 0)
    {
        const ULONG chunk = static_cast<ULONG>(vx::min(count, max_chunk));

        if (!tools.fill(bytes, chunk))
        {
            return err::system_error;
        }

        bytes += chunk;
        count -= chunk;
    }

    return success{};
}

} // namespace os
} // namespace vx
