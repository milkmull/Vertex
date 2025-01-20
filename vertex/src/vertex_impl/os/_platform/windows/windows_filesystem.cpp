#include "vertex_impl/os/_platform/windows/windows_filesystem.hpp"

#include <shlobj_core.h>
#include <winioctl.h> // FSCTL_GET_REPARSE_POINT

#include "vertex/system/error.hpp"
#include "vertex/os/shared_library.hpp"

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer

typedef struct REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

static bool update_file_permissions_internal(
    const path& p,
    bool read_only,
    bool follow_symlinks
)
{
    const DWORD attrs = GetFileAttributesW(p.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        windows::error_message("GetFileAttributesW()");
        return false;
    }

    const DWORD read_only_test = read_only ? FILE_ATTRIBUTE_READONLY : 0;

    if ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) && follow_symlinks)
    {
        // Resolve the symbolic link
        windows::handle h = CreateFileW(
            p.c_str(),
            FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,
            NULL,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );

        if (!h.is_valid())
        {
            windows::error_message("CreateFileW()");
            return false;
        }
        
        FILE_BASIC_INFO basic_info{};
        if (!GetFileInformationByHandleEx(h.get(), FileBasicInfo, &basic_info, sizeof(basic_info)))
        {
            windows::error_message("GetFileInformationByHandleEx()");
            return false;
        }

        if ((basic_info.FileAttributes & FILE_ATTRIBUTE_READONLY) == read_only_test)
        {
            // Nothing to do
            return true;
        }

        basic_info.FileAttributes ^= FILE_ATTRIBUTE_READONLY;
        if (!SetFileInformationByHandle(h.get(), FileBasicInfo, &basic_info, sizeof(basic_info)))
        {
            windows::error_message("SetFileInformationByHandle()");
            return false;
        }
    }
    else
    {
        if ((attrs & FILE_ATTRIBUTE_READONLY) == read_only_test)
        {
            // Nothing to do
            return true;
        }

        if (!SetFileAttributesW(p.c_str(), attrs ^ FILE_ATTRIBUTE_READONLY))
        {
            windows::error_message("SetFileAttributesW()");
            return false;
        }
    }

    return true;
}

bool update_file_permissions_impl(
    const path& p,
    typename file_permissions::type permissions,
    file_permission_operator op,
    bool follow_symlinks
)
{
    const auto write_perms = (permissions & file_permissions::ALL_WRITE);
    bool read_only = false;

    switch (op)
    {
        case file_permission_operator::REPLACE:
        {
            // always apply FILE_ATTRIBUTE_READONLY according to permissions
            read_only = (write_perms == file_permissions::NONE);
            break;
        }
        case file_permission_operator::ADD:
        {
            if (write_perms == file_permissions::NONE)
            {
                // if we aren't adding any write bits, then we won't change
                // FILE_ATTRIBUTE_READONLY, so there's nothing to do
                return true;
            }

            read_only = false;
            break;
        }
        case file_permission_operator::REMOVE:
        {
            if (write_perms != file_permissions::ALL_WRITE)
            {
                // if we aren't removing all write bits, then we won't change
                // FILE_ATTRIBUTE_READONLY, so there's nothing to do
                return true;
            }

            read_only = true;
            break;
        }
    }

    return update_file_permissions_internal(p, read_only, follow_symlinks);
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

union reparse_point_data
{
    REPARSE_DATA_BUFFER rdb;
    BYTE buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
};

static bool get_reparse_point_data_from_handle(
    const windows::handle& h,
    std::unique_ptr<reparse_point_data>& data
)
{
    if (!h.is_valid())
    {
        return false;
    }

    data = std::make_unique<reparse_point_data>();
    if (!data)
    {
        VX_ERR(err::OUT_OF_MEMORY);
        return false;
    }

    DWORD count = 0;

    if (!DeviceIoControl(
        h.get(),
        FSCTL_GET_REPARSE_POINT,
        NULL,
        0,
        data->buffer,
        static_cast<DWORD>(sizeof(data->buffer)),
        &count,
        NULL))
    {
        windows::error_message("DeviceIoControl()");
        return false;
    }

    return true;
}

static bool get_reparse_point_data(const path& p, std::unique_ptr<reparse_point_data>& data)
{
    // Open the file to check if it's a symbolic link
    windows::handle h = CreateFileW(
        p.c_str(),
        0,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    return h.is_valid() && get_reparse_point_data_from_handle(h, data);
}

// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/operations.cpp#L1684

#define is_symlink_reparse_tag(tag) ((tag) == IO_REPARSE_TAG_SYMLINK)

static file_type get_file_type(const path& p, const DWORD attrs)
{
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        const DWORD error = GetLastError();
        return (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) ? file_type::NOT_FOUND : file_type::UNKNOWN;
    }

    if (attrs & FILE_ATTRIBUTE_DIRECTORY)
    {
        return file_type::DIRECTORY;
    }

    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        std::unique_ptr<reparse_point_data> reparse_data;

        if (get_reparse_point_data(p, reparse_data) &&
            is_symlink_reparse_tag(reparse_data->rdb.ReparseTag))
        {
            return file_type::SYMLINK;
        }
    }

    return file_type::REGULAR;
}

