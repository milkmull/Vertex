#pragma once

#include <sstream>
#include <iomanip>
#include <memory>

#include "vertex/system/profiler.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace profile {

class profiler
{
public:

    profiler() {}
    ~profiler() { stop(); }

public:

    bool start(const char* output_file)
    {
        stop();

        m_file.open(output_file, os::file::mode::WRITE);
        if (m_file.is_open())
        {
            write_header();
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

private:

    void write_header()
    {
        // produces a csv where each line is:
        // name,thread_id,start_time,elapsed_time

        if (m_file.is_open())
        {
            std::ostringstream oss;

            oss << "name" << ','
                << "thread_id" << ','
                << "start_time" << ','
                << "elapsed_time" << '\n';

            m_file << oss.str();
            m_file.flush();
        }
    }

public:

    void record(const result& r)
    {
        if (m_file.is_open())
        {
            std::ostringstream oss;

            oss << std::setprecision(3) << std::fixed
                << r.name << ','
                << std::this_thread::get_id() << ','
                << r.start.as_nanoseconds() << ','
                << r.elapsed_time.as_nanoseconds() << '\n';

            m_file << oss.str();
            m_file.flush();
        }
    }

private:

    os::file m_file;
};

class profiler_wrapper
{
public:

    profiler_wrapper() : m_profiler(std::make_unique<profiler>()) {}

public:

    bool start(const char* output_file)
    {
        os::lock_guard lock(m_mutex);
        return m_profiler->start(output_file);
    }

    void stop()
    {
        os::lock_guard lock(m_mutex);
        m_profiler->stop();
    }

    bool is_enabled() const
    {
        os::lock_guard lock(m_mutex);
        return m_profiler->is_enabled();
    }

    void record(const result& r)
    {
        os::lock_guard lock(m_mutex);
        m_profiler->record(r);
    }

private:

    std::unique_ptr<profiler> m_profiler;
    mutable os::mutex m_mutex;
};

static profiler_wrapper s_profiler;

VX_API bool start(const char* output_file)
{
    return s_profiler.start(output_file);
}

VX_API void stop()
{
    s_profiler.stop();
}

VX_API bool is_enabled()
{
    return s_profiler.is_enabled();
}

VX_API void record(const result& r)
{
    s_profiler.record(r);
}

}
}