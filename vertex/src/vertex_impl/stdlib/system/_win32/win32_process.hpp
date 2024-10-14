#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/stdlib/system/process.hpp"

namespace vx {
namespace proc {

class process::process_impl
{
public:

    process_impl() = default;
    ~process_impl() = default;

public:

    bool start(process* p, const process_config& config);

public:

    bool is_valid() const;

private:

    PROCESS_INFORMATION m_process_information;

};

} // namespace proc
} // namespace vx

#endif