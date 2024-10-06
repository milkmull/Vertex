#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include <shlobj.h>
#undef min
#undef max

#include "vertex/stdlib/file/filesystem.hpp"
#include "vertex/stdlib/string/string.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html
// https://docs.godotengine.org/en/stable/classes/class_diraccess.html
// https://docs.godotengine.org/en/stable/classes/class_os.html
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

static void path_already_exists_error(const std::string& path)
{
    VX_ERROR(error::error_code::FILE_PATH_ALREADY_EXISTS)
        << error::error_code_to_string(error::error_code::FILE_PATH_ALREADY_EXISTS)
        << ": " << path;
}

static void file_operation_error(const std::string& path, const char* operation)
{
    VX_ERROR(error::error_code::FILE_OPERATION_FAILED)
        << "faild to " << operation << ": " << path;
}

static void file_operation_error(const std::string& old_path, const std::string& new_path, const char* operation)
{
    VX_ERROR(error::error_code::FILE_OPERATION_FAILED)
        << "faild to " << operation << ": " << old_path << " -> " << new_path;
}

static void directory_creation_error(const std::string& path)
{
    file_operation_error(path, "create directory");
}

///////////////////////////////////////////////////////////////////////////////
// File Ops
///////////////////////////////////////////////////////////////////////////////

bool exists(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);
    const DWORD attributes = GetFileAttributesW(wpath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

bool create_directory(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);

    // Don't throw an error if the directory already exists
    if (!CreateDirectoryW(wpath.c_str(), NULL) && (GetLastError() != ERROR_ALREADY_EXISTS))
    {
        directory_creation_error(path);
        return false;
    }

    return true;
}

VX_API bool create_directories(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);
    std::wstring wcurrent_path;

    size_t i = 0;
    while (i < wpath.size())
    {
        i = wpath.find_first_of(L"/\\", i + 1);
        wcurrent_path = wpath.substr(0, i);

        if (!CreateDirectoryW(wcurrent_path.c_str(), NULL) && (GetLastError() != ERROR_ALREADY_EXISTS))
        {
            directory_creation_error(str::wstring_to_string(wcurrent_path));
            return false;
        }
    }

    return true;
}

