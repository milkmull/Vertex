#pragma once

#include <cstdio>
#include <cstdlib>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/os/error.hpp"
#include "vertex/os/thread_id.hpp"
#include "vertex/std/_memory/memory_base.hpp"
#include "vertex/std_old/error_code.hpp"

//=============================================================================
// configuration macros
//=============================================================================
// The following macros can be defined - typically via a build system flag -
// before this header (and the corresponding .cpp) is compiled, to configure
// the error module:
//
//   VX_ERR_BUFFER_MAX_SIZE
//       Maximum size, in bytes, of the thread-local buffer used to store the
//       formatted message set by err::set(). Longer messages are silently
//       truncated to fit. Defaults to 1024 if left undefined.
//
//   VX_ERR_DISABLE_PRINT_ERROR_HOOK
//       If defined, omits err::print_error_hook() - the default hook that
//       writes error info to stderr - along with its declaration below.
//=============================================================================

namespace vx {
namespace err {

//=============================================================================
// error info
//=============================================================================

struct error_string
{
    const char* data;
    size_t size;
};

// Groups the call-site info (function/file/line) that error(), format_error(),
// and print_error() all need together as a single unit, instead of threading
// three separate trailing parameters through every call site.
struct error_site
{
    const char* function;
    const char* file;
    int line;

    constexpr error_site() noexcept
        : function(nullptr), file(nullptr), line(0)
    {}

    constexpr error_site(const char* fn, const char* fl, int ln) noexcept
        : function(fn), file(fl), line(ln)
    {}
};

struct error_info
{
    bool is_system_error;
    error_type code;
    error_string message;
    os::thread_id thread;
};

//=============================================================================
// printing
//=============================================================================

VX_API void safe_print(const char* data, const size_t count);

//=============================================================================
// error formatting
//=============================================================================

/**
 * @brief Formats and immediately prints an error message, without setting
 * or otherwise reading/writing any thread-local error state.
 *
 * Intended for fatal/unrecoverable paths, where the process is about to
 * terminate and there is no caller left to observe err::get() /
 * err::get_message(). Formats into a small local buffer, not shared or
 * thread-local storage.
 */
VX_API void print_error(
    bool system_error,
    error_type ec,
    const char* message,
    const error_site& site = {}) noexcept;

//=============================================================================
// error accessors and manipulators
//=============================================================================

VX_API error_info get() noexcept;
VX_API error_string get_message() noexcept;
VX_API error_type get_code() noexcept;

namespace _err_priv {

VX_API void set_impl(
    bool system_error,
    error_type ec,
    const char* message,
    const error_site& site = {});

} // namespace _err_priv

inline void set(
    error_type e,
    const char* message = nullptr,
    const error_site& site = {})
{
    if (!message)
    {
        message = code_to_string(static_cast<code>(e));
    }
    _err_priv::set_impl(false, e, message, site);
}

inline void set_system_error(
    error_type e,
    const char* message = nullptr,
    const error_site& site = {})
{
    _err_priv::set_impl(true, e, message, site);
}

inline bool is_set() noexcept
{
    return get_code() != error_type{};
}

inline void clear() noexcept
{
    set(error_type{});
}

VX_API void set_last_os_error(const char* message = nullptr);

//=============================================================================
// error hook
//=============================================================================

using error_hook_t = bool (*)(error_info info);

#if !defined(VX_ERR_DISABLE_HOOK)

/**
 * @brief Sets a thread-local hook that intercepts errors before they are stored.
 * The hook returns true to allow the error to be set, or false to suppress it.
 */
VX_API void set_hook(error_hook_t hook) noexcept;

/**
 * @brief Retrieves the current error hook.
 */
VX_API error_hook_t get_hook() noexcept;

    #if !defined(VX_ERR_DISABLE_PRINT_ERROR_HOOK)

VX_API bool print_error_hook(error_info info);

    #endif // !defined(VX_ERR_DISABLE_PRINT_ERROR_HOOK)

#endif // !defined(VX_ERR_DISABLE_HOOK)

//=============================================================================

class scoped_error_hook
{
public:

    explicit scoped_error_hook(error_hook_t hook) noexcept
        : m_prev(get_hook())
    {
        set_hook(hook);
    }

    ~scoped_error_hook()
    {
        set_hook(m_prev);
    }

    scoped_error_hook(const scoped_error_hook&) = delete;
    scoped_error_hook(scoped_error_hook&&) = delete;

    scoped_error_hook& operator=(const scoped_error_hook&) = delete;
    scoped_error_hook& operator=(scoped_error_hook&&) = delete;

private:

    error_hook_t m_prev;
};

//=============================================================================
// calls
//=============================================================================

template <typename R>
auto return_error(error_type e, R&& ret) noexcept
{
    set(e);
    return std::forward<R>(ret);
}

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

#if defined(VX_DEBUG) && VX_DEBUG

    #define VX_ERR(ec) \
        ::vx::err::set((ec), nullptr, { VX_FUNCTION, VX_FILE, VX_LINE })
    #define VX_ERR_MSG(ec, msg) \
        ::vx::err::set((ec), (msg), { VX_FUNCTION, VX_FILE, VX_LINE })

#else

    #define VX_ERR(ec)          ::vx::err::set(ec)
    #define VX_ERR_MSG(ec, msg) ::vx::err::set((ec), (msg))

#endif

#define VX_ERR_PRINT(msg) \
    ::vx::err::safe_print((msg), ::vx::mem::literal_size(msg))

//========================================
// fatal errors: format and print directly, bypassing thread-local error
// state entirely, since the process is about to trap/terminate and there is
// no caller left to later inspect err::get() / err::get_message().
//========================================

#if (defined(VX_DEBUG) && VX_DEBUG) || defined(VX_ERR_ENABLE_FATAL_PRINT)

    #define VX_ERR_PRIV_FATAL_PRINT(ec, msg) \
        ::vx::err::print_error(false, (ec), (msg), { VX_FUNCTION, VX_FILE, VX_LINE })

#else

    #define VX_ERR_PRIV_FATAL_PRINT(ec, msg) VX_EMPTY_STATEMENT()

#endif

//========================================

#define VX_FATAL_MSG(ec, msg) \
    VX_ERR_PRIV_FATAL_PRINT(ec, msg); \
    VX_GENERATE_TRAP(); \
    VX_UNREACHABLE()

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

#define VX_RET_UNEXPECTED_ERR_IF(cond, ec) \
    VX_RET_IF_UL((cond), ::vx::make_unexpected(::vx::error{ ec }))

//========================================

#define VX_UNSUPPORTED(op) \
    ::vx::err::set(::vx::err::unsupported_operation, op " is not a supported operation")

} // namespace err
} // namespace vx
