#pragma once

#include <unordered_map>
#include <sstream>

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
    ERROR = 1,

    OUT_OF_RANGE,
    OUT_OF_MEMORY,
    SIZE_ERROR,

    INVALID_ARGUMENT,
    UNSUPPORTED_TYPE,
    UNSUPPORTED_FORMAT,
    UNSUPPORTED_CONVERSION,
    UNSUPPORTED_OPERATION,

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
inline constexpr const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::NONE:                      return "none";
        case error_code::ERROR:                     return "error";
        case error_code::OUT_OF_RANGE:              return "out of range";
        case error_code::OUT_OF_MEMORY:             return "out of memory";
        case error_code::SIZE_ERROR:                return "size error";
        case error_code::INVALID_ARGUMENT:          return "invalid argument";
        case error_code::UNSUPPORTED_TYPE:          return "unsupported type";
        case error_code::UNSUPPORTED_FORMAT:        return "unsupported format";
        case error_code::UNSUPPORTED_CONVERSION:    return "unsupported conversion";
        case error_code::UNSUPPORTED_OPERATION:     return "unsupported operation";
        case error_code::FILE_ERROR:                return "file error";
        case error_code::PLATFORM_ERROR:            return "platform error";

        default:                                    break;
    }

    return nullptr;
}

struct error_info
{
    error_code code = error_code::NONE;
    std::string message;

    inline constexpr explicit operator bool() const { return code != error_code::NONE; }
};

///////////////////////////////////////////////////////////////////////////////
// internal
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

inline error_info& get_error_internal()
{
    static thread_local error_info error;
    return error;
}

inline void set_error_internal(error_code code, const std::string& msg)
{
    error_info& error = get_error_internal();
    error.code = code;
    error.message = msg;
}

struct error_stream
{
    error_stream(error_code code) : code(code) {}
    ~error_stream() { set_error_internal(code, stream.str()); }

    error_code code;
    std::ostringstream stream;
};

} // namespace _priv

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
inline error_info get_error()
{
    return _priv::get_error_internal();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// @param code The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
inline void set_error(error_code code, const std::string& msg)
{
    _priv::set_error_internal(code, msg);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread using the default error
/// message for the error code.
/// 
/// @param code The error code.
///////////////////////////////////////////////////////////////////////////////
inline void set_error(error_code code)
{
    _priv::set_error_internal(code, error_code_to_string(code));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Clears all error information for the current thread.
///////////////////////////////////////////////////////////////////////////////
inline void clear_error()
{
    _priv::set_error_internal(error_code::NONE, std::string());
}

#define VX_ERROR(code) ::vx::error::_priv::error_stream(code).stream
#define VX_ERROR_DEFAULT(code) ::vx::error::set_error(code)

}
}