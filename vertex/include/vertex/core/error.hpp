#pragma once

#include <sstream>

#include "vertex/core/compiler.hpp"

#ifdef ERROR
#   undef ERROR
#endif

namespace vx {
namespace err {

///////////////////////////////////////////////////////////////////////////////
// error code
///////////////////////////////////////////////////////////////////////////////

enum code : int
{
    NONE = 0,
    OK = NONE,

    ERROR = 1,
    FAILED = ERROR,

    RUNTIME_ERROR,
    NOT_CONFIGURED,

    OUT_OF_RANGE,
    OUT_OF_MEMORY,
    SIZE_ERROR,

    INVALID_DATA,
    INVALID_ARGUMENT,
    UNSUPPORTED_FORMAT,
    UNSUPPORTED_CONVERSION,
    UNSUPPORTED_OPERATION,

    RESOURCE_NOT_FOUND,
    RESOURCE_ALREADY_EXISTS,

    FILE_OPERATION_FAILED,
    FILE_OPEN_FAILED,
    FILE_READ_FAILED,
    FILE_WRITE_FAILED,

    FILE_NO_PERMISSION,
    FILE_IN_USE,
    FILE_CORRUPT,

    PLATFORM_ERROR
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

namespace _priv {

struct error_stream
{
    error_stream(code err) : err(err) {}
    ~error_stream() { set(err, stream.str()); }

    code err;
    std::ostringstream stream;
};

} // namespace _priv

#define VX_ERR(ec) ::vx::err::_priv::error_stream(ec).stream
#define VX_ERR_DEFAULT(ec) ::vx::err::_priv::set_error(ec)

} // namespace err
} // namespace vx