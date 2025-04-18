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

VX_API level get_level();
VX_API void set_level(level l);

VX_API bool start(const char* output_file);
VX_API void stop();
VX_API bool is_enabled();

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

    template <typename T>
    log_stream& operator<<(const T& value)
    {
        m_stream << value;
        return *this;
    }

    log_stream& operator<<(std::ostream& (*func)(std::ostream&))
    {
        m_stream << func;
        return *this;
    }

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

#define VX_LOG_TRACE         if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::TRACE)
#define VX_LOG_DEBUG         if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::DEBUG)
#define VX_LOG_INFO          if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::INFO)
#define VX_LOG_WARNING       if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::WARNING)
#define VX_LOG_ERROR         if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::ERROR)
#define VX_LOG_CRITICAL      if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::CRITICAL)

#define VX_LOG_TRACE_FULL    if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::TRACE,    VX_LINE, VX_FILE)
#define VX_LOG_DEBUG_FULL    if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::DEBUG,    VX_LINE, VX_FILE)
#define VX_LOG_INFO_FULL     if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::INFO,     VX_LINE, VX_FILE)
#define VX_LOG_WARNING_FULL  if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::WARNING,  VX_LINE, VX_FILE)
#define VX_LOG_ERROR_FULL    if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::ERROR,    VX_LINE, VX_FILE)
#define VX_LOG_CRITICAL_FULL if (!VX_ENABLE_LOGGING); else ::vx::log::__detail::log_stream(::vx::log::level::CRITICAL, VX_LINE, VX_FILE)

} // namespace vx