#pragma once

#include <sstream>

#include "vertex/system/compiler.hpp"

#ifdef ERROR
#   undef ERROR
#endif

namespace vx {
namespace error {

///////////////////////////////////////////////////////////////////////////////
// error code
///////////////////////////////////////////////////////////////////////////////

enum class error_code : int
{
    NONE = 0,
    OK = NONE,

    ERROR = 1,
    FAILED = ERROR,

    RUNTIME_ERROR,

    OUT_OF_RANGE,
    OUT_OF_MEMORY,
    SIZE_ERROR,

    INVALID_ARGUMENT,
    UNSUPPORTED_TYPE,
    UNSUPPORTED_FORMAT,
    UNSUPPORTED_CONVERSION,
    UNSUPPORTED_OPERATION,

    FILE_PATH_NOT_FOUND,
    FILE_PATH_ALREADY_EXISTS,

    FILE_OPERATION_FAILED,
    FILE_OPEN_FAILED,
    FILE_READ_FAILED,
    FILE_WRITE_FAILED,

    FILE_NO_PERMISSION,
    FILE_IN_USE,
    FILE_CORRUPT,
    FILE_ERROR,

    PLATFORM_ERROR
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Obtain a string representing an error code.
/// 
/// @param code The error code.
/// 
/// @return A string representaion of the error code.
///////////////////////////////////////////////////////////////////////////////
VX_API const char* error_code_to_string(error_code code);

struct error_info
{
    error_code code = error_code::NONE;
    std::string message;

    inline explicit operator bool() const { return code != error_code::NONE; }
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
/// error code of error_code::NONE indicates no error.
///////////////////////////////////////////////////////////////////////////////
VX_API error_info get_error();

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// @param code The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
VX_API void set_error(error_code code, const std::string& msg);

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread using the default error
/// message for the error code.
/// 
/// @param code The error code.
///////////////////////////////////////////////////////////////////////////////
VX_API void set_error(error_code code);

///////////////////////////////////////////////////////////////////////////////
/// @brief Clears all error information for the current thread.
///////////////////////////////////////////////////////////////////////////////
VX_API void clear_error();

struct error_stream
{
    error_stream(error_code code) : code(code) {}
    ~error_stream() { set_error(code, stream.str()); }

    error_code code;
    std::ostringstream stream;
};

#define VX_ERROR(code) ::vx::error::error_stream(code).stream
#define VX_ERROR_DEFAULT(code) ::vx::error::set_error(code)

}
}