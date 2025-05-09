#pragma once

#include <sstream>

#include "vertex/config/compiler.hpp"
#include "vertex/system/assert.hpp"

#if defined(ERROR)
#   undef ERROR
#endif

#if VX_DEBUG
#   define VX_ENABLE_LOGGING 1
#endif

#if !defined(VX_ENABLE_LOGGING)
#   define VX_ENABLE_LOGGING 0
#endif

namespace vx {
namespace log {

enum class level
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

/**
 * @brief Gets the currently active logging level.
 *
 * @return The current log level.
 */
VX_API level get_level();

/**
 * @brief Sets the minimum logging level.
 *
 * Only messages at or above this level will be logged.
 *
 * @param l The new minimum log level.
 */
VX_API void set_level(level l);

/**
 * @brief Initializes the logging system and opens the given output file.
 *
 * @param output_file Path to the log file.
 * @return true if logging was successfully started, false otherwise.
 */
VX_API bool start(const char* output_file);

/**
 * @brief Stops the logging system and flushes all output.
 */
VX_API void stop();

/**
 * @brief Checks whether logging is currently enabled.
 *
 * @return true if logging is active, false otherwise.
 */
VX_API bool is_enabled();

/**
 * @brief Writes a message at the given log level.
 *
 * @param l The severity level of the log message.
 * @param msg The message to log.
 */
VX_API void write(level l, const std::string& msg);

namespace __detail {

class log_stream
{
public:

    log_stream(level log_level) : m_level(log_level) { prefix(); }
    ~log_stream() { flush(); }

    log_stream(level log_level, int line, const char* file)
        : m_level(log_level)
    {
        prefix();
        m_stream << "line " << line << " file " << file << ": ";
    }

private:

    void prefix()
    {
        switch (m_level)
        {
            case level::TRACE:    m_stream << "[TRACE] ";    break;
            case level::DEBUG:    m_stream << "[DEBUG] ";    break;
            case level::INFO:     m_stream << "[INFO] ";     break;
            case level::WARNING:  m_stream << "[WARNING] ";  break;
            case level::ERROR:    m_stream << "[ERROR] ";    break;
            case level::CRITICAL: m_stream << "[CRITICAL] "; break;
        }
    }

public:

    template <typename... Args>
    void stream(Args&&... args)
    {
        (m_stream << ... << args);
    }

private:

    void flush()
    {
        m_stream << VX_LINE_END;
        const std::string msg(m_stream.str());
        write(m_level, m_stream.str());
    }

private:

    level m_level;
    std::ostringstream m_stream;
};

} // namespace __detail
} // namespace log

///////////////////////////////////////////////////////////////////////////////
// logging macros
///////////////////////////////////////////////////////////////////////////////

/**
 * @def VX_LOG_TRACE, VX_LOG_DEBUG, VX_LOG_INFO, VX_LOG_WARNING, VX_LOG_ERROR, VX_LOG_CRITICAL
 * @brief Stream-based macros for logging messages at specific severity levels.
 *
 * These macros are active only if VX_ENABLE_LOGGING is enabled. They use
 * `log_stream` to accumulate a message which is logged automatically.
 *
 * Example:
 * @code
 * VX_LOG_INFO("Loaded configuration for user: ", username);
 * @endcode
 */

#if VX_ENABLE_LOGGING

#   define VX_LOG_TRACE(...)                ::vx::log::__detail::log_stream(::vx::log::level::TRACE                     ).stream(__VA_ARGS__)
#   define VX_LOG_DEBUG(...)                ::vx::log::__detail::log_stream(::vx::log::level::DEBUG                     ).stream(__VA_ARGS__)
#   define VX_LOG_INFO(...)                 ::vx::log::__detail::log_stream(::vx::log::level::INFO                      ).stream(__VA_ARGS__)
#   define VX_LOG_WARNING(...)              ::vx::log::__detail::log_stream(::vx::log::level::WARNING                   ).stream(__VA_ARGS__)
#   define VX_LOG_ERROR(...)                ::vx::log::__detail::log_stream(::vx::log::level::ERROR                     ).stream(__VA_ARGS__)
#   define VX_LOG_CRITICAL(...)             ::vx::log::__detail::log_stream(::vx::log::level::CRITICAL                  ).stream(__VA_ARGS__)

#   define VX_LOG_TRACE_FULL(...)           ::vx::log::__detail::log_stream(::vx::log::level::TRACE,    VX_LINE, VX_FILE).stream(__VA_ARGS__)
#   define VX_LOG_DEBUG_FULL(...)           ::vx::log::__detail::log_stream(::vx::log::level::DEBUG,    VX_LINE, VX_FILE).stream(__VA_ARGS__)
#   define VX_LOG_INFO_FULL(...)            ::vx::log::__detail::log_stream(::vx::log::level::INFO,     VX_LINE, VX_FILE).stream(__VA_ARGS__)
#   define VX_LOG_WARNING_FULL(...)         ::vx::log::__detail::log_stream(::vx::log::level::WARNING,  VX_LINE, VX_FILE).stream(__VA_ARGS__)
#   define VX_LOG_ERROR_FULL(...)           ::vx::log::__detail::log_stream(::vx::log::level::ERROR,    VX_LINE, VX_FILE).stream(__VA_ARGS__)
#   define VX_LOG_CRITICAL_FULL(...)        ::vx::log::__detail::log_stream(::vx::log::level::CRITICAL, VX_LINE, VX_FILE).stream(__VA_ARGS__)

#else

#   define VX_LOG_TRACE(...)
#   define VX_LOG_DEBUG(...)
#   define VX_LOG_INFO(...)
#   define VX_LOG_WARNING(...)
#   define VX_LOG_ERROR(...)
#   define VX_LOG_CRITICAL(...)

#   define VX_LOG_TRACE_FULL(...)
#   define VX_LOG_DEBUG_FULL(...)
#   define VX_LOG_INFO_FULL(...)
#   define VX_LOG_WARNING_FULL(...)
#   define VX_LOG_ERROR_FULL(...)
#   define VX_LOG_CRITICAL_FULL(...)

#endif // VX_ENABLE_LOGGING

} // namespace vx