bool copy(const std::string& old_path, const std::string& new_path, copy_options::type options)
{
    const std::wstring wold_path = str::string_to_wstring(old_path);
    const std::wstring wnew_path = str::string_to_wstring(new_path);

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(wold_path.c_str(), GetFileExInfoStandard, &data))
    {
        if (!exists(old_path))
        {
            path_already_exists_error(old_path);
        }
        else
        {
            file_operation_error(old_path, "copy");
        }

        return false;
    }

    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (!CreateDirectoryW(wnew_path.c_str(), NULL) && (GetLastError() != ERROR_ALREADY_EXISTS))
        {
            directory_creation_error(new_path);
            return false;
        }

        if (options & copy_options::RECURSIVE)
        {
            error::clear_error();
            const std::vector<std::string> subdirectories = list(old_path);
            
            // Make sure no error was thrown by list()
            if (error::get_error())
            {
                return false;
            }

            for (const std::string& path : subdirectories)
            {
                const std::string src = old_path + '\\' + path;
                const std::string dst = new_path + '\\' + path;

                if (!filesystem::copy(src, dst, options))
                {
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

        if ((options & copy_options::SKIP_EXISTING) && (data.dwFileAttributes != INVALID_FILE_ATTRIBUTES))
        {
            return true;
        }

        const BOOL bFailIfExists = !(options & copy_options::OVERWRITE_EXISTING);
        if (!CopyFileW(wold_path.c_str(), wnew_path.c_str(), bFailIfExists))
        {
            if ((GetLastError() == ERROR_FILE_EXISTS) && bFailIfExists)
            {
                path_already_exists_error(new_path);
            }
            else
            {
                file_operation_error(old_path, new_path, "copy");
            }

            return false;
        }
    }

    return true;
}

bool rename(const std::string& old_path, const std::string& new_path, bool overwrite_existing)
{
    const std::wstring wold_path = str::string_to_wstring(old_path);
    const std::wstring wnew_path = str::string_to_wstring(new_path);

    const DWORD dwFalgs = (overwrite_existing ? MOVEFILE_REPLACE_EXISTING : 0);
    if (!MoveFileExW(wold_path.c_str(), wnew_path.c_str(), dwFalgs))
    {
        if ((GetLastError() == ERROR_FILE_EXISTS) && (dwFalgs != MOVEFILE_REPLACE_EXISTING))
        {
            path_already_exists_error(new_path);
        }
        else
        {
            file_operation_error(old_path, new_path, "rename");
        }

        return false;
    }

    return true;
}

bool remove(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &data))
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            // Path is already gone
            return true;
        }

        file_operation_error(path, "remove");
        return false;
    }

    const bool is_directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    const BOOL success = is_directory ? RemoveDirectoryW(wpath.c_str()) : DeleteFileW(wpath.c_str());

    if (!success)
    {
        file_operation_error(path, "remove");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

std::string get_absolute_path(const std::string& path)
{
    std::string abs_path;
    const std::wstring wpath = str::string_to_wstring(path);

    const DWORD size = GetFullPathNameW(wpath.c_str(), 0, NULL, NULL);
    std::vector<WCHAR> data(size);

    if (GetFullPathNameW(wpath.c_str(), size, data.data(), NULL))
    {
        str::utf8::from_wide(data.begin(), data.end() - 1, std::back_inserter(abs_path));
    }
    else
    {
        file_operation_error(path, "resolve path");
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
    else
    {
        file_operation_error(path, "get current directory");
    }

    return path;
}

bool change_current_directory(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);

    if (!SetCurrentDirectoryW(wpath.c_str()))
    {
        file_operation_error(path, "set current directory");
        return false;
    }

    return true;
}

std::vector<std::string> list()
{
    std::vector<std::string> ls;
    error::clear_error();

    const std::string dir = get_current_directory();
    if (!error::get_error())
    {
        ls = list();
    }

    return ls;
}

std::vector<std::string> list(const std::string& path, bool absolute)
{
    // Append wildcard to search for all items
    const std::wstring wpath = str::string_to_wstring(path) + L"\\*";
    std::vector<std::string> subdirectories;

    WIN32_FIND_DATAW data{};
    HANDLE handle = FindFirstFileW(wpath.c_str(), &data);

    if (handle == INVALID_HANDLE_VALUE)
    {
        // Return empty list if the directory is not accessible
        file_operation_error(path, "list directory");
        return subdirectories;
    }

    do
    {
        const WCHAR* wname = data.cFileName;

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
            name.insert(0, path + '\\');
        }

        subdirectories.push_back(name);
    }
    while (FindNextFileW(handle, &data));

    FindClose(handle);
    return subdirectories;
}

path_info get_path_info(const std::string& path)
{
    const std::wstring wpath = str::string_to_wstring(path);
    path_info info{};

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &data))
    {
        file_operation_error(path, "obtain path info");
        return info;
    }

    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        info.type = path_type::DIRECTORY;
        info.size = 0;
    }
    else if (data.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_DEVICE))
    {
        info.type = path_type::OTHER;
        info.size = ((static_cast<uint64_t>(data.nFileSizeHigh)) << 32) | data.nFileSizeLow;
    }
    else
    {
        info.type = path_type::FILE;
        info.size = ((static_cast<uint64_t>(data.nFileSizeHigh)) << 32) | data.nFileSizeLow;
    }

    info.create_time = time::time_from_windows_file_time(data.ftCreationTime.dwLowDateTime, data.ftCreationTime.dwHighDateTime);
    info.modify_time = time::time_from_windows_file_time(data.ftLastWriteTime.dwLowDateTime, data.ftLastWriteTime.dwHighDateTime);
    info.access_time = time::time_from_windows_file_time(data.ftLastAccessTime.dwLowDateTime, data.ftLastAccessTime.dwHighDateTime);

    return info;
}

std::string get_app_data_folder(const std::string& app)
{
    const std::wstring wapp = str::string_to_wstring(app);
    std::string path;
    WCHAR wpath[MAX_PATH]{};

    // Get the path to the AppData folder
    if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, wpath)))
    {
        file_operation_error(app, "locate app data directory");
        return path;
    }

    path = str::wstring_to_string(wpath);
    // Append application specific directory within the app data folder
    path.append("\\").append(app);

    if (!create_directory(path))
    {
        // Error in create_directory(), clear the path
        path.clear();
    }
    
    return path;
}

std::string get_user_folder(user_folder folder)
{
    std::string path;

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
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &szPath)))
    {
        path = str::wstring_to_string(szPath);
    }
    else
    {
        VX_ERROR(error::error_code::FILE_PATH_NOT_FOUND) << "could not find folder";
    }

    return path;
}

}
}

#endif