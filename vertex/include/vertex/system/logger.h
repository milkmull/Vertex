#pragma once

#include <fstream>
#include <mutex>
#include <sstream>
#include <thread>

#include "vertex/config.h"

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

using callback_fn = void(*)(log_level level, const char* msg);

namespace _priv {

class logger
{
private:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors and destructor
    ///////////////////////////////////////////////////////////////////////////////

    logger() {}
    ~logger() { close_file(); }

    logger(const logger&) = delete;
    logger(logger&&) = delete;

    logger& operator=(const logger&) = delete;
    logger& operator=(logger&&) = delete;

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
            log_buffer(log_level::WARNING) << "Logger: Failed to open file at " << m_filename;
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

    void set_callback(callback_fn callback) { m_callback = callback; }

    void log(log_level level, const std::string& msg)
    {
        if (m_callback_thread == std::this_thread::get_id())
        {
            // We discard any logs from inside a logging
            // callback to prevent infinite recursion.
            return;
        }

        m_mutex.lock();

        if (m_callback)
        {
            m_callback_thread = std::this_thread::get_id();
            m_callback(level, msg.c_str());
            m_callback_thread = std::thread::id();
        }

        std::cout << msg;

        if (m_output_stream.is_open())
        {
            m_output_stream << msg;
            m_output_stream.flush();
        }

        m_mutex.unlock();
    }

private:

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    log_level m_level = log_level::TRACE;

    const char* m_filename = nullptr;
    std::ofstream m_output_stream;

    callback_fn m_callback = nullptr;
    std::thread::id m_callback_thread;

    std::mutex m_mutex;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // helper classes
    ///////////////////////////////////////////////////////////////////////////////

    class log_buffer
    {
    public:

        ///////////////////////////////////////////////////////////////////////////////
        // constructors and destructor
        ///////////////////////////////////////////////////////////////////////////////

        log_buffer(log_level level)
            : m_level(level)
        {
            prefix();
        }

        log_buffer(log_level level, int line, const char* filename)
            : m_level(level)
        {
            prefix();
            m_oss << "line " << line << " file " << filename << ": ";
        };

        ~log_buffer()
        {
            if (m_level >= logger::get().get_log_level())
            {
                if (m_oss.str().back() != '\n')
                {
                    m_oss << '\n';
                }
                logger::get().log(m_level, m_oss.str());
            }
        }

        std::string str() const { return m_oss.str(); }

        ///////////////////////////////////////////////////////////////////////////////
        // static constructors
        ///////////////////////////////////////////////////////////////////////////////

        static log_buffer get(log_level level)
        {
            return log_buffer(level);
        }

        static log_buffer get(log_level level, int line, const char* filename)
        {
            return log_buffer(level, line, filename);
        }

    public:

        ///////////////////////////////////////////////////////////////////////////////
        // manipulator overloads
        ///////////////////////////////////////////////////////////////////////////////

        template <typename T>
        log_buffer& operator<<(T&& msg)
        {
            m_oss << std::forward<T>(msg);
            return *this;
        }

        using manip1 = std::ostream& (*)(std::ostream&);
        log_buffer& operator<<(std::ostream& (*fp)(std::ostream&))
        {
            m_oss << fp;
            return *this;
        }

        using ios_type = std::basic_ios<std::ostream::char_type, std::ostream::traits_type>;
        using manip2 = ios_type & (*)(ios_type&);
        log_buffer& operator<<(manip2 fp)
        {
            m_oss << fp;
            return *this;
        }

        using manip3 = std::ios_base& (*)(std::ios_base&);
        log_buffer& operator<<(manip3 fp)
        {
            m_oss << fp;
            return *this;
        }

    private:

        void prefix()
        {
            switch (m_level)
            {
                case log_level::TRACE:    m_oss << "[TRACE] ";    break;
                case log_level::DEBUG:    m_oss << "[DEBUG] ";    break;
                case log_level::INFO:     m_oss << "[INFO] ";     break;
                case log_level::WARNING:  m_oss << "[WARNING] ";  break;
                case log_level::ERROR:    m_oss << "[ERROR] ";    break;
                case log_level::CRITICAL: m_oss << "[CRITICAL] "; break;
            }
        }

    private:

        ///////////////////////////////////////////////////////////////////////////////
        // data
        ///////////////////////////////////////////////////////////////////////////////

        log_level m_level;
        std::ostringstream m_oss;

    };

    struct dummy_log_buffer
    {
        template <typename T>
        dummy_log_buffer operator<<(const T&) { return dummy_log_buffer(); }
    };

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
/// @brief Sets the global logging callback function.
///
/// The callback will be called whenever any message is logged, and will
/// receive the level of the log as well as the message. Logging is forbidden
/// inside the logging callback function to prevent infinite recursion. Any
/// messages logged inside the callback will be discarded. 
/// 
/// @param callback The callback function.
///////////////////////////////////////////////////////////////////////////////
inline void set_callback(callback_fn callback) { _priv::logger::get().set_callback(callback); }

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

#   define VX_LOG_TRACE         ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::TRACE)
#   define VX_LOG_DEBUG         ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::DEBUG)
#   define VX_LOG_INFO          ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::INFO)
#   define VX_LOG_WARNING       ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::WARNING)
#   define VX_LOG_ERROR         ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::ERROR)
#   define VX_LOG_CRITICAL      ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::CRITICAL)

#   define VX_LOG_TRACE_FULL    ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::TRACE,    VX_LINE, VX_FILE)
#   define VX_LOG_DEBUG_FULL    ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::DEBUG,    VX_LINE, VX_FILE)
#   define VX_LOG_INFO_FULL     ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::INFO,     VX_LINE, VX_FILE)
#   define VX_LOG_WARNING_FULL  ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::WARNING,  VX_LINE, VX_FILE)
#   define VX_LOG_ERROR_FULL    ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::ERROR,    VX_LINE, VX_FILE)
#   define VX_LOG_CRITICAL_FULL ::vx::log::_priv::logger::log_buffer::get(::vx::log::log_level::CRITICAL, VX_LINE, VX_FILE)

#else

#   define VX_LOG_TRACE         ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_DEBUG         ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_INFO          ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_WARNING       ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_ERROR         ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_CRITICAL      ::vx::log::_priv::logger::dummy_log_buffer()

#   define VX_LOG_TRACE_FULL    ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_DEBUG_FULL    ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_INFO_FULL     ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_WARNING_FULL  ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_ERROR_FULL    ::vx::log::_priv::logger::dummy_log_buffer()
#   define VX_LOG_CRITICAL_FULL ::vx::log::_priv::logger::dummy_log_buffer()

#endif

}
}