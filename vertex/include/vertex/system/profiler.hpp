#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>

#include "vertex/stdlib/time.h"

namespace vx {
namespace profile {

class profiler
{
private:

    ///////////////////////////////////////////////////////////////////////////////
    // types
    ///////////////////////////////////////////////////////////////////////////////

    struct result
    {
        std::string name;
        std::chrono::duration<double, std::micro> start;
        std::chrono::microseconds elapsed_time;
    };

    struct result2
    {
        std::string name;
        uint64_t start;
        uint64_t elapsed_time;
    };

    struct session
    {
        const char* name;
    };

public:

    class timer
    {
    public:

        timer(const char* name) : m_name(name), m_start_time(std::chrono::steady_clock::now()), m_stopped(false) {}
        ~timer() { if (!m_stopped) stop(); }

        void stop()
        {
            auto end_time = std::chrono::steady_clock::now();
            auto high_res_start = std::chrono::duration<double, std::micro>{ m_start_time.time_since_epoch() };
            auto elapsed_time = (
                std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch() -
                std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time).time_since_epoch()
                );

            profiler::get().write_profile(result{ m_name, high_res_start, elapsed_time });

            m_stopped = true;
        }

    private:

        const char* m_name;
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        bool m_stopped;
    };

    class timer2
    {
    public:

        timer2(const char* name) : m_name(name), high_res_start(m_timer.start()) {}
        ~timer2() { if (m_timer.running()) stop(); }

        void stop()
        {
            m_timer.stop();
            uint64_t elapsed_time = time::nanoseconds_to_microseconds(m_timer.elapsed());

            profiler::get().write_profile2(result2{ m_name, high_res_start, elapsed_time });
        }

    private:

        const char* m_name;
        uint64_t high_res_start;
        time::timer m_timer;
    };

private:

    profiler() {}
    ~profiler() { end_session(); }

    profiler(const profiler&) = delete;
    profiler(profiler&&) = delete;

    profiler& operator=(const profiler&) = delete;
    profiler& operator=(profiler&&) = delete;

public:

    static profiler& get()
    {
        static profiler instance;
        return instance;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // session stuff
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Begins a new profiling session.
    ///
    /// @param name The name of the profiling session.
    /// @param filename The name of the file to write profiling results to
    /// (defaults to "results.txt").
    ///////////////////////////////////////////////////////////////////////////////
    static void begin_session(const char* name, const char* filename = "results.txt")
    {
        get().begin_session_internal(name, filename);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Ends the current profiling session.
    ///////////////////////////////////////////////////////////////////////////////
    static void end_session()
    {
        get().end_session_internal();
    }

private:

    void begin_session_internal(const char* name, const char* filename)
    {
        m_filename = filename;
        m_output_stream.open(filename, std::ios_base::out);

        if (m_output_stream.is_open())
        {
            write_header();
            m_current_session = new session{ name };
        }
        else
        {
            std::printf("profiler: Failed to open file at %s\n", filename);
        }
    }

    void end_session_internal()
    {
        m_output_stream.close();

        if (m_current_session)
        {
            delete m_current_session;
            m_current_session = nullptr;
        }
    }

    void write_header()
    {
        // produces a csv where each line is:
        // name,thread_id,start_time,elapsed_time

        std::ostringstream entry;

        entry << "name" << ',';
        entry << "thread_id" << ',';
        entry << "start_time" << ',';
        entry << "elapsed_time" << '\n';

        if (m_output_stream.is_open())
        {
            m_output_stream << entry.str();
            m_output_stream.flush();
        }
    }

    void write_profile(const result& result)
    {
        std::ostringstream entry;

        entry << std::setprecision(3) << std::fixed;
        entry << result.name << ',';
        entry << std::this_thread::get_id() << ',';
        entry << result.start.count() << ',';
        entry << result.elapsed_time.count() << '\n';

        if (m_current_session && m_output_stream.is_open())
        {
            m_mutex.lock();

            m_output_stream << entry.str();
            m_output_stream.flush();

            m_mutex.unlock();
        }
    }

    void write_profile2(const result2& result)
    {
        std::ostringstream entry;

        entry << std::setprecision(3) << std::fixed;
        entry << result.name << ',';
        entry << std::this_thread::get_id() << ',';
        entry << result.start << ',';
        entry << result.elapsed_time << '\n';

        if (m_current_session && m_output_stream.is_open())
        {
            m_mutex.lock();

            m_output_stream << entry.str();
            m_output_stream.flush();

            m_mutex.unlock();
        }
    }

private:

    std::mutex m_mutex;
    const char* m_filename = nullptr;
    session* m_current_session = nullptr;
    std::ofstream m_output_stream;

};

#if defined(VX_ENABLE_PROFILING)
#   define VX_PROFILE_SCOPE1(name) ::vx::profile::profiler::timer  timer##VX_LINE(name)
#   define VX_PROFILE_SCOPE2(name) ::vx::profile::profiler::timer2 timer##VX_LINE(name)
#else
#   define VX_PROFILE_SCOPE1(name) 
#   define VX_PROFILE_SCOPE2(name) 
#endif

}
}