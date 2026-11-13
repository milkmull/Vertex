#pragma once

#include <cstdlib>

#include "vertex/config/language_config.hpp"
#include "vertex/config/os.hpp"
#include "vertex/os/error_code.hpp"
#include "vertex/os/stream.hpp"

// for __fastfail on windows
#if defined(_MSC_VER)
    #include <intrin.h>
#endif

namespace vx {
namespace err {

struct error_site
{
    const char* function;
    const char* file;
    uint32_t line;
};

//=============================================================================
// printing
//=============================================================================

inline void safe_print(const char* data, const size_t count)
{
    const auto h = os::get_stream_handle(os::stream::err);
    os::_stream_priv::safe_write(h, reinterpret_cast<const uint8_t*>(data), count);
}

template <size_t N>
inline void safe_print_literal(const char (&data)[N]) noexcept
{
    safe_print(data, N - 1);
}

/**
 * @brief Formats a fatal error report and writes it to stderr in a single
 *        safe_print call.
 *
 * Uses a fixed stack buffer (VX_ERR_BUFFER_MAX_SIZE); long messages are
 * truncated. Output:
 *   [ERROR]: <code> | <msg> | function: <f> | file: <file> | line: <n>
 */
VX_API void fatal_print(
    const bool system_error,
    const int64_t ec,
    const char* msg,
    const error_site& site) noexcept;

/**
 * @brief Terminates the process immediately.
 *
 * Used when execution cannot safely continue after an unrecoverable error.
 */
VX_NO_RETURN inline void abort() noexcept
{
    std::abort();
}

/**
 * @brief Immediately terminates execution using a compiler/platform trap.
 *
 * Used for internal failures and violated invariants where continuing
 * execution is not valid. When supported, this produces a hard trap that
 * can be caught by a debugger or crash handler.
 */
VX_NO_RETURN inline void fast_fail() noexcept
{
    VX_GENERATE_TRAP();
}

VX_NO_RETURN inline void unreachable() noexcept
{
    VX_UNREACHABLE();
}

//=============================================================================
// error macros
//=============================================================================

#if (defined(VX_DEBUG) && VX_DEBUG) || defined(VX_ERR_ENABLE_FATAL_PRINT)

    #define VX_ERR_PRIV_FATAL_PRINT(ec, msg) \
        ::vx::err::fatal_print( \
            false, \
            static_cast<int64_t>(ec), \
            (msg), \
            ::vx::err::error_site{ VX_FUNCTION, VX_FILE, static_cast<uint32_t>(VX_LINE) })

#else

    #define VX_ERR_PRIV_FATAL_PRINT(ec, msg) VX_EMPTY_STATEMENT()

#endif

//========================================

#define VX_FATAL_MSG(ec, msg) \
    do \
    { \
        VX_ERR_PRIV_FATAL_PRINT(ec, msg); \
        VX_GENERATE_TRAP(); \
        VX_UNREACHABLE(); \
    } while (0)

#define VX_FATAL(ec) VX_FATAL_MSG(ec, nullptr)

//========================================

#define VX_VERIFY_CODE_MSG(cond, ec, msg) \
    VX_UNLIKELY_COLD_PATH((!(cond)), { \
        VX_FATAL_MSG((ec), (msg)); \
    }); \
    VX_ASSUME(cond)

#define VX_VERIFY_CODE(cond, ec) \
    VX_VERIFY_CODE_MSG((cond), (ec), ::vx::err::code_to_string(static_cast<::vx::err::code>(ec)))

#define VX_VERIFY_MSG(cond, msg) \
    VX_VERIFY_CODE_MSG((cond), ::vx::err::failed, (msg))

#define VX_VERIFY(cond) \
    VX_VERIFY_CODE(cond, ::vx::err::failed)

//========================================

#define VX_RET_ERR_IF(cond, ec) \
    VX_RET_IF_UL((cond), ::vx::error{ ec })

} // namespace err
} // namespace vx
