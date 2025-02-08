#pragma once

#include "vertex/system/log.hpp"
#include "vertex/util/io/iostream.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/mutex.hpp"

#ifdef ERROR
#   undef ERROR
#endif

namespace vx {
namespace log {

class logger
{
public:

    logger() = default;
    ~logger() { stop(); }

public:

    level get_level() const { return m_level; }
    void set_level(level log_level) { m_level = log_level; }

    bool start(const char* filename)
    {
        stop();

        m_file.open(filename, os::file::mode::WRITE);
        if (!m_file.is_open())
        {
            stop();
        }

        return m_file.is_open();
    }

    void stop()
    {
        m_file.close();
    }

    bool is_enabled() const
    {
        return m_file.is_open();
    }

    void write(level log_level, const std::string& msg)
    {
        if (log_level < m_level)
        {
            return;
        }

        io::println(msg);

        if (m_file.is_open())
        {
            m_file.write(msg.data(), msg.size());
            m_file.flush();
        }
    }

private:

    level m_level = level::TRACE;
    os::file m_file;
};

class logger_wrapper
{
public:

    logger_wrapper() : m_logger(std::make_unique<logger>()) {}

public:

    level get_level() const
    {
        os::lock_guard lock(m_mutex);
        return m_logger->get_level();
    }

    void set_level(level log_level)
    {
        os::lock_guard lock(m_mutex);
        m_logger->set_level(log_level);
    }

    bool start(const char* filename)
    {
        os::lock_guard lock(m_mutex);
        return m_logger->start(filename);
    }

    void stop()
    {
        os::lock_guard lock(m_mutex);
        m_logger->stop();
    }

    bool is_enabled() const
    {
        os::lock_guard lock(m_mutex);
        return m_logger->is_enabled();
    }

    void write(level log_level, const std::string& msg)
    {
        os::lock_guard lock(m_mutex);
        m_logger->write(log_level, msg);
    }

private:

    mutable os::mutex m_mutex;
    std::unique_ptr<logger> m_logger;
};

static logger_wrapper s_logger;

VX_API level get_level()
{
    return s_logger.get_level();
}
VX_API void set_level(level log_level)
{
    s_logger.set_level(log_level);
}

VX_API bool start(const char* output_file)
{
    return s_logger.start(output_file);
}

VX_API void stop()
{
    s_logger.stop();
}

VX_API bool is_enabled()
{
    return s_logger.is_enabled();
}

VX_API void write(level log_level, const std::string& msg)
{
    return s_logger.write(log_level, msg);
}

} // namespace log
} // namespace vx