#pragma once

#include <sstream>

#include "vertex/system/compiler.hpp"
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

    log_stream(level level) : m_level(level) { prefix(); }
    ~log_stream() { flush(); }

    log_stream(level level, int line, const char* file)
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
        std::string msg(m_stream.str());
        if (msg.back() != '\n')
        {
            msg.push_back('\n');
        }

        write(m_level, m_stream.str());
    }

private:

    level m_level;
    std::ostringstream m_stream;
};

struct dummy_log_stream
{
    template <typename T>
    dummy_log_stream& operator<<(T) { return dummy_log_stream(); }
};

#if defined(VX_ENABLE_LOGGING)
static dummy_log_stream null;
#endif

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