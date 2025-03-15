#pragma once

#include <sstream>

#include "vertex/system/compiler.hpp"

#if defined(ERROR)
#   undef ERROR
#endif

#define VX_ERROR_PRINTING_AVAILABLE VX_DEBUG

namespace vx {

using error_t = uint8_t;

namespace err {

enum { ERROR_MESSAGE_MAX_SIZE = 1024 };

///////////////////////////////////////////////////////////////////////////////
// error code
///////////////////////////////////////////////////////////////////////////////

enum code : error_t
{
    // General success and error indicators
    NONE = 0,               // No error; indicates a successful operation or no action needed
    OK = NONE,              // Alias for NONE, to clearly indicate successful completion

    ERROR = 1,              // General error indicator
    FAILED = ERROR,         // Alias for ERROR, to indicate operation failure

    // Runtime-specific errors
    RUNTIME_ERROR,          // Error occurring during runtime, often due to unexpected conditions
    NOT_CONFIGURED,         // Attempted operation without required configuration settings

    // Boundary and memory errors
    OUT_OF_RANGE,           // Attempted to access beyond valid range, e.g., index out of bounds
    OUT_OF_MEMORY,          // Insufficient memory for requested operation
    SIZE_ERROR,             // Inappropriate or invalid size for an operation

    // Data validation and argument errors
    INVALID_DATA,           // Data provided is invalid or corrupted
    INVALID_ARGUMENT,       // Provided argument is invalid or inappropriate for the function
    UNSUPPORTED_FORMAT,     // Format of input or output is not supported
    UNSUPPORTED_CONVERSION, // Attempted conversion between incompatible types
    UNSUPPORTED_OPERATION,  // Operation requested is unsupported or not implemented

    // Resource-related errors
    RESOURCE_NOT_FOUND,     // Resource, such as a file or database entry, could not be found
    RESOURCE_ALREADY_EXISTS,// Resource already exists, e.g., trying to create a file that already exists

    // File operation errors
    FILE_OPERATION_FAILED,  // General file operation failure
    FILE_OPEN_FAILED,       // Failed to open the specified file
    FILE_READ_FAILED,       // Error occurred while reading from a file
    FILE_WRITE_FAILED,      // Error occurred while writing to a file

    // File-specific conditions
    FILE_NO_PERMISSION,     // Lack of permission to perform a file operation
    FILE_IN_USE,            // File is currently in use and cannot be accessed
    FILE_CORRUPT,           // File is corrupted or unreadable

    // Platform-specific error
    PLATFORM_ERROR,         // Error specific to the platform, e.g., system-level failures
    UNSUPPORTED             // Operation is unsupported on the current platform
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Obtain a string representing an error code.
/// 
/// @param err The error code.
/// 
/// @return A string representaion of the error code.
///////////////////////////////////////////////////////////////////////////////
constexpr const char* code_to_string(code err) noexcept
{
    switch (err)
    {
        case code::NONE:                        return "none";
        case code::ERROR:                       return "error";

        case code::RUNTIME_ERROR:               return "runtime error";
        case code::NOT_CONFIGURED:              return "not configured";

        case code::OUT_OF_RANGE:                return "out of range";
        case code::OUT_OF_MEMORY:               return "out of memory";
        case code::SIZE_ERROR:                  return "size error";

        case code::INVALID_DATA:                return "invalid data";
        case code::INVALID_ARGUMENT:            return "invalid argument";
        case code::UNSUPPORTED_FORMAT:          return "unsupported format";
        case code::UNSUPPORTED_CONVERSION:      return "unsupported conversion";
        case code::UNSUPPORTED_OPERATION:       return "unsupported operation";

        case code::RESOURCE_NOT_FOUND:          return "resource not found";
        case code::RESOURCE_ALREADY_EXISTS:     return "resource already exists";

        case code::FILE_OPERATION_FAILED:       return "file operation failed";
        case code::FILE_OPEN_FAILED:            return "file open failed";
        case code::FILE_READ_FAILED:            return "file read failed";
        case code::FILE_WRITE_FAILED:           return "file write failed";

        case code::FILE_NO_PERMISSION:          return "file no permission";
        case code::FILE_IN_USE:                 return "file in use";
        case code::FILE_CORRUPT:                return "file corrupt";

        case code::PLATFORM_ERROR:              return "platform error";
        case code::UNSUPPORTED:                 return "unsupported operation";
        default:                                return "";
    }
}

struct info
{
    code err = code::NONE;
    const char* message;

    constexpr explicit operator bool() const noexcept { return (err != code::NONE); }
};

///////////////////////////////////////////////////////////////////////////////
// error printing
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

#if VX_ERROR_PRINTING_AVAILABLE

    VX_API void set_error_printing_enabled(bool enabled) noexcept;
#   define VX_PRINT_ERRORS(enabled) ::vx::err::__detail::set_error_printing_enabled(enabled)

#else

#   define VX_PRINT_ERRORS(enabled)

#endif // VX_ERROR_PRINTING_AVAILABLE

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the last error that occurred in the current thread.
/// 
/// The returned structure overloads the boolean operator to allow for use in
/// if statements. Call -if (vx::error::get_error())- if you only need to check
/// wheather an error has occurred.
///
/// @return Error info structure containing the error code and message. An
/// error code of code::NONE indicates no error.
///////////////////////////////////////////////////////////////////////////////
VX_API info get() noexcept;

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// @param err The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
VX_API void set(code err, const char* msg) noexcept;

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread using the default error
/// message for the error code.
/// 
/// @param err The error code.
///////////////////////////////////////////////////////////////////////////////
inline void set(code err) noexcept
{
    set(err, code_to_string(err));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if any error has been set
/// 
/// @return True if an error has been set, false otherwise.
///////////////////////////////////////////////////////////////////////////////
inline bool is_set() noexcept
{
    return get().err != NONE;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Clears all error information for the current thread.
///////////////////////////////////////////////////////////////////////////////
inline void clear() noexcept
{
    set(code::NONE, "");
}

///////////////////////////////////////////////////////////////////////////////
// error stream
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

struct error_stream
{
    error_stream(code err) : ec(err) {}
    ~error_stream() { set(ec, stream.str().c_str()); }

    code ec;
    std::ostringstream stream;
};

inline void unsupported(const char* operation)
{
    error_stream(UNSUPPORTED).stream << operation << " is not a supported operation on this platform";
}

} // namespace __detail

#define VX_ERR(ec) ::vx::err::__detail::error_stream(ec).stream
#define VX_ERR_DEFAULT(ec) ::vx::err::__detail::set_error(ec)
#define VX_UNSUPPORTED(op) ::vx::err::__detail::unsupported(op)

#define VX_RETURN_IF_ERROR(check, ret) \
    do \
    { \
        ::vx::err::clear(); \
        (check); \
        if (::vx::err::is_set()) return ret; \
    } \
    while (VX_NULL_WHILE_LOOP_CONDITION)

} // namespace err
} // namespace vx