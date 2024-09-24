#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/stdlib/file/filesystem.hpp"
#include "vertex/stdlib/string/string.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Ops
///////////////////////////////////////////////////////////////////////////////

bool exists(const std::string& path)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);
    const DWORD attributes = GetFileAttributesW(wpath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

bool create_file(const std::string& path, bool fail_if_exists)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);

    HANDLE handle = CreateFileW(
        wpath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_EXISTS)
        {
            if (fail_if_exists)
            {
                VX_ERROR(error::error_code::FILE_ERROR) << "File already exists: " << path;
            }
            else
            {
                return true;
            }
        }
        else
        {
            VX_ERROR(error::error_code::FILE_ERROR) << "Failed to create file: " << path;
        }
        return false;
    }

    CloseHandle(handle);
    return true;
}

bool create_directory(const std::string& path, bool fail_if_exists)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);

    if (!CreateDirectoryW(wpath.c_str(), NULL))
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            if (fail_if_exists)
            {
                VX_ERROR(error::error_code::FILE_ERROR) << "Directory already exists: " << path;
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            VX_ERROR(error::error_code::FILE_ERROR) << "Failed to create directory: " << path;
        }
        return false;
    }

    return true;
}

bool copy(const std::string& old_path, const std::string& new_path, copy_options::type options)
{
    const std::wstring wold_path = vx::str::string_to_wstring(old_path);
    const std::wstring wnew_path = vx::str::string_to_wstring(new_path);

    WIN32_FILE_ATTRIBUTE_DATA info{};
    if (!GetFileAttributesExW(wold_path.c_str(), GetFileExInfoStandard, &info))
    {
        // error
        return false;
    }

    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (!CreateDirectoryW(wnew_path.c_str(), NULL) && (GetLastError() != ERROR_ALREADY_EXISTS))
        {
            // error
            return false;
        }

        if (options & copy_options::RECURSIVE)
        {
            const std::vector<std::string> files = list(old_path);
            for (const std::string& path : files)
            {
                if (!filesystem::copy(old_path + "\\" + path, new_path + "\\" + path, options))
                {
                    // error
                    return false;
                }
            }
        }

        return true;
    }
    else
    {
        if (options & copy_options::DIRECTORY_ONLY)
        {
            return true;
        }

        if ((options & copy_options::SKIP_EXISTING) && exists(new_path))
        {
            return true;
        }

        const bool overwrite = (options & copy_options::OVERWRITE_EXISTING);
        if (!CopyFileW(wold_path.c_str(), wnew_path.c_str(), !overwrite))
        {
            VX_ERROR(error::error_code::FILE_ERROR) << "File already exists: " << new_path;
            return false;
        }
    }

    return true;
}

bool rename(const std::string& old_path, const std::string& new_path)
{
    const std::wstring wold_path = vx::str::string_to_wstring(old_path);
    const std::wstring wnew_path = vx::str::string_to_wstring(new_path);
    return MoveFileExW(wold_path.c_str(), wnew_path.c_str(), MOVEFILE_REPLACE_EXISTING);
}

bool remove(const std::string& path)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);

    WIN32_FILE_ATTRIBUTE_DATA info{};
    if (!GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &info))
    {
        // ERROR_PATH_NOT_FOUND means a parent dir is missing and we consider that an error
        return (GetLastError() == ERROR_FILE_NOT_FOUND);
    }

    const bool is_dir = (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    return is_dir ? RemoveDirectoryW(wpath.c_str()) : DeleteFileW(wpath.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

std::string get_absolute_path(const std::string& path)
{
    std::string abs_path;
    const std::wstring wpath = vx::str::string_to_wstring(path);

    const DWORD size = GetFullPathNameW(wpath.c_str(), 0, NULL, NULL);
    std::vector<WCHAR> data(size);

    if (GetFullPathNameW(wpath.c_str(), size, data.data(), NULL))
    {
        str::utf8::from_wide(data.begin(), data.end() - 1, std::back_inserter(abs_path));
    }

    return abs_path;
}

std::string get_current_directory()
{
    std::string path;

    const DWORD size = GetCurrentDirectoryW(0, NULL);
    std::vector<WCHAR> data(size);

    if (GetCurrentDirectoryW(size, data.data()))
    {
        str::utf8::from_wide(data.begin(), data.end() - 1, std::back_inserter(path));
    }

    return path;
}

bool set_current_directory(const std::string& path)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);
    return SetCurrentDirectoryW(wpath.c_str());
}

std::vector<std::string> list(const std::string& path, bool absolute)
{
    // Append wildcard to search for all items
    const std::wstring wpath = vx::str::string_to_wstring(path) + L"\\*";
    std::vector<std::string> subdirectories;

    WIN32_FIND_DATA find_file_data{};
    const HANDLE handle = FindFirstFileW(wpath.c_str(), &find_file_data);

    if (handle == INVALID_HANDLE_VALUE)
    {
        // Return empty list if the directory is not accessible
        VX_ERROR(error::error_code::FILE_NOT_FOUND) << "Failed to find first file in directory: " << path;
        return subdirectories;
    }

    do
    {
        const WCHAR* wname = find_file_data.cFileName;

        // Exclude the current directory (.) and the parent directory (..)
        if (wname[0] == '.')
        {
            if ((wname[1] == '\0') || ((wname[1] == '.') && (wname[2] == '\0')))
            {
                continue;
            }
        }

        std::string name(str::wstring_to_string(wname));
        if (absolute)
        {
            name.insert(0, path);
        }

        subdirectories.push_back(name);
    }
    while (FindNextFileW(handle, &find_file_data) != 0);

    FindClose(handle);
    return subdirectories;
}

path_info get_path_info(const std::string& path)
{
    const std::wstring wpath = vx::str::string_to_wstring(path);

    WIN32_FILE_ATTRIBUTE_DATA info{};
    if (!GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &info))
    {
        // error
        return path_info{};
    }

    path_info pinfo{};

    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        pinfo.type = path_type::DIRECTORY;
        pinfo.size = 0;
    }
    else if (info.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_DEVICE))
    {
        pinfo.type = path_type::OTHER;
        pinfo.size = ((static_cast<uint64_t>(info.nFileSizeHigh)) << 32) | info.nFileSizeLow;
    }
    else
    {
        pinfo.type = path_type::FILE;
        pinfo.size = ((static_cast<uint64_t>(info.nFileSizeHigh)) << 32) | info.nFileSizeLow;
    }

    pinfo.create_time = time::time_from_windows_file_time(info.ftCreationTime.dwLowDateTime, info.ftCreationTime.dwHighDateTime);
    pinfo.modify_time = time::time_from_windows_file_time(info.ftLastWriteTime.dwLowDateTime, info.ftLastWriteTime.dwHighDateTime);
    pinfo.access_time = time::time_from_windows_file_time(info.ftLastAccessTime.dwLowDateTime, info.ftLastAccessTime.dwHighDateTime);

    return pinfo;
}

std::string get_system_folder(system_folder p)
{
    return "";
}

}
}

#endif