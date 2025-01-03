#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_PROCESS)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/process.hpp"

namespace vx {
namespace os {

class process::process_impl
{
public:

    process_impl();
    ~process_impl();

public:

    bool start(process* p, const config& config);

    id get_pid() const;

    bool is_valid() const;
    bool is_alive() const;
    bool is_complete() const;

    bool join();
    bool kill(bool force);

    bool get_exit_code(int* exit_code) const;

private:

    PROCESS_INFORMATION m_process_information;
};

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_PROCESS