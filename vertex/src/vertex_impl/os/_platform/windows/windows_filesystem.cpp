#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/system/error.hpp"
#include "vertex/os/filesystem.hpp"

namespace vx {
namespace os {
namespace filesystem {

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

static file_permissions::type get_file_permissions(const path& p, const DWORD attrs)
{
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return file_permissions::NONE;
    }

    file_permissions::type permissions = file_permissions::OWNER_READ | file_permissions::GROUP_READ | file_permissions::OTHERS_READ;

    if (!(attrs & FILE_ATTRIBUTE_READONLY))
    {
        permissions |= file_permissions::OWNER_WRITE | file_permissions::GROUP_WRITE | file_permissions::OTHERS_WRITE;
    }

    const auto ext = p.extension().native();
    if ((ext == L".exe" || ext == L".EXE") ||
        (ext == L".com" || ext == L".COM") ||
        (ext == L".bat" || ext == L".BAT") ||
        (ext == L".cmd" || ext == L".CMD"))
    {
        permissions |= file_permissions::OWNER_EXEC | file_permissions::GROUP_EXEC | file_permissions::OTHERS_EXEC;
    }

    return permissions;
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

    const DWORD attrs = fi.dwFileAttributes;
    info.type = get_file_type(p, attrs);
    info.permissions = get_file_permissions(p, attrs);

    info.size = (static_cast<size_t>(fi.nFileSizeHigh) << 32) | fi.nFileSizeLow;
    info.hard_link_count = fi.nNumberOfLinks;

    info.create_time = time::time_point::from_windows_file_time(fi.ftCreationTime.dwLowDateTime, fi.ftCreationTime.dwHighDateTime);
    info.modify_time = time::time_point::from_windows_file_time(fi.ftLastWriteTime.dwLowDateTime, fi.ftLastWriteTime.dwHighDateTime);

    return info;
}

VX_API file_info get_file_info(const path& p)
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

VX_API file_info get_symlink_info(const path& p)
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
        windows::error_message("CreateFileW()");
        return info;
    }

    return file_info_from_handle(h, p);
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542

VX_API path read_symlink(const path& p)
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

} // namespace filesystem
} // namespace os
} // namespace vx

#endif // __VX_OS_WINDOWS_FILESYSTEM