#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

#include "vertex/detail/setup.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif

namespace vx {

class logger
{
public:

    enum class priority
    {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        CRITICAL = 5
    };

private:

    // =============== constructors and destructor ===============

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

    static inline priority get_priority() { return get().m_priority; }
    static void set_priority(priority priority) { get().m_priority = priority; }

    // =============== file stuff ===============

    static void start_file_output(const char* filename = "log.txt")
    {
        get().open_file(filename);
    }

    static void stop_file_output()
    {
        get().close_file();
    }

private:

    void open_file(const char* filename)
    {
        close_file();

        m_filename = filename;
        m_output_stream.open(m_filename, std::ios_base::app);

        if (!m_output_stream.is_open())
        {
            std::printf("log: Failed to open file at %s\n", m_filename);
        }
    }

    void close_file()
    {
        if (m_output_stream.is_open())
        {
            m_output_stream.close();
        }
    }

public:

    // =============== logging ===============

    static void log(const std::string& str)
    {
        get().log_internal(str);
    }

    static void log(const std::ostringstream& stream)
    {
        log(stream.str());
    }

private:

    void log_internal(const std::string& str)
    {
        m_mutex.lock();

        std::cout << str;
        if (m_output_stream.is_open())
        {
            m_output_stream << str;
        }

        m_mutex.unlock();
    }

private:

    // =============== data ===============

    std::mutex m_mutex;
    priority m_priority = priority::CRITICAL;

    const char* m_filename = nullptr;
    std::ofstream m_output_stream;

public:

    class log_buffer
    {
    public:

        // =============== constructors and destructor ===============

        log_buffer(priority priority)
            : m_priority(priority)
        {
            prefix();
        }

        log_buffer(priority priority, int line, const char* filename)
            : m_priority(priority)
        {
            prefix();
            m_oss << "line " << line << " file " << filename << ": ";
        };

        ~log_buffer()
        {
            if (m_priority <= get_priority())
            {
                if (m_oss.str().back() != '\n')
                {
                    m_oss << "\n";
                }
                log(m_oss);
            }

            m_oss.flush();
        }

        // =============== static constructors ===============

        static log_buffer get(priority priority)
        {
            return log_buffer(priority);
        }

        static log_buffer get(priority priority, int line, const char* filename)
        {
            return log_buffer(priority, line, filename);
        }

    public:

        // =============== manipulator overloads ===============

        template <typename T>
        log_buffer& operator<<(T&& message)
        {
            m_oss << std::forward<T>(message);
            return *this;
        }

        // overload for manipulators

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
            switch (m_priority)
            {
                case priority::TRACE:
                    m_oss << "[TRACE] ";
                    break;
                case priority::DEBUG:
                    m_oss << "[DEBUG] ";
                    break;
                case priority::INFO:
                    m_oss << "[INFO] ";
                    break;
                case priority::WARNING:
                    m_oss << "[WARNING] ";
                    break;
                case priority::ERROR:
                    m_oss << "[ERROR] ";
                    break;
                case priority::CRITICAL:
                    m_oss << "[CRITICAL] ";
                    break;
            }
        }

    private:

        // =============== data ===============

        std::ostringstream m_oss;
        priority m_priority;

    };

};

#if defined(VX_ENABLE_LOGGING)

#   define VX_LOG_TRACE         ::vx::logger::log_buffer::get(::vx::logger::priority::TRACE)
#   define VX_LOG_DEBUG         ::vx::logger::log_buffer::get(::vx::logger::priority::DEBUG)
#   define VX_LOG_INFO          ::vx::logger::log_buffer::get(::vx::logger::priority::INFO)
#   define VX_LOG_WARNING       ::vx::logger::log_buffer::get(::vx::logger::priority::WARNING)
#   define VX_LOG_ERROR         ::vx::logger::log_buffer::get(::vx::logger::priority::ERROR)
#   define VX_LOG_CRITICAL      ::vx::logger::log_buffer::get(::vx::logger::priority::CRITICAL)

#   define VX_LOG_TRACE_FULL    ::vx::logger::log_buffer::get(::vx::logger::priority::TRACE,    VX_LINE, VX_FILE)
#   define VX_LOG_DEBUG_FULL    ::vx::logger::log_buffer::get(::vx::logger::priority::DEBUG,    VX_LINE, VX_FILE)
#   define VX_LOG_INFO_FULL     ::vx::logger::log_buffer::get(::vx::logger::priority::INFO,     VX_LINE, VX_FILE)
#   define VX_LOG_WARNING_FULL  ::vx::logger::log_buffer::get(::vx::logger::priority::WARNING,  VX_LINE, VX_FILE)
#   define VX_LOG_ERROR_FULL    ::vx::logger::log_buffer::get(::vx::logger::priority::ERROR,    VX_LINE, VX_FILE)
#   define VX_LOG_CRITICAL_FULL ::vx::logger::log_buffer::get(::vx::logger::priority::CRITICAL, VX_LINE, VX_FILE)

#else

#   define VX_LOG_TRACE   
#   define VX_LOG_DEBUG   
#   define VX_LOG_INFO    
#   define VX_LOG_WARNING 
#   define VX_LOG_ERROR   
#   define VX_LOG_CRITICAL

#   define VX_LOG_TRACE_FULL   
#   define VX_LOG_DEBUG_FULL   
#   define VX_LOG_INFO_FULL    
#   define VX_LOG_WARNING_FULL 
#   define VX_LOG_ERROR_FULL   
#   define VX_LOG_CRITICAL_FULL

#endif

}