static file_type file_type_from_error(const DWORD error)
{
    if (error == ENOENT || error == ENOTDIR)
    {
        return file_type::NOT_FOUND;
    }
    else if (error == ERROR_SHARING_VIOLATION)
    {
        return file_type::UNKNOWN;
    }

    return file_type::NONE;
}

// https://github.com/boostorg/filesystem/blob/0848f5347b69bd9f8f0459dc0cb88e7a52714448/src/windows_tools.hpp#L89

static typename file_permissions::type get_file_permissions(const path& p, const DWORD attrs)
{
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return file_permissions::NONE;
    }

    typename file_permissions::type permissions = file_permissions::OWNER_READ | file_permissions::GROUP_READ | file_permissions::OTHERS_READ;

    if (!(attrs & FILE_ATTRIBUTE_READONLY))
    {
        permissions |= file_permissions::OWNER_WRITE | file_permissions::GROUP_WRITE | file_permissions::OTHERS_WRITE;
    }

    const auto& ext = p.extension().native();
    if ((ext == L".exe" || ext == L".EXE") ||
        (ext == L".com" || ext == L".COM") ||
        (ext == L".bat" || ext == L".BAT") ||
        (ext == L".cmd" || ext == L".CMD"))
    {
        permissions |= file_permissions::OWNER_EXEC | file_permissions::GROUP_EXEC | file_permissions::OTHERS_EXEC;
    }

    return permissions;
}

static file_info create_file_info(
    const path& p,
    const DWORD dwFileAttributes,
    const DWORD nFileSizeHigh,
    const DWORD nFileSizeLow,
    const FILETIME ftCreationTime,
    const FILETIME ftLastWriteTime
)
{
    return file_info{
        get_file_type(p, dwFileAttributes),
        get_file_permissions(p, dwFileAttributes),
        (static_cast<size_t>(nFileSizeHigh) << 32) | nFileSizeLow,
        time::time_point::from_windows_file_time(ftCreationTime.dwLowDateTime, ftCreationTime.dwHighDateTime),
        time::time_point::from_windows_file_time(ftLastWriteTime.dwLowDateTime, ftLastWriteTime.dwHighDateTime)
    };
}

static file_info file_info_from_handle(const windows::handle& h, const path& p)
{
    file_info info{};

    if (!h.is_valid())
    {
        return info;
    }

    BY_HANDLE_FILE_INFORMATION fi;
    if (!GetFileInformationByHandle(h.get(), &fi))
    {
        windows::error_message("GetFileInformationByHandle()");
        return info;
    }

    info = create_file_info(
        p,
        fi.dwFileAttributes,
        fi.nFileSizeHigh,
        fi.nFileSizeLow,
        fi.ftCreationTime,
        fi.ftLastWriteTime
    );

    return info;
}

