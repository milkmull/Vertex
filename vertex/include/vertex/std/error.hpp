#pragma once

#include <cstdio>
#include <cstdlib>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/os/error_type.hpp"
#include "vertex/os/thread_id.hpp"
#include "vertex/std/_memory/memory_base.hpp"

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

using error_type = os::error_type;

namespace err {

//=============================================================================
// error code
//=============================================================================

enum code : error_type
{
    // General success and error indicators
    none = error_type{}, // No error; indicates a successful operation or no action needed
    failed = 1,          // General error indicator

    // Runtime-specific errors
    runtime_error,  // Error occurring during runtime, often due to unexpected conditions
    not_configured, // Attempted operation without required configuration settings

    // Boundary and memory errors
    out_of_range,  // Attempted to access beyond valid range, e.g., index out of bounds
    out_of_memory, // Insufficient memory for requested operation
    size_error,    // Inappropriate or invalid size for an operation

    // Data validation and argument errors
    invalid_data,           // Data provided is invalid or corrupted
    invalid_argument,       // Provided argument is invalid or inappropriate for the function
    unsupported_format,     // Format of input or output is not supported
    unsupported_conversion, // Attempted conversion between incompatible types
    unsupported_operation,  // Operation requested is unsupported or not implemented

    // Resource-related errors
    resource_not_found,      // Resource, such as a file or database entry, could not be found
    resource_already_exists, // Resource already exists, e.g., trying to create a file that already exists

    // File operation errors
    file_operation_failed, // General file operation failure
    file_open_failed,      // Failed to open the specified file
    file_read_failed,      // Error occurred while reading from a file
    file_write_failed,     // Error occurred while writing to a file

    // File-specific conditions
    file_no_permission, // Lack of permission to perform a file operation
    file_in_use,        // File is currently in use and cannot be accessed
    file_corrupt,       // File is corrupted or unreadable

    // Platform-specific error
    system_error // Error specific to the platform, e.g., system-level failures
};

/**
 * @brief Converts an error code to its human-readable string representation.
 *
 * Provides a static string description of the error code. Intended for debugging,
 * logging, or displaying user-friendly error messages.
 *
 * @param err The error code to describe.
 * @return A constant C-string describing the error.
 */
constexpr const char* code_to_string(code c) noexcept
{
    constexpr const char* strings[] = {
        "",       // none
        "failed", // failed

        "runtime error",  // runtime_error
        "not configured", // not_configured

        "out of range",  // out_of_range
        "out of memory", // out_of_memory
        "size error",    // size_error

        "invalid data",           // invalid_data
        "invalid argument",       // invalid_argument
        "unsupported format",     // unsupported_format
        "unsupported conversion", // unsupported_conversion
        "unsupported operation",  // unsupported_operation

        "resource not found",      // resource_not_found
        "resource already exists", // resource_already_exists

        "file operation failed", // file_operation_failed
        "file open failed",      // file_open_failed
        "file read failed",      // file_read_failed
        "file write failed",     // file_write_failed

        "file no permission", // file_no_permission
        "file in use",        // file_in_use
        "file corrupt",       // file_corrupt

        "system error" // system_error
    };

    constexpr size_t count = mem::array_size(strings);
    VX_STATIC_ASSERT_MSG(count == static_cast<size_t>(code::system_error) + 1,
        "code_to_string: string table is out of sync with the err::code enum");

    const size_t index = static_cast<size_t>(c);
    return (index < count) ? strings[index] : strings[0];
}

//=============================================================================
// error / success shared base
//=============================================================================

class error;
class success;

namespace _error_priv {

template <typename Derived, typename Other>
class error_code_base
{
public:

    constexpr error_code_base() noexcept : m_code(err::none)
    {}
    constexpr explicit error_code_base(error_type c) noexcept : m_code(c)
    {}
    constexpr error_code_base(code c) noexcept : m_code(static_cast<error_type>(c))
    {}

    constexpr error_code_base(const Other& other) noexcept
        : m_code(other.value())
    {}

    constexpr error_code_base(const error_code_base&) noexcept = default;
    constexpr error_code_base(error_code_base&&) noexcept = default;

    constexpr error_code_base& operator=(const error_code_base&) noexcept = default;
    constexpr error_code_base& operator=(error_code_base&&) noexcept = default;

    constexpr error_type value() const noexcept
    {
        return m_code;
    }

    constexpr const char* message() const noexcept
    {
        return code_to_string(static_cast<code>(m_code));
    }

    constexpr explicit operator Other() const noexcept
    {
        return Other(m_code);
    }

    constexpr friend bool operator==(const Derived& lhs, const Derived& rhs) noexcept
    {
        return lhs.value() == rhs.value();
    }

    constexpr friend bool operator!=(const Derived& lhs, const Derived& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator==(const Derived& lhs, code rhs) noexcept
    {
        return lhs.value() == static_cast<error_type>(rhs);
    }

    constexpr friend bool operator==(code lhs, const Derived& rhs) noexcept
    {
        return rhs == lhs;
    }

private:

    error_type m_code;
};

} // namespace _error_priv

//=============================================================================
// error wrapper
//=============================================================================

class error : public _error_priv::error_code_base<error, success>
{
    using base = _error_priv::error_code_base<error, success>;

public:

    using base::base;

    // true if this represents an actual error (i.e. not err::none)
    constexpr explicit operator bool() const noexcept
    {
        return value() != err::none;
    }
};

//=============================================================================
// success wrapper
//=============================================================================

class success : public _error_priv::error_code_base<success, error>
{
    using base = _error_priv::error_code_base<success, error>;

public:

    using base::base;

    // true if this represents success (i.e. err::none)
    constexpr explicit operator bool() const noexcept
    {
        return value() == err::none;
    }
};

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
    ::vx::err::set_error(err::unsupported_operation, op " is not a supported operation")

} // namespace err

using error = err::error;
using success = err::success;

} // namespace vx
