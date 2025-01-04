#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/filesystem.hpp"

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator Helpers
///////////////////////////////////////////////////////////////////////////////

void open_directory_iterator(const path& p, windows::handle& h, WIN32_FIND_DATAW& find_data, directory_entry& entry);
void advance_directory_iterator(const path& p, windows::handle& h, WIN32_FIND_DATAW& find_data, directory_entry& entry);
void update_directory_iterator_entry(const path& p, windows::handle& h, const WIN32_FIND_DATAW& find_data, directory_entry& entry);
void close_directory_iterator(windows::handle& h);

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator::directory_iterator_impl
{
    friend directory_iterator;

public:

    directory_iterator_impl(const path& p)
        : m_path(p)
    {
        open_directory_iterator(m_path, m_handle, m_find_data, m_entry);
    }

    ~directory_iterator_impl()
    {
        close();
    }

public:

    void close()
    {
        close_directory_iterator(m_handle);
    }

    void advance()
    {
        advance_directory_iterator(m_path, m_handle, m_find_data, m_entry);
    }

    bool is_valid() const { return m_handle.is_valid(); }

private:

    path m_path;
    windows::handle m_handle;
    WIN32_FIND_DATAW m_find_data{};
    directory_entry m_entry;
};

} // namespace filesystem
} // namespace os
} // namespace vx

#endif // __VX_OS_WINDOWS_FILESYSTEM