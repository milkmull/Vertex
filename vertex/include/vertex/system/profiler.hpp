#pragma once

#include "vertex/util/time/timer.hpp"

namespace vx {
namespace profile {

/**
 * @brief Starts the profiling system and opens the output file for results.
 *
 * @param output_file The path to the file where profiling data will be written.
 * @return true if the profiler started successfully; false otherwise.
 */
VX_API bool start(const char* output_file);

/**
 * @brief Stops the profiling system and flushes any pending results to file.
 */
VX_API void stop();

/**
 * @brief Returns whether the profiling system is currently enabled.
 *
 * @return true if profiling is enabled; false otherwise.
 */
VX_API bool is_enabled();

/**
 * @struct result
 * @brief Represents a single profiling result.
 *
 * Stores the name of the profiled region, the start time, and the elapsed time.
 */
struct result
{
    std::string name;
    time::time_point start;
    time::time_point elapsed_time;
};

/**
 * @brief Records a profiling result.
 *
 * Typically called internally by RAII timers like `profile_timer`.
 *
 * @param r The profiling result to be recorded.
 */
VX_API void record(const result& r);

namespace _priv {

class profile_timer
{
public:

    profile_timer(const char* name)
        : m_name(name)
    {
        m_timer.start();
    }

    ~profile_timer()
    {
        stop();
    }

    void stop()
    {
        if (m_timer.running())
        {
            m_timer.stop();
            record({ m_name, m_timer.start_time(), m_timer.elapsed_time() });
        }
    }

private:

    const char* m_name;
    time::timer m_timer;
};

} // namespace _priv

#if defined(VX_ENABLE_PROFILING)

#   define VX_PROFILE_START(file) ::vx::profile::start(file)
#   define VX_PROFILE_STOP()      ::vx::profile::stop()

#   define VX_PROFILE_SCOPE(name) ::vx::profile::_priv::profile_timer timer##VX_LINE(name)
#   define VX_PROFILE_FUNCTION()  ::vx::profile::_priv::profile_timer timer##VX_FUNCTION(VX_FUNCTION)

#else

#   define VX_PROFILE_START(file)
#   define VX_PROFILE_STOP()

#   define VX_PROFILE_SCOPE(name)
#   define VX_PROFILE_FUNCTION()

#endif

}
}