file_info get_file_info_impl(const path& p)
{
    file_info info = get_symlink_info(p);

    if (info.type == file_type::SYMLINK)
    {
        // Resolve the symbolic link
        windows::handle h = CreateFileW(
            p.c_str(),
            FILE_READ_ATTRIBUTES | FILE_READ_EA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );

        if (!h.is_valid())
        {
            windows::error_message("CreateFileW()");
            info.type = file_type_from_error(GetLastError());
        }
        else
        {
            info = file_info_from_handle(h, p);
        }
    }

    return info;
}

file_info get_symlink_info_impl(const path& p)
{
    file_info info{};

    windows::handle h = CreateFileW(
        p.c_str(),
        FILE_READ_ATTRIBUTES | FILE_READ_EA,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL
    );

    if (!h.is_valid())
    {
        //windows::error_message("CreateFileW()");
        return info;
    }

    return file_info_from_handle(h, p);
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542

path read_symlink_impl(const path& p)
{
    path symlink_path;

    std::unique_ptr<reparse_point_data> reparse_data;
    if (!get_reparse_point_data(p, reparse_data))
    {
        return symlink_path;
    }

    const wchar_t* buffer = nullptr;
    size_t size = 0;
    size_t offset = 0;

    switch (reparse_data->rdb.ReparseTag)
    {
        case IO_REPARSE_TAG_SYMLINK:
        {
            buffer = reinterpret_cast<const wchar_t*>(reparse_data->rdb.SymbolicLinkReparseBuffer.PathBuffer);

            if (reparse_data->rdb.SymbolicLinkReparseBuffer.PrintNameLength != 0)
            {
                size = reparse_data->rdb.SymbolicLinkReparseBuffer.PrintNameLength / sizeof(wchar_t);
                offset = reparse_data->rdb.SymbolicLinkReparseBuffer.PrintNameOffset / sizeof(wchar_t);
            }
            else
            {
                size = reparse_data->rdb.SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(wchar_t);
                offset = reparse_data->rdb.SymbolicLinkReparseBuffer.SubstituteNameOffset / sizeof(wchar_t);
            }

            break;
        }
        default:
        {
            return symlink_path;
        }
    }

    symlink_path.assign(buffer + offset, buffer + offset + size);
    return symlink_path;
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

path get_current_path_impl()
{
    const DWORD size = GetCurrentDirectoryW(0, NULL);
    std::vector<WCHAR> data(size);

    if (!GetCurrentDirectoryW(size, data.data()))
    {
        windows::error_message("GetCurrentDirectoryW()");
        return {};
    }

    return path{ data.data() };
}

bool set_current_path_impl(const path& p)
{
    if (!SetCurrentDirectoryW(p.c_str()))
    {
        windows::error_message("SetCurrentDirectoryW()");
        return false;
    }

    return true;
}

path absolute_impl(const path& p)
{
    const DWORD size = GetFullPathNameW(p.c_str(), 0, NULL, NULL);
    std::vector<WCHAR> data(size);

    if (!GetFullPathNameW(p.c_str(), size, data.data(), NULL))
    {
        windows::error_message("GetFullPathNameW()");
        return {};
    }

    return path{ data.data() };
}

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

path get_temp_path_impl()
{
    static path temp_path_cache;

    if (temp_path_cache.empty())
    {
        wchar_t buffer[MAX_PATH];
        DWORD size = 0;

        // Attermpt to call GetTempPath2W first
        do
        {
            shared_library kernel32;
            if (!kernel32.load("kernel32.dll"))
            {
                break;
            }

            using GetTempPath2W_t = DWORD(WINAPI*)(DWORD, LPWSTR, PSID);
            auto GetTempPath2W = kernel32.get_function<GetTempPath2W_t>("GetTempPath2W");
            if (!GetTempPath2W)
            {
                break;
            }

            size = GetTempPath2W(MAX_PATH, buffer, NULL);

        } while (0);

        if (size == 0)
        {
            size = GetTempPathW(MAX_PATH, buffer);
        }

        if (size == 0)
        {
            windows::error_message("GetTempPathW()");
        }
        else
        {
            // Set the cache
            temp_path_cache.assign(buffer);
        }
    }

    return temp_path_cache;
}

path get_user_folder_impl(user_folder folder)
{
    path p;

    KNOWNFOLDERID id{};
    switch (folder)
    {
        case user_folder::HOME:
        {
            id = FOLDERID_Profile;
            break;
        }
        case user_folder::DESKTOP:
        {
            id = FOLDERID_Desktop;
            break;
        }
        case user_folder::DOCUMENTS:
        {
            id = FOLDERID_Documents;
            break;
        }
        case user_folder::DOWNLOADS:
        {
            id = FOLDERID_Downloads;
            break;
        }
        case user_folder::MUSIC:
        {
            id = FOLDERID_Music;
            break;
        }
        case user_folder::PICTURES:
        {
            id = FOLDERID_Pictures;
            break;
        }
        case user_folder::VIDEOS:
        {
            id = FOLDERID_Videos;
            break;
        }
    }

    PWSTR szPath;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, NULL, &szPath)))
    {
        p.assign(szPath);
    }
    else
    {
        windows::error_message("SHGetKnownFolderPath()");
    }
    CoTaskMemFree(szPath);

    return p;
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

