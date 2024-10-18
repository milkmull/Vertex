#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
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

    bool is_initialized() const;
    process_id get_pid() const;

    bool is_alive() const;
    bool kill(bool force);
    bool wait(bool block, int* exit_code);

private:

    PROCESS_INFORMATION m_process_information;

};

} // namespace os
} // namespace vx

#endif