#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/shared_library.hpp"

namespace vx {
namespace os {

class shared_library::shared_library_impl
{
public:

    shared_library_impl();
    ~shared_library_impl();

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