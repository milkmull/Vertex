#pragma once

#include "vertex/system/assert.hpp"
#include "vertex/util/time/timer.hpp"

namespace vx {
namespace profile {

VX_API bool start(const char* output_file);
VX_API void stop();
VX_API bool is_enabled();

struct result
{
    std::string name;
    time::time_point start;
    time::time_point elapsed_time;
};

VX_API void record(const result& r);

namespace __detail {

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

} // namespace __detail

#if defined(VX_ENABLE_PROFILING)

#   define VX_PROFILE_SCOPE(name) ::vx::profile::__detail::profile_timer timer##VX_LINE(name)
#   define VX_PROFILE_FUNCTION() ::vx::profile::__detail::profile_timer timer##VX_FUNCTION(VX_FUNCTION)

#else

#   define VX_PROFILE_SCOPE(name)
#   define VX_PROFILE_FUNCTION()

#endif

}
}