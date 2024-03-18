#pragma once

#include <iostream>
#include <sstream>
#include <mutex>

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
    logger(logger&&) noexcept = delete;

    logger& operator=(const logger&) = delete;
    logger& operator=(logger&&) noexcept = delete;

    // =============== file stuff ===============

    void open_file()
    {
        close_file();
        m_file = std::fopen(m_filename, "a");

        if (m_file == nullptr)
        {
            std::printf("log: Failed to open file at %s\n", m_filename);
        }
    }

    void close_file()
    {
        if (m_file != nullptr)
        {
            std::fclose(m_file);
            m_file = nullptr;
        }
    }

public:

    static void start_file_output()
    {
        logger& log = get();
        log.m_filename = "log.txt";
        log.open_file();
    }

    static void start_file_output(const char* filename)
    {
        logger& log = get();
        log.m_filename = filename;
        log.open_file();
    }

    static void stop_file_output()
    {
        get().close_file();
    }

    // =============== logging ===============

    static logger& get()
    {
        static logger log;
        return log;
    }

    static inline priority get_priority() { return get().m_priority; }
    static void set_priority(priority priority) { get().m_priority = priority; }

    static void log(std::ostringstream& stream)
    {
        logger& log = get();
        log.m_mutex.lock();

        std::cout << stream.str();
        if (log.m_file)
        {
            std::fprintf(log.m_file, stream.str().c_str());
        }

        log.m_mutex.unlock();
    }

private:

    // =============== data ===============

    std::mutex m_mutex;
    priority m_priority = priority::CRITICAL;

    const char* m_filename = nullptr;
    FILE* m_file = nullptr;

};

class log_buffer
{
public:

    // =============== constructors and destructor ===============

    log_buffer(logger::priority priority)
        : m_priority(priority)
    {
        prefix();
    }

    log_buffer(logger::priority priority, int line, const char* filename)
        : m_priority(priority)
    {
        prefix();
        m_oss << "line " << line << " file " << filename << ": ";
    };

    ~log_buffer()
    {
        if (m_priority <= logger::get_priority())
        {
            if (m_oss.str().back() != '\n')
                m_oss << "\n";
            logger::log(m_oss);
        }

        m_oss.flush();
    }

    // =============== static constructors ===============

    static log_buffer get(logger::priority priority)
    {
        return log_buffer(priority);
    }

    static log_buffer get(logger::priority priority, int line, const char* file)
    {
        return log_buffer(priority, line, file);
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
    log_buffer& operator<<(std::ostream&(*fp)(std::ostream&))
    {
        m_oss << fp;
        return *this;
    }

    using ios_type = std::basic_ios<std::ostream::char_type, std::ostream::traits_type>;
    using manip2 = ios_type& (*)(ios_type&);
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

    // =============== log ===============

    template <typename T>
    void log(T&& message)
    {
        m_oss << std::forward<T>(message);
    }

private:

    void prefix()
    {
        switch (m_priority)
        {
            case logger::priority::TRACE:
                m_oss << "[TRACE] ";
                break;
            case logger::priority::DEBUG:
                m_oss << "[DEBUG] ";
                break;
            case logger::priority::INFO:
                m_oss << "[INFO] ";
                break;
            case logger::priority::WARNING:
                m_oss << "[WARNING] ";
                break;
            case logger::priority::ERROR:
                m_oss << "[ERROR] ";
                break;
            case logger::priority::CRITICAL:
                m_oss << "[CRITICAL] ";
                break;
        }
    }

private:

    // =============== data ===============

    std::ostringstream m_oss;
    logger::priority m_priority;

};

#define VX_LOG_TRACE         vx::log_buffer::get(vx::logger::priority::TRACE)
#define VX_LOG_DEBUG         vx::log_buffer::get(vx::logger::priority::DEBUG)
#define VX_LOG_INFO          vx::log_buffer::get(vx::logger::priority::INFO)
#define VX_LOG_WARNING       vx::log_buffer::get(vx::logger::priority::WARNING)
#define VX_LOG_ERROR         vx::log_buffer::get(vx::logger::priority::ERROR)
#define VX_LOG_CRITICAL      vx::log_buffer::get(vx::logger::priority::CRITICAL)

#define VX_LOG_TRACE_FULL    vx::log_buffer::get(vx::logger::priority::TRACE,    VX_LINE, VX_FILE)
#define VX_LOG_DEBUG_FULL    vx::log_buffer::get(vx::logger::priority::DEBUG,    VX_LINE, VX_FILE)
#define VX_LOG_INFO_FULL     vx::log_buffer::get(vx::logger::priority::INFO,     VX_LINE, VX_FILE)
#define VX_LOG_WARNING_FULL  vx::log_buffer::get(vx::logger::priority::WARNING,  VX_LINE, VX_FILE)
#define VX_LOG_ERROR_FULL    vx::log_buffer::get(vx::logger::priority::ERROR,    VX_LINE, VX_FILE)
#define VX_LOG_CRITICAL_FULL vx::log_buffer::get(vx::logger::priority::CRITICAL, VX_LINE, VX_FILE)

}