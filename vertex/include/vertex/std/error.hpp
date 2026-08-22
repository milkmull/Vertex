#pragma once

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
// error wrapper
//=============================================================================

class error
{
public:

    constexpr error() noexcept : m_code(err::none)
    {}
    constexpr error(error_type c) noexcept : m_code(c)
    {}
    constexpr error(code c) noexcept : m_code(static_cast<error_type>(c))
    {}

    constexpr error(const error&) noexcept = default;
    constexpr error(error&&) noexcept = default;

    constexpr error& operator=(const error&) noexcept = default;
    constexpr error& operator=(error&&) noexcept = default;

    // true if this represents an actual error (i.e. not err::none)
    constexpr explicit operator bool() const noexcept
    {
        return m_code != err::none;
    }

    constexpr error_type value() const noexcept
    {
        return m_code;
    }

    constexpr const char* message() const noexcept
    {
        return code_to_string(static_cast<code>(m_code));
    }

    constexpr friend bool operator==(const error& lhs, const error& rhs) noexcept
    {
        return lhs.m_code == rhs.m_code;
    }

    constexpr friend bool operator!=(const error& lhs, const error& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator==(const error& lhs, code rhs) noexcept
    {
        return lhs.m_code == static_cast<error_type>(rhs);
    }

    constexpr friend bool operator==(code lhs, const error& rhs) noexcept
    {
        return rhs == lhs;
    }

private:

    error_type m_code;
};

//=============================================================================
// success wrapper
//=============================================================================

class success
{
public:

    constexpr success() noexcept : m_code(err::none)
    {}
    constexpr success(error_type c) noexcept : m_code(c)
    {}
    constexpr success(code c) noexcept : m_code(static_cast<error_type>(c))
    {}
    constexpr success(const error& e) noexcept : m_code(e.value())
    {}

    constexpr success(const success&) noexcept = default;
    constexpr success(success&&) noexcept = default;

    constexpr success& operator=(const success&) noexcept = default;
    constexpr success& operator=(success&&) noexcept = default;

    // true if this represents success (i.e. err::none)
    constexpr explicit operator bool() const noexcept
    {
        return m_code == err::none;
    }

    constexpr operator error() const noexcept
    {
        return error(m_code);
    }

    constexpr error_type value() const noexcept
    {
        return m_code;
    }

    constexpr const char* message() const noexcept
    {
        return code_to_string(static_cast<code>(m_code));
    }

    constexpr friend bool operator==(const success& lhs, const success& rhs) noexcept
    {
        return lhs.m_code == rhs.m_code;
    }

    constexpr friend bool operator!=(const success& lhs, const success& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator==(const success& lhs, code rhs) noexcept
    {
        return lhs.m_code == static_cast<error_type>(rhs);
    }

    constexpr friend bool operator==(code lhs, const success& rhs) noexcept
    {
        return rhs == lhs;
    }

private:

    error_type m_code;
};

//=============================================================================
// error info
//=============================================================================

struct error_string
{
    const char* data;
    size_t size;
};

struct error_info
{
    error_type code;
    error_string message;
    os::thread_id thread;
};

//=============================================================================
// error accessors and manipulators
//=============================================================================

VX_API error_info get() noexcept;

VX_API error_string get_message() noexcept;
VX_API error_type get_code() noexcept;

VX_API void set(error_type e, const char* msg, const char* function, const char* file, int line);

inline void set(error_type e, const char* msg)
{
    set(e, msg, nullptr, nullptr, 0);
}

inline void set(error_type e)
{
    set(e, nullptr);
}

inline bool is_set() noexcept
{
    return get_code() != error_type{};
}

inline void clear() noexcept
{
    set(error_type{}, nullptr);
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
// printing
//=============================================================================

VX_API void safe_print(const char* data, const size_t count);

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

//=============================================================================
// error macros
//=============================================================================

#if defined(VX_DEBUG) && VX_DEBUG

    #define VX_ERR(ec)          ::vx::err::set(ec, nullptr, VX_FUNCTION, VX_FILE, VX_LINE)
    #define VX_ERR_MSG(ec, msg) ::vx::err::set(ec, msg, VX_FUNCTION, VX_FILE, VX_LINE)

#else

    #define VX_ERR(ec)          ::vx::err::set(ec)
    #define VX_ERR_MSG(ec, msg) ::vx::err::set(ec, msg)

#endif

#define VX_UNSUPPORTED(op) ::vx::err::set(err::unsupported_operation, op " is not a supported operation")

#define VX_VERIFY(cond, msg) \
    do \
    { \
        if (!(cond)) \
        { \
            ::vx::err::safe_print(msg, mem::literal_size(msg)); \
            ::vx::err::fast_fail(); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

//=============================================================================

#define VX_TRY              ::vx::err::clear();
#define VX_CATCH            if (::vx::err::is_set())
#define VX_CATCH_CODE(code) if (::vx::err::get_code() == code)

} // namespace err

using error = err::error;
using success = err::success;

inline error make_error(error_type e) noexcept
{
    return error{ e };
}

} // namespace vx
