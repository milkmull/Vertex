#pragma once

#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/os/thread_id.hpp"

namespace vx {

using error_t = int;

namespace err {

//=============================================================================
// error code
//=============================================================================

enum code : error_t
{
    // General success and error indicators
    none = 0,   // No error; indicates a successful operation or no action needed
    failed = 1, // General error indicator

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
constexpr const char* code_to_string(code err)
{
    switch (err)
    {
        case code::none: return "";
        case code::failed: return "failed";

        case code::runtime_error: return "runtime error";
        case code::not_configured: return "not configured";

        case code::out_of_range: return "out of range";
        case code::out_of_memory: return "out of memory";
        case code::size_error: return "size error";

        case code::invalid_data: return "invalid data";
        case code::invalid_argument: return "invalid argument";
        case code::unsupported_format: return "unsupported format";
        case code::unsupported_conversion: return "unsupported conversion";
        case code::unsupported_operation: return "unsupported operation";

        case code::resource_not_found: return "resource not found";
        case code::resource_already_exists: return "resource already exists";

        case code::file_operation_failed: return "file operation failed";
        case code::file_open_failed: return "file open failed";
        case code::file_read_failed: return "file read failed";
        case code::file_write_failed: return "file write failed";

        case code::file_no_permission: return "file no permission";
        case code::file_in_use: return "file in use";
        case code::file_corrupt: return "file corrupt";

        case code::system_error: return "system error";
        default: return "";
    }
}

/**
 * @struct info
 * @brief Holds the current error state and message for the current thread.
 *
 * Represents the result of an operation, including an error code and an optional
 * descriptive message. The error state is thread-local, ensuring independent error
 * tracking across threads. Can be evaluated in boolean context to check if an error is set.
 */
struct info
{
    code err = code::none;
    const char* message;

    constexpr explicit operator bool() const
    {
        return (err != code::none);
    }
};

//=============================================================================
// error accessors and manipulators
//=============================================================================

VX_API code get_code() noexcept;
VX_API const char* get_message() noexcept;

VX_API void set(code err, const char* msg, const char* function, const char* file, int line);

inline void set(code err, const char* msg)
{
    set(err, msg, nullptr, nullptr, 0);
}

/**
 * @brief Sets the current thread's error state using only an error code.
 *
 * Uses the default string description for the given code.
 * @param err The error code.
 */
VX_API void set(code err) noexcept;

/**
 * @brief Checks if an error is currently set in the current thread.
 *
 * @return true if an error is present; false otherwise.
 */
inline bool is_set() noexcept
{
    return get_code() != none;
}

/**
 * @brief Clears the current thread's error state.
 *
 * Resets the error code to NONE and clears the message for the calling thread.
 */
inline void clear() noexcept
{
    set(code::none);
}

template <typename R>
auto return_error(err::code e, R&& ret) noexcept
{
    set(e);
    return std::forward<R>(ret);
}

//=============================================================================
// error hook
//=============================================================================

using error_hook_t = bool (*)(code err, const char* msg, os::thread_id thread);

/**
 * @brief Sets a thread-local hook that intercepts errors before they are stored.
 * The hook returns true to allow the error to be set, or false to suppress it.
 */
VX_API void set_hook(error_hook_t hook) noexcept;

/**
 * @brief Retrieves the current error hook.
 */
VX_API error_hook_t get_hook() noexcept;

VX_API bool print_error_hook(code err, const char* msg, os::thread_id thread);

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

#define VX_RETURN_IF_ERROR(check, ret) \
    do \
    { \
        ::vx::err::clear(); \
        (check); \
        if (::vx::err::is_set()) \
            return ret; \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_RETURN_IF_ERROR_VOID(check) VX_RETURN_IF_ERROR(void())

#define VX_REPORT_ERROR(msg) ::vx::err::print(msg)

#define VX_CRASH_WITH_MESSAGE(msg) \
    VX_GENERATE_TRAP(); \
    VX_UNREACHABLE()

#define VX_VERIFY(cond, msg) \
    do \
    { \
        if (!(cond)) \
        { \
            VX_CRASH_WITH_MESSAGE(msg); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

//=============================================================================

#define VX_TRY              ::vx::err::clear();
#define VX_CATCH            if (::vx::err::is_set())
#define VX_CATCH_CODE(code) if (::vx::err::get_code() == code)

} // namespace err
} // namespace vx
