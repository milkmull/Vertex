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

void open_directory_iterator(const path& p, directory_entry& entry, windows::handle& h, WIN32_FIND_DATAW& find_data);
void advance_directory_iterator(const path& p, directory_entry& entry, windows::handle& h, WIN32_FIND_DATAW& find_data);
void update_directory_iterator_entry(const path& p, directory_entry& entry, windows::handle& h, const WIN32_FIND_DATAW& find_data);
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
        open_directory_iterator(m_path, m_entry, m_handle, m_find_data);
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
        advance_directory_iterator(m_path, m_entry, m_handle, m_find_data);
    }

    bool is_valid() const { return m_handle.is_valid(); }

private:

    const path m_path;
    directory_entry m_entry;
    windows::handle m_handle;
    WIN32_FIND_DATAW m_find_data{};
};

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class recursive_directory_iterator::recursive_directory_iterator_impl
{
    friend recursive_directory_iterator;

public:

    recursive_directory_iterator_impl(const path& p)
        : m_path(p)
    {
        push_stack();
    }

    ~recursive_directory_iterator_impl()
    {
        close();
    }

private:

    void close()
    {
        while (!m_stack.empty())
        {
            pop_stack();
        }
    }

    bool push_stack()
    {
        m_path /= m_entry.path.filename();
        windows::handle h;
        open_directory_iterator(m_path, m_entry, h, m_find_data);

        if (!h.is_valid())
        {
            close_directory_iterator(h);
            return false;
        }

        m_stack.push_back(std::move(h));
        return true;
    }

    void pop_stack()
    {
        m_path.pop_back();
        close_directory_iterator(m_stack.back());
        m_stack.pop_back();
    }

public:

    void advance()
    {
        windows::handle* current = &m_stack.back();

        if (m_recursion_pending && push_stack())
        {
            current = &m_stack.back();
        }
        else
        {
            advance_directory_iterator(m_path, m_entry, *current, m_find_data);
        }

        while (!current->is_valid())
        {
            pop_stack();
            if (m_stack.empty())
            {
                break;
            }

            current = &m_stack.back();
            advance_directory_iterator(m_path, m_entry, *current, m_find_data);
        }

        m_recursion_pending = m_entry.is_directory();
    }

    bool is_valid() const { return !m_stack.empty() && m_stack.back().is_valid(); }

    size_t depth() const { return m_stack.size(); }

    bool recursion_pending() const { return m_recursion_pending; }
    void disable_pending_recursion() { m_recursion_pending = false; }

    void pop() { pop_stack(); }

private:

    path m_path;
    directory_entry m_entry;
    std::vector<windows::handle> m_stack;
    WIN32_FIND_DATAW m_find_data{};
    bool m_recursion_pending = false;
};

} // namespace filesystem
} // namespace os
} // namespace vx

#endif // __VX_OS_WINDOWS_FILESYSTEM