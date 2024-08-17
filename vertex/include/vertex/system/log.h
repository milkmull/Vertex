#pragma once

#include <fstream>
#include <mutex>
#include <sstream>
#include <thread>
#include <iostream>

#include "vertex/system/compiler.h"

#ifdef ERROR
#   undef ERROR
#endif

namespace vx {
namespace log {

///////////////////////////////////////////////////////////////////////////////
// log level
///////////////////////////////////////////////////////////////////////////////

enum class log_level
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

///////////////////////////////////////////////////////////////////////////////
// internal
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

class logger
{
private:

    logger() {}
    ~logger() { close_file(); }

public:

    static logger& get()
    {
        static logger instance;
        return instance;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file
    ///////////////////////////////////////////////////////////////////////////////

    void open_file(const char* filename)
    {
        close_file();

        m_filename = filename;
        m_output_stream.open(m_filename, std::ios_base::app);

        if (!m_output_stream.is_open())
        {
            std::cout << "[ERROR] Logger: Failed to open file at " << m_filename << std::endl;
            close_file();
        }
    }

    void close_file()
    {
        if (m_output_stream.is_open())
        {
            m_output_stream.close();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // logging
    ///////////////////////////////////////////////////////////////////////////////

    log_level get_log_level() const { return m_level; }
    void set_log_level(log_level level) { m_level = level; }

    void log(log_level level, const std::string& msg)
    {
        m_mutex.lock();

        std::cout << msg;

        if (m_output_stream.is_open())
        {
            m_output_stream << msg;
            m_output_stream.flush();
        }

        m_mutex.unlock();
    }

private:

    log_level m_level = log_level::TRACE;

    const char* m_filename = nullptr;
    std::ofstream m_output_stream;

    mutable std::mutex m_mutex;

};

struct log_stream
{
    log_stream(log_level level)
        : level(level)
    {
        prefix();
    }

    log_stream(log_level level, int line, const char* filename)
        : level(level)
    {
        prefix();
        stream << "line " << line << " file " << filename << ": ";
    }

    ~log_stream()
    {
        if (level >= logger::get().get_log_level())
        {
            if (stream.str().back() != '\n')
            {
                stream << '\n';
            }
            logger::get().log(level, stream.str());
        }
    }

    void prefix()
    {
        switch (level)
        {
            case log_level::TRACE:    stream << "[TRACE] ";    break;
            case log_level::DEBUG:    stream << "[DEBUG] ";    break;
            case log_level::INFO:     stream << "[INFO] ";     break;
            case log_level::WARNING:  stream << "[WARNING] ";  break;
            case log_level::ERROR:    stream << "[ERROR] ";    break;
            case log_level::CRITICAL: stream << "[CRITICAL] "; break;
        }
    }

    log_level level;
    std::ostringstream stream;

};

struct dummy_log_stream
{
    template <typename T>
    dummy_log_stream operator<<(T) { return dummy_log_stream(); }
};

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////
// log level
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the current global log level.
/// 
/// @return The current global log level.
///////////////////////////////////////////////////////////////////////////////
inline log_level get_log_level() { return _priv::logger::get().get_log_level(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Sets the current global log level.
/// 
/// Logging messages that have a severity below this level will not be shown.
///////////////////////////////////////////////////////////////////////////////
inline void set_log_level(log_level level) { _priv::logger::get().set_log_level(level); }

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Begins logging all messages to an output file.
///
/// @param filename The path to the output file. Default is "log.txt".
///////////////////////////////////////////////////////////////////////////////
inline void start_file_output(const char* filename = "log.txt")
{
    _priv::logger::get().open_file(filename);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Stops logging messages to an output file if one was specified.
///////////////////////////////////////////////////////////////////////////////
inline void stop_file_output()
{
    _priv::logger::get().close_file();
}

///////////////////////////////////////////////////////////////////////////////
// logging
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Log a message of a specified level.
///
/// @param level The level of the message being logged.
/// @param msg The message to log.
///////////////////////////////////////////////////////////////////////////////
inline void log(log_level level, const std::string& msg)
{
    _priv::logger::get().log(level, msg);
}

///////////////////////////////////////////////////////////////////////////////
// logging macros
///////////////////////////////////////////////////////////////////////////////

#define VX_ENABLE_LOGGING 1

#if defined(VX_ENABLE_LOGGING)

#   define VX_LOG_TRACE         ::vx::log::_priv::log_stream(::vx::log::log_level::TRACE).stream
#   define VX_LOG_DEBUG         ::vx::log::_priv::log_stream(::vx::log::log_level::DEBUG).stream
#   define VX_LOG_INFO          ::vx::log::_priv::log_stream(::vx::log::log_level::INFO).stream
#   define VX_LOG_WARNING       ::vx::log::_priv::log_stream(::vx::log::log_level::WARNING).stream
#   define VX_LOG_ERROR         ::vx::log::_priv::log_stream(::vx::log::log_level::ERROR).stream
#   define VX_LOG_CRITICAL      ::vx::log::_priv::log_stream(::vx::log::log_level::CRITICAL).stream

#   define VX_LOG_TRACE_FULL    ::vx::log::_priv::log_stream(::vx::log::log_level::TRACE,    VX_LINE, VX_FILE).stream
#   define VX_LOG_DEBUG_FULL    ::vx::log::_priv::log_stream(::vx::log::log_level::DEBUG,    VX_LINE, VX_FILE).stream
#   define VX_LOG_INFO_FULL     ::vx::log::_priv::log_stream(::vx::log::log_level::INFO,     VX_LINE, VX_FILE).stream
#   define VX_LOG_WARNING_FULL  ::vx::log::_priv::log_stream(::vx::log::log_level::WARNING,  VX_LINE, VX_FILE).stream
#   define VX_LOG_ERROR_FULL    ::vx::log::_priv::log_stream(::vx::log::log_level::ERROR,    VX_LINE, VX_FILE).stream
#   define VX_LOG_CRITICAL_FULL ::vx::log::_priv::log_stream(::vx::log::log_level::CRITICAL, VX_LINE, VX_FILE).stream

#else

#   define VX_LOG_TRACE         ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_DEBUG         ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_INFO          ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_WARNING       ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_ERROR         ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_CRITICAL      ::vx::log::_priv::dummy_log_stream()

#   define VX_LOG_TRACE_FULL    ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_DEBUG_FULL    ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_INFO_FULL     ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_WARNING_FULL  ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_ERROR_FULL    ::vx::log::_priv::dummy_log_stream()
#   define VX_LOG_CRITICAL_FULL ::vx::log::_priv::dummy_log_stream()

#endif

}
}