bool create_file_impl(const path& p)
{
    const windows::handle h = CreateFileW(
        p.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS, // Open the file if it exists, or create it if it doesn't
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (!h.is_valid())
    {
        windows::error_message("CreateFileW()");
        return false;
    }

    return true;
}

static bool create_symlink_impl(const path& target, const path& link, DWORD flags)
{
    if (!CreateSymbolicLinkW(link.c_str(), target.c_str(), flags))
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            windows::error_message("CreateSymbolicLinkW()");
            return false;
        }
    }

    return true;
}

bool create_symlink_impl(const path& target, const path& link)
{
    return create_symlink_impl(target, link, 0);
}

bool create_directory_symlink_impl(const path& target, const path& link)
{
    return create_symlink_impl(target, link, SYMBOLIC_LINK_FLAG_DIRECTORY);
}

bool create_directory_impl(const path& p)
{
    if (!CreateDirectoryW(p.c_str(), NULL))
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            windows::error_message("CreateDirectoryW()");
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/copy_file

bool copy_file_impl(const path& from, const path& to, bool overwrite_existing)
{
    if (!CopyFileW(from.c_str(), to.c_str(), !overwrite_existing))
    {
        windows::error_message("CopyFileW()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/04081522161549b18fa0b0fae0d2a531ee967958/stl/src/filesystem.cpp#L720

bool rename_impl(const path& from, const path& to)
{
    if (!MoveFileExW(from.c_str(), to.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
    {
        windows::error_message("MoveFileExW()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

__detail::remove_error remove_impl(const path& p, bool in_recursive_remove)
{
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(p.c_str(), GetFileExInfoStandard, &data))
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            // Path is already gone
            return __detail::remove_error::FILE_NOT_FOUND;
        }

        windows::error_message("GetFileAttributesExW()");
        return __detail::remove_error::OTHER;
    }

    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (!RemoveDirectoryW(p.c_str()))
        {
            if (GetLastError() == ERROR_DIR_NOT_EMPTY)
            {
                if (!in_recursive_remove)
                {
                    windows::error_message("RemoveDirectoryW()");
                }
                return __detail::remove_error::DIRECTORY_NOT_EMPTY;
            }

            windows::error_message("RemoveDirectoryW()");
            return __detail::remove_error::OTHER;
        }
    }
    else
    {
        if (!DeleteFileW(p.c_str()))
        {
            windows::error_message("DeleteFileW()");
            return __detail::remove_error::OTHER;
        }
    }

    return __detail::remove_error::NONE;
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

bool space_impl(const path& p, space_info& info)
{
    ULARGE_INTEGER capacity{};
    ULARGE_INTEGER free{};
    ULARGE_INTEGER available{};

    if (!GetDiskFreeSpaceExW(p.c_str(), &available, &capacity, &free))
    {
        windows::error_message("GetDiskFreeSpaceExW()");
        return false;
    }

    info.capacity = capacity.QuadPart;
    info.free = free.QuadPart;
    info.available = available.QuadPart;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator Helpers
///////////////////////////////////////////////////////////////////////////////

static bool is_dot_or_dotdot(const wchar_t* filename)
{
    if (filename[0] != L'.')
    {
        return false;
    }

    const auto second_char = filename[1];
    if (second_char == 0)
    {
        return true;
    }

    if (second_char != L'.')
    {
        return false;
    }

    return filename[2] == 0;
}

static void close_directory_iterator(windows::handle& h)
{
    if (h.is_valid() && !FindClose(h.get()))
    {
        std::abort();
    }

    h.reset();
}

static void update_directory_iterator_entry(const path& p, directory_entry& entry, windows::handle& h, const WIN32_FIND_DATAW& find_data)
{
    if (h.is_valid())
    {
        entry.path = p / find_data.cFileName;
        entry.info = create_file_info(
            entry.path,
            find_data.dwFileAttributes,
            find_data.nFileSizeHigh,
            find_data.nFileSizeLow,
            find_data.ftCreationTime,
            find_data.ftLastAccessTime
        );
    }
    else
    {
        close_directory_iterator(h);
    }
}

static void advance_directory_iterator_once(const path& p, directory_entry& entry, windows::handle& h, WIN32_FIND_DATAW& find_data)
{
    if (FindNextFileW(h.get(), &find_data))
    {
        update_directory_iterator_entry(p, entry, h, find_data);
    }
    else
    {
        close_directory_iterator(h);
    }
}

static void advance_directory_iterator(const path& p, directory_entry& entry, windows::handle& h, WIN32_FIND_DATAW& find_data)
{
    do
    {
        advance_directory_iterator_once(p, entry, h, find_data);

    } while (h.is_valid() && is_dot_or_dotdot(find_data.cFileName));
}

static void open_directory_iterator(const path& p, directory_entry& entry, windows::handle& h, WIN32_FIND_DATAW& find_data)
{
    close_directory_iterator(h);

    // Append wildcard to search for all items
    const path pattern = p / L"*";

    h = FindFirstFileExW(
        pattern.c_str(),
        FindExInfoBasic,
        &find_data,
        FindExSearchNameMatch,
        NULL,
        0
    );

    while (h.is_valid() && is_dot_or_dotdot(find_data.cFileName))
    {
        advance_directory_iterator_once(p, entry, h, find_data);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

void directory_iterator::directory_iterator_impl::open()
{
    open_directory_iterator(m_path, m_entry, m_handle, m_find_data);
}

void directory_iterator::directory_iterator_impl::close()
{
    close_directory_iterator(m_handle);
}

void directory_iterator::directory_iterator_impl::advance()
{
    advance_directory_iterator(m_path, m_entry, m_handle, m_find_data);
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

bool recursive_directory_iterator::recursive_directory_iterator_impl::push_stack()
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

void recursive_directory_iterator::recursive_directory_iterator_impl::pop_stack()
{
    m_path.pop_back();
    close_directory_iterator(m_stack.back());
    m_stack.pop_back();
}

void recursive_directory_iterator::recursive_directory_iterator_impl::advance()
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

} // namespace filesystem
} // namespace os
} // namespace vx