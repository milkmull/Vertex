#include "vertex/core/platform_config.hpp"

#if defined(VX_OS_WINDOWS_LIBRARY)

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include "vertex/os/library.hpp"

namespace vx {
namespace os {

class library::library_impl
{
public:

    library_impl();
    ~library_impl();

public:

    bool load(const std::string& lib);
    bool is_loaded() const;
    void free();

    void* get_function(const std::string& name) const;

private:

    HMODULE m_handle;
};

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_LIBRARY