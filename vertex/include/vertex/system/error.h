#pragma once

#include <unordered_map>

#include "logger.h"

namespace vx {
namespace error {

///////////////////////////////////////////////////////////////////////////////
// error code
///////////////////////////////////////////////////////////////////////////////

enum class error_code : int
{
    NONE = 0,

    OUT_OF_RANGE,
    OUT_OF_MEMORY,

    INVALID_ARGUMENT,
    UNSUPPORTED_TYPE,
    UNSUPPORTED_FORMAT,
    UNSUPPORTED_CONVERSION,

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
        case error_code::OUT_OF_RANGE:              return "out of range";
        case error_code::OUT_OF_MEMORY:             return "out of memory";
        case error_code::INVALID_ARGUMENT:          return "invalid argument";
        case error_code::UNSUPPORTED_TYPE:          return "unsupported type";
        case error_code::UNSUPPORTED_FORMAT:        return "unsupported format";
        case error_code::UNSUPPORTED_CONVERSION:    return "unsupported conversion";
        case error_code::FILE_ERROR:                return "file error";
        case error_code::PLATFORM_ERROR:            return "platform error";

        default:                                    break;
    }

    return "";
}

struct error_info
{
    error_code code = error_code::NONE;
    const char* message = nullptr;

    inline constexpr explicit operator bool() const { return code != error_code::NONE; }
};

using callback_fn = void(*)(error_code code, const char* msg);

namespace _priv {

class error
{
private:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors and destructor
    ///////////////////////////////////////////////////////////////////////////////

    error() = default;
    ~error() = default;

    error(const error&) = delete;
    error(error&&) = delete;

    error& operator=(const error&) = delete;
    error& operator=(error&&) = delete;

public:

    static error& get()
    {
        static error instance;
        return instance;
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // callback
    ///////////////////////////////////////////////////////////////////////////////

    void set_callback(callback_fn callback) { m_callback = callback; }

    ///////////////////////////////////////////////////////////////////////////////
    // error
    ///////////////////////////////////////////////////////////////////////////////

    error_info get_error()
    {
        return get_error(std::this_thread::get_id());
    }

    error_info get_error(std::thread::id thread)
    {
        m_mutex.lock();

        auto it = m_error_info.find(thread);
        error_info err = (it != m_error_info.end()) ? it->second : error_info();

        m_mutex.unlock();

        return err;
    }

    void set_error(const error_info& err)
    {
        if (m_callback_thread == std::this_thread::get_id())
        {
            // Drop any errors that occurr in the callback to prevent infinite recursion
            return;
        }

        m_mutex.lock();

        if (m_callback)
        {
            m_callback_thread = std::this_thread::get_id();
            m_callback(err.code, err.message);
            m_callback_thread = std::thread::id();
        }

        m_error_info[std::this_thread::get_id()] = err;

        m_mutex.unlock();
    }

    void clear()
    {
        m_mutex.lock();
        m_error_info.clear();
        m_mutex.unlock();
    }

private:

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    std::unordered_map<std::thread::id, error_info> m_error_info;

    callback_fn m_callback = nullptr;
    std::thread::id m_callback_thread;

    std::mutex m_mutex;

};

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////
// error
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the last error that occurred in the current thread.
/// 
/// The returned structure overloads the boolean operator to allow for use in
/// if statements. Call -if (vx::error::get_error())- If you only need to check
/// wheather any error has occurred.
///
/// @return Error info structure containing the error code and message. An
/// error code of error_code::NONE indicates no error.
///////////////////////////////////////////////////////////////////////////////
inline error_info get_error() { return _priv::error::get().get_error(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the last error that occurred in the specified thread.
/// 
/// The returned structure overloads the boolean operator to allow for use in
/// if statements. Call -if (vx::error::get_error())- If you only need to check
/// wheather any error has occurred.
/// 
/// @param thread The id of the thread to check.
///
/// @return Error info structure containing the error code and message. An
/// error code of error_code::NONE indicates no error.
///////////////////////////////////////////////////////////////////////////////
inline error_info get_error(std::thread::id thread) { return _priv::error::get().get_error(thread); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// If a callback has been specified, it will be called with the passed
/// information.
/// 
/// @param code The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
inline void set_error(error_code code, const std::string& msg) { _priv::error::get().set_error(error_info{ code, msg.c_str() }); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Clears all error information for all threads.
///////////////////////////////////////////////////////////////////////////////
inline void clear_errors() { _priv::error::get().clear(); }

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the global error callback function.
///
/// The callback will be called whenever any error occurrs, and will receive
/// the error code as well as the message if one was provided. If any errors
/// occurr inside the error callback function they will only be logged to
/// prevent infinite recursion.
/// 
/// @param callback The callback function.
///////////////////////////////////////////////////////////////////////////////
inline void set_callback(callback_fn callback) { _priv::error::get().set_callback(callback); }

///////////////////////////////////////////////////////////////////////////////
// error macros
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// This macro will first log the error message to the consol, then set the
/// current error for this thread using the provided code and message. If a
/// callback has been specified, it will be called with the passed information
/// as well.
/// 
/// @param code The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
#define VX_ERROR(code, msg) VX_LOG_ERROR_FULL << msg; ::vx::error::set_error(static_cast<::vx::error::error_code>(code), msg)

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the error for the current thread.
///
/// This macro will first log the error message to the consol, then set the
/// current error for this thread using the provided code and message. If a
/// callback has been specified, it will be called with the passed information
/// as well. After this, the program will be terminated.
/// 
/// @param code The error code.
/// @param msg The error message.
///////////////////////////////////////////////////////////////////////////////
#define VX_FATAL_ERROR(code, msg) VX_LOG_CRITICAL_FULL << msg; ::vx::error::set_error(static_cast<::vx::error::error_code>(code), msg); VX_TERMINATE(msg)

}
}