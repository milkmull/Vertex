#pragma once

#include <sstream>

#include "vertex/core/compiler.hpp"

#ifdef ERROR
#   undef ERROR
#endif

namespace vx {

using error_t = int;

namespace err {

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
    PLATFORM_ERROR          // Error specific to the platform, e.g., system-level failures
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Obtain a string representing an error code.
/// 
/// @param err The error code.
/// 
/// @return A string representaion of the error code.
///////////////////////////////////////////////////////////////////////////////
VX_API const char* code_to_string(code err);

struct info
{
    code err = code::NONE;
    std::string message;

    inline explicit operator bool() const { return (err != code::NONE); }
};

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
VX_API info get();

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// @param err The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
VX_API void set(code err, const std::string& msg);

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread using the default error
/// message for the error code.
/// 
/// @param err The error code.
///////////////////////////////////////////////////////////////////////////////
VX_API void set(code err);

///////////////////////////////////////////////////////////////////////////////
/// @brief Clears all error information for the current thread.
///////////////////////////////////////////////////////////////////////////////
VX_API void clear();

namespace __detail {

struct error_stream
{
public:

    error_stream(code err) : err(err) {}
    ~error_stream() { set(err, stream.str()); }

    code err;
    std::ostringstream stream;
};

} // namespace __detail

#define VX_ERR(ec) ::vx::err::__detail::error_stream(ec).stream
#define VX_ERR_DEFAULT(ec) ::vx::err::__detail::set_error(ec)

} // namespace err
} // namespace vx