//#include <shlobj_core.h> // SHGetKnownFolderPath
//#include <winioctl.h>    // FSCTL_GET_REPARSE_POINT
//
//#include "vertex/os/filesystem.hpp"
//#include "vertex/system/error.hpp"
//#include "vertex_impl/os/_platform/windows/windows_error.hpp"
//#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
//
//namespace vx {
//namespace os {
//
////=============================================================================
//// Small helpers
////=============================================================================
//
//using file_guard = handle_guard;
//
//// Opens an existing file or directory with full sharing. Follows symlinks unless
//// FILE_FLAG_OPEN_REPARSE_POINT is passed in extra_flags.
//inline file_guard open_path(native_cstring p, DWORD access, DWORD extra_flags = 0)
//{
//    return file_guard{ ::CreateFileW(
//        p,
//        access,
//        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//        NULL,
//        OPEN_EXISTING,
//        FILE_FLAG_BACKUP_SEMANTICS | extra_flags,
//        NULL) };
//}
//
//inline bool is_sep(wchar_t c)
//{
//    return c == L'\\' || c == L'/';
//}
//
//inline bool is_letter(wchar_t c)
//{
//    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
//}
//
//// Writes s[0..n) + NUL into buf if it fits. Always returns n.
//inline size_t emit(native_char* buf, size_t cap, const wchar_t* s, size_t n)
//{
//    if (n < cap)
//    {
//        mem::copy(buf, s, n * sizeof(wchar_t));
//        buf[n] = L'\0';
//    }
//    return n;
//}
//
//inline file_time time_from_large_integer(const LARGE_INTEGER& li)
//{
//    const uint64_t q = static_cast<uint64_t>(li.QuadPart);
//    return windows::time_point_from_file_time(static_cast<DWORD>(q & 0xffffffffu), static_cast<DWORD>(q >> 32));
//}
//
//// Opens an existing file or directory with full sharing. Follows symlinks unless
//// FILE_FLAG_OPEN_REPARSE_POINT is passed in extra_flags.
//inline file_guard open_path(native_cstring p, DWORD access, DWORD extra_flags = 0)
//{
//    return file_guard{ ::CreateFileW(
//        p,
//        access,
//        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//        NULL,
//        OPEN_EXISTING,
//        FILE_FLAG_BACKUP_SEMANTICS | extra_flags,
//        NULL) };
//}
//
////=============================================================================
//// Type / permission mapping
////=============================================================================
//
//// Directory junctions / mount points are reported as symlinks. As far as traversal is
//// concerned they are links, and treating them as directories would let recursive
//// iteration and remove_all walk through them (cycles, deleting outside the tree).
//inline bool is_link_tag(ULONG tag)
//{
//    return tag == IO_REPARSE_TAG_SYMLINK || tag == IO_REPARSE_TAG_MOUNT_POINT;
//}
//
//static bool has_exec_extension(const wchar_t* p)
//{
//    if (!p)
//    {
//        return false;
//    }
//
//    const wchar_t* dot = nullptr;
//    for (const wchar_t* c = p; *c; ++c)
//    {
//        if (is_sep(*c))
//        {
//            dot = nullptr;
//        }
//        else if (*c == L'.')
//        {
//            dot = c;
//        }
//    }
//
//    if (!dot)
//    {
//        return false;
//    }
//
//    return ::_wcsicmp(dot, L".exe") == 0 || ::_wcsicmp(dot, L".com") == 0 ||
//        ::_wcsicmp(dot, L".bat") == 0 || ::_wcsicmp(dot, L".cmd") == 0;
//}
//
//static file_permissions permissions_from_attrs(DWORD attrs, bool exec)
//{
//    file_permissions perms = file_permissions::all_read;
//
//    if (!(attrs & FILE_ATTRIBUTE_READONLY))
//    {
//        perms |= file_permissions::all_write;
//    }
//
//    if (exec || (attrs & FILE_ATTRIBUTE_DIRECTORY))
//    {
//        perms |= file_permissions::all_exec;
//    }
//
//    return perms;
//}
//
//// path may be nullptr (fstat), in which case executable-by-extension is skipped
//static expected<file_stat, error> stat_from_handle(HANDLE h, const wchar_t* path)
//{
//    BY_HANDLE_FILE_INFORMATION fi{};
//    if (!::GetFileInformationByHandle(h, &fi))
//    {
//        return fail("GetFileInformationByHandle");
//    }
//
//    FILE_BASIC_INFO bi{};
//    if (!::GetFileInformationByHandleEx(h, FileBasicInfo, &bi, sizeof(bi)))
//    {
//        return fail("GetFileInformationByHandleEx");
//    }
//
//    bool is_symlink = false;
//    if (fi.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
//    {
//        FILE_ATTRIBUTE_TAG_INFO tag{};
//        if (::GetFileInformationByHandleEx(h, FileAttributeTagInfo, &tag, sizeof(tag)))
//        {
//            is_symlink = (tag.ReparseTag == IO_REPARSE_TAG_SYMLINK);
//        }
//    }
//
//    file_stat st{};
//
//    if (is_symlink)
//    {
//        st.type = file_type::symlink;
//    }
//    else if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//    {
//        st.type = file_type::directory;
//    }
//    else
//    {
//        st.type = file_type::regular;
//    }
//
//    st.permissions = permissions_from_attrs(fi.dwFileAttributes, has_exec_extension(path));
//    st.size = (static_cast<uint64_t>(fi.nFileSizeHigh) << 32) | fi.nFileSizeLow;
//    st.hard_links = fi.nNumberOfLinks;
//    st.modified = time_from_large_integer(bi.LastWriteTime);
//    st.accessed = time_from_large_integer(bi.LastAccessTime);
//    st.changed = time_from_large_integer(bi.ChangeTime);
//    st.id.device = fi.dwVolumeSerialNumber;
//    st.id.index = (static_cast<uint64_t>(fi.nFileIndexHigh) << 32) | fi.nFileIndexLow;
//
//    return st;
//}
//
////=============================================================================
//// Read-only attribute handling (the only permission Windows really has)
////=============================================================================
//
//static bool wants_read_only(file_permissions mode)
//{
//    return (mode & file_permissions::all_write) == file_permissions::none;
//}
//
//static success apply_read_only(HANDLE h, bool read_only)
//{
//    FILE_BASIC_INFO bi{};
//    if (!::GetFileInformationByHandleEx(h, FileBasicInfo, &bi, sizeof(bi)))
//    {
//        return fail("GetFileInformationByHandleEx");
//    }
//
//    const DWORD attrs = bi.FileAttributes;
//    DWORD wanted = read_only ? (attrs | FILE_ATTRIBUTE_READONLY) : (attrs & ~FILE_ATTRIBUTE_READONLY);
//
//    if (wanted == attrs)
//    {
//        return {};
//    }
//
//    if (wanted == 0)
//    {
//        // 0 means "leave unchanged"
//        wanted = FILE_ATTRIBUTE_NORMAL;
//    }
//
//    // zeroed timestamps mean "leave unchanged"
//    FILE_BASIC_INFO set{};
//    set.FileAttributes = wanted;
//
//    if (!::SetFileInformationByHandle(h, FileBasicInfo, &set, sizeof(set)))
//    {
//        return fail("SetFileInformationByHandle");
//    }
//
//    return {};
//}
//
////=============================================================================
//// Removal
////=============================================================================
//
//static success remove_path(native_cstring p, bool as_directory)
//{
//    const DWORD attrs = ::GetFileAttributesW(p); // does not follow symlinks
//    if (attrs == INVALID_FILE_ATTRIBUTES)
//    {
//        return fail("GetFileAttributesW");
//    }
//
//    // DeleteFileW/RemoveDirectoryW refuse read-only entries, unlink/rmdir do not
//    const bool read_only = (attrs & FILE_ATTRIBUTE_READONLY) != 0;
//    if (read_only && !::SetFileAttributesW(p, attrs & ~FILE_ATTRIBUTE_READONLY))
//    {
//        return fail("SetFileAttributesW");
//    }
//
//    // unlink semantics: a directory symlink/junction is removed with RemoveDirectoryW
//    const bool is_dir_link = (attrs & FILE_ATTRIBUTE_DIRECTORY) && (attrs & FILE_ATTRIBUTE_REPARSE_POINT);
//    const bool use_rmdir = as_directory || is_dir_link;
//
//    const BOOL ok = use_rmdir ? ::RemoveDirectoryW(p) : ::DeleteFileW(p);
//    if (!ok)
//    {
//        // capture before the restore call overwrites the last error
//        const DWORD e = ::GetLastError();
//        if (read_only)
//        {
//            ::SetFileAttributesW(p, attrs);
//        }
//
//        ::SetLastError(e);
//        return fail(use_rmdir ? "RemoveDirectoryW" : "DeleteFileW");
//    }
//
//    return {};
//}
//
////=============================================================================
//// Symlink target normalization
////=============================================================================
//
//// https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L3117
//static size_t find_root_name_end(const wchar_t* s, size_t n)
//{
//    if (n >= 2 && is_letter(s[0]) && s[1] == L':')
//    {
//        return 2;
//    }
//
//    if (n >= 4 && is_sep(s[0]) && is_sep(s[1]) && (s[2] == L'?' || s[2] == L'.') && is_sep(s[3]))
//    {
//        return 3; // "\\?" / "\\."
//    }
//
//    if (n >= 3 && is_sep(s[0]) && is_sep(s[1]) && !is_sep(s[2]))
//    {
//        size_t i = 2; // "\\server"
//        while (i < n && !is_sep(s[i]))
//        {
//            ++i;
//        }
//        return i;
//    }
//
//    return 0;
//}
//
//// Stack storage with a heap fallback. Never throws; reserve() reports failure instead.
//template <size_t StackCount>
//class wide_buffer
//{
//public:
//
//    wide_buffer() noexcept
//        : m_data(m_stack)
//        , m_capacity(StackCount)
//    {}
//
//    ~wide_buffer()
//    {
//        release();
//    }
//
//    wide_buffer(const wide_buffer&) = delete;
//    wide_buffer& operator=(const wide_buffer&) = delete;
//
//    // Ensures room for at least count characters. Existing contents are NOT preserved.
//    bool reserve(size_t count) noexcept
//    {
//        if (count <= m_capacity)
//        {
//            return true;
//        }
//
//        if (count > mem::max_array_size<wchar_t>())
//        {
//            return false;
//        }
//
//        wchar_t* p = static_cast<wchar_t*>(mem::allocate(count * sizeof(wchar_t)));
//        if (!p)
//        {
//            return false;
//        }
//
//        release();
//        m_data = p;
//        m_capacity = count;
//
//        return true;
//    }
//
//    wchar_t* data() noexcept
//    {
//        return m_data;
//    }
//
//    size_t capacity() const noexcept
//    {
//        return m_capacity;
//    }
//
//private:
//
//    void release() noexcept
//    {
//        if (m_data != m_stack)
//        {
//            mem::deallocate_raw(m_data);
//        }
//    }
//
//    wchar_t m_stack[StackCount];
//    wchar_t* m_data;
//    size_t m_capacity;
//};
//
//using path_buffer = wide_buffer<512>;
//
//// Writes the normalized, NUL-terminated target into out. Returns false on allocation failure.
//// transforms separators in the root-name to '\', and collapses all other runs into a single '\'
//// a/\/b -> a\b      //server/a////b -> \\server\a\b
//// (the result is never longer than the input)
//static bool normalize_symlink_target(const wchar_t* text, path_buffer& out) noexcept
//{
//    const size_t size = std::wcslen(text);
//    const size_t root_name_end = find_root_name_end(text, size);
//
//    if (!out.reserve(size + 1))
//    {
//        return false;
//    }
//
//    wchar_t* dst = out.data();
//    size_t n = 0;
//
//    bool last_was_sep = false;
//    for (size_t i = 0; i < size; ++i)
//    {
//        if (is_sep(text[i]))
//        {
//            if (i < root_name_end || !last_was_sep)
//            {
//                dst[n++] = L'\\';
//            }
//            last_was_sep = true;
//        }
//        else
//        {
//            dst[n++] = text[i];
//            last_was_sep = false;
//        }
//    }
//
//    dst[n] = L'\0';
//    return true;
//}
//
////=============================================================================
//// Reparse data (symlink only)
////=============================================================================
//
//struct symlink_reparse_buffer
//{
//    ULONG ReparseTag;
//    USHORT ReparseDataLength;
//    USHORT Reserved;
//    USHORT SubstituteNameOffset;
//    USHORT SubstituteNameLength;
//    USHORT PrintNameOffset;
//    USHORT PrintNameLength;
//    ULONG Flags;
//    WCHAR PathBuffer[1];
//};
//
//// same leading fields as the symlink layout, but no Flags member
//struct mount_point_reparse_buffer
//{
//    ULONG ReparseTag;
//    USHORT ReparseDataLength;
//    USHORT Reserved;
//    USHORT SubstituteNameOffset;
//    USHORT SubstituteNameLength;
//    USHORT PrintNameOffset;
//    USHORT PrintNameLength;
//    WCHAR PathBuffer[1];
//};
//
//static bool is_dot_or_dotdot(const wchar_t* name)
//{
//    if (name[0] != L'.')
//    {
//        return false;
//    }
//
//    if (name[1] == L'\0')
//    {
//        return true;
//    }
//
//    return name[1] == L'.' && name[2] == L'\0';
//}
//
//static file_type type_from_find_data(const WIN32_FIND_DATAW& d)
//{
//    // dwReserved0 holds the reparse tag when FILE_ATTRIBUTE_REPARSE_POINT is set,
//    // so no extra CreateFile per entry is needed
//    if ((d.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && is_link_tag(d.dwReserved0))
//    {
//        return file_type::symlink;
//    }
//
//    if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//    {
//        return file_type::directory;
//    }
//
//    return file_type::regular;
//}
//
//// Opaque in the header (struct directory; using directory_handle = directory*;)
//struct directory
//{
//    HANDLE find;
//    WIN32_FIND_DATAW data;
//    bool pending; // data holds an entry (from FindFirstFileEx) that was not returned yet
//    bool done;
//};
//
////=============================================================================
//// metadata
////=============================================================================
//
//expected<file_stat, error> stat(const native_cstring p)
//{
//    file_guard h = open_path(p, FILE_READ_ATTRIBUTES);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    return stat_from_handle(h.get(), p);
//}
//
//expected<file_stat, error> lstat(const native_cstring p)
//{
//    file_guard h = open_path(p, FILE_READ_ATTRIBUTES, FILE_FLAG_OPEN_REPARSE_POINT);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    return stat_from_handle(h.get(), p);
//}
//
//expected<file_stat, error> fstat(native_handle handle)
//{
//    const HANDLE h = handle;
//
//    ::SetLastError(NO_ERROR);
//    const DWORD kind = ::GetFileType(h);
//
//    file_stat st{};
//    switch (kind)
//    {
//        case FILE_TYPE_DISK:
//        {
//            return stat_from_handle(h, nullptr);
//        }
//        case FILE_TYPE_CHAR:
//        {
//            st.type = file_type::character;
//            return st;
//        }
//        case FILE_TYPE_PIPE:
//        {
//            st.type = file_type::fifo;
//            return st;
//        }
//        default:
//        {
//            if (::GetLastError() != NO_ERROR)
//            {
//                return fail("GetFileType");
//            }
//            st.type = file_type::unknown;
//            return st;
//        }
//    }
//}
//
//success set_permissions(const native_cstring p, file_permissions mode, bool follow_symlinks)
//{
//    const bool read_only = wants_read_only(mode);
//
//    if (follow_symlinks)
//    {
//        file_guard h = open_path(p, FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES);
//        if (!h.valid())
//        {
//            return fail("CreateFileW");
//        }
//
//        return apply_read_only(h.get(), read_only);
//    }
//
//    // SetFileAttributesW operates on the link itself
//    const DWORD attrs = ::GetFileAttributesW(p);
//    if (attrs == INVALID_FILE_ATTRIBUTES)
//    {
//        return fail("GetFileAttributesW");
//    }
//
//    DWORD wanted = read_only ? (attrs | FILE_ATTRIBUTE_READONLY) : (attrs & ~FILE_ATTRIBUTE_READONLY);
//    if (wanted == attrs)
//    {
//        return {};
//    }
//
//    if (wanted == 0)
//    {
//        wanted = FILE_ATTRIBUTE_NORMAL;
//    }
//
//    if (!::SetFileAttributesW(p, wanted))
//    {
//        return fail("SetFileAttributesW");
//    }
//
//    return {};
//}
//
//success set_permissions(native_handle h, file_permissions mode)
//{
//    return apply_read_only(h, wants_read_only(mode));
//}
//
//success set_modified_time(const native_cstring p, file_time t)
//{
//    file_guard h = open_path(p, FILE_WRITE_ATTRIBUTES);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    return set_modified_time(h.get(), t);
//}
//
//success set_modified_time(native_handle h, file_time t)
//{
//    FILETIME ft{};
//    windows::time_point_to_file_time(t, ft.dwLowDateTime, ft.dwHighDateTime);
//
//    if (!::SetFileTime(h, NULL, NULL, &ft))
//    {
//        return fail("SetFileTime");
//    }
//
//    return {};
//}
//
//success resize_file(const native_cstring p, uint64_t size)
//{
//    file_guard h = open_path(p, FILE_WRITE_DATA);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    return set_file_size(h.get(), size);
//}
//
////=============================================================================
//// files and directories
////=============================================================================
//
//bool exists(const native_cstring p)
//{
//    const DWORD attrs = ::GetFileAttributesW(p); // does not follow symlinks
//    if (attrs == INVALID_FILE_ATTRIBUTES)
//    {
//        // e.g. pagefile.sys: the file is there, we just may not query it
//        return ::GetLastError() == ERROR_SHARING_VIOLATION;
//    }
//
//    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
//    {
//        // follow the link so a dangling symlink does not "exist" (stat semantics)
//        file_guard h = open_path(p, FILE_READ_ATTRIBUTES);
//        if (!h.valid())
//        {
//            return ::GetLastError() == ERROR_SHARING_VIOLATION;
//        }
//    }
//
//    return true;
//}
//
//expected<uint64_t, error> file_size(const native_cstring p)
//{
//    file_guard h = open_path(p, FILE_READ_ATTRIBUTES); // follows symlinks
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    FILE_STANDARD_INFO info{};
//    if (!::GetFileInformationByHandleEx(h.get(), FileStandardInfo, &info, sizeof(info)))
//    {
//        return fail("GetFileInformationByHandleEx");
//    }
//
//    if (info.Directory)
//    {
//        ::SetLastError(ERROR_DIRECTORY); // surfaces as err::invalid_argument
//        return fail("file_size");
//    }
//
//    return static_cast<uint64_t>(info.EndOfFile.QuadPart);
//}
//
//success create_directory(const native_cstring p, file_permissions /*mode*/)
//{
//    // Windows has no mode bits here. An existing directory is reported as an error
//    // (like mkdir/EEXIST) so the front end can tell "created" from "already there".
//    //
//    // Note: each path component must still be <= 255 chars, even with the \\?\ prefix.
//    if (!::CreateDirectoryW(p, NULL))
//    {
//        return fail("CreateDirectoryW");
//    }
//
//    return {};
//}
//
//success remove_file(const native_cstring p)
//{
//    return remove_path(p, false);
//}
//
//success remove_directory(const native_cstring p)
//{
//    return remove_path(p, true);
//}
//
//success rename(const native_cstring from, const native_cstring to)
//{
//    // No MOVEFILE_COPY_ALLOWED: rename() must not silently degrade into copy+delete across volumes
//    if (!::MoveFileExW(from, to, MOVEFILE_REPLACE_EXISTING))
//    {
//        return fail("MoveFileExW");
//    }
//
//    return {};
//}
//
////=============================================================================
//// directory iteration
////=============================================================================
//
//expected<directory_handle, error> open_directory(const native_cstring p)
//{
//    const size_t len = std::wcslen(p);
//    if (len == 0)
//    {
//        ::SetLastError(ERROR_PATH_NOT_FOUND);
//        return fail("open_directory");
//    }
//
//    // build "<p>\*" (stack for typical paths, heap for long ones)
//    const bool needs_sep = !is_sep(p[len - 1]);
//
//    path_buffer pattern;
//    if (!pattern.reserve(len + (needs_sep ? 1 : 0) + 2)) // sep + '*' + NUL
//    {
//        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//        return fail("open_directory");
//    }
//
//    wchar_t* w = pattern.data();
//    mem::copy(w, p, len * sizeof(wchar_t));
//    size_t n = len;
//    if (needs_sep)
//    {
//        w[n++] = L'\\';
//    }
//    w[n++] = L'*';
//    w[n] = L'\0';
//
//    // the one allocation per opened directory: FindFirstFileEx needs a stable WIN32_FIND_DATAW
//    directory* d = static_cast<directory*>(mem::allocate(sizeof(directory)));
//    if (!d)
//    {
//        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//        return fail("open_directory");
//    }
//
//    mem::set(d, 0, sizeof(directory));
//
//    d->find = ::FindFirstFileExW(
//        pattern.data(),
//        FindExInfoBasic, // skip short names
//        &d->data,
//        FindExSearchNameMatch,
//        nullptr,
//        FIND_FIRST_EX_LARGE_FETCH);
//
//    if (d->find == INVALID_HANDLE_VALUE)
//    {
//        if (::GetLastError() == ERROR_FILE_NOT_FOUND)
//        {
//            // directory exists but has no entries (volume roots have no "." / "..")
//            d->done = true;
//        }
//        else
//        {
//            const auto f = fail("FindFirstFileExW");
//            mem::deallocate_raw(d);
//            return f;
//        }
//    }
//    else
//    {
//        d->pending = true;
//    }
//
//    return d;
//}
//
//expected<bool, error> read_directory(directory_handle d, directory_entry& entry)
//{
//    for (;;)
//    {
//        if (d->done)
//        {
//            return false;
//        }
//
//        if (d->pending)
//        {
//            d->pending = false;
//        }
//        else if (!::FindNextFileW(d->find, &d->data))
//        {
//            const DWORD e = ::GetLastError();
//            d->done = true;
//
//            if (e == ERROR_NO_MORE_FILES)
//            {
//                return false;
//            }
//
//            ::SetLastError(e);
//            return fail("FindNextFileW");
//        }
//
//        if (is_dot_or_dotdot(d->data.cFileName))
//        {
//            continue;
//        }
//
//        entry.name = d->data.cFileName; // valid until the next read_directory / close_directory
//        entry.type = type_from_find_data(d->data);
//        return true;
//    }
//}
//
//success close_directory(directory_handle d)
//{
//    if (!d)
//    {
//        return success{};
//    }
//
//    const bool ok = (d->find == INVALID_HANDLE_VALUE) || ::FindClose(d->find);
//    const DWORD e = ok ? NO_ERROR : ::GetLastError();
//
//    mem::deallocate_raw(d);
//
//    if (!ok)
//    {
//        ::SetLastError(e);
//        return fail("FindClose");
//    }
//
//    return success{};
//}
//
////=============================================================================
//// links
////=============================================================================
//
//success create_symlink(const native_cstring target, const native_cstring link, bool target_is_directory)
//{
//    path_buffer normalized;
//    if (!normalize_symlink_target(target, normalized))
//    {
//        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//        return fail("create_symlink");
//    }
//
//    const DWORD flags = target_is_directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
//
//    // https://github.com/microsoft/STL/blob/main/stl/src/filesystem.cpp#L26
//    // Try without the admin / developer-mode requirement first; older builds reject the flag
//    if (!::CreateSymbolicLinkW(link, normalized.data(), flags | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE))
//    {
//        if (::GetLastError() != ERROR_INVALID_PARAMETER ||
//            !::CreateSymbolicLinkW(link, normalized.data(), flags))
//        {
//            return fail("CreateSymbolicLinkW");
//        }
//    }
//
//    return {};
//}
//
//success create_hard_link(const native_cstring target, const native_cstring link)
//{
//    if (!::CreateHardLinkW(link, target, NULL))
//    {
//        return fail("CreateHardLinkW");
//    }
//
//    return {};
//}
//
//// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542
//expected<size_t, error> read_symlink(const native_cstring p, native_char* buf, size_t cap)
//{
//    file_guard h = open_path(p, 0, FILE_FLAG_OPEN_REPARSE_POINT);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    alignas(8) BYTE raw[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
//    DWORD bytes = 0;
//
//    if (!::DeviceIoControl(h.get(), FSCTL_GET_REPARSE_POINT, NULL, 0, raw, sizeof(raw), &bytes, NULL))
//    {
//        return fail("DeviceIoControl");
//    }
//
//    const auto* rb = reinterpret_cast<const symlink_reparse_buffer*>(raw);
//
//    if (rb->ReparseTag != IO_REPARSE_TAG_SYMLINK)
//    {
//        ::SetLastError(ERROR_REPARSE_TAG_MISMATCH);
//        return fail("DeviceIoControl");
//    }
//
//    constexpr size_t header = offsetof(symlink_reparse_buffer, PathBuffer);
//
//    USHORT offset = rb->PrintNameOffset;
//    USHORT length = rb->PrintNameLength;
//    bool substitute = false;
//
//    if (length == 0)
//    {
//        offset = rb->SubstituteNameOffset;
//        length = rb->SubstituteNameLength;
//        substitute = true;
//    }
//
//    if (bytes < header || static_cast<size_t>(offset) + length > bytes - header)
//    {
//        ::SetLastError(ERROR_INVALID_DATA);
//        return fail("DeviceIoControl");
//    }
//
//    const wchar_t* s = reinterpret_cast<const wchar_t*>(reinterpret_cast<const BYTE*>(rb->PathBuffer) + offset);
//    size_t n = length / sizeof(wchar_t);
//
//    // substitute names of absolute links carry the NT prefix "\??\"
//    if (substitute && n >= 4 && s[0] == L'\\' && s[1] == L'?' && s[2] == L'?' && s[3] == L'\\')
//    {
//        s += 4;
//        n -= 4;
//    }
//
//    return emit(buf, cap, s, n);
//}
//
////=============================================================================
//// path resolution
////=============================================================================
//
//// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/operations.cpp#L2686
//expected<size_t, error> canonical(const native_cstring p, native_char* buf, size_t cap)
//{
//    file_guard h = open_path(p, FILE_READ_ATTRIBUTES);
//    if (!h.valid())
//    {
//        return fail("CreateFileW");
//    }
//
//    // The final length depends on which prefix gets stripped, so resolve into a
//    // scratch buffer first and then emit into the caller's buffer.
//    DWORD flags = FILE_NAME_NORMALIZED;
//
//    path_buffer scratch;
//    wchar_t* data = scratch.data();
//    DWORD capacity = static_cast<DWORD>(scratch.capacity());
//
//    DWORD size = ::GetFinalPathNameByHandleW(h.get(), data, capacity, flags);
//
//    if (size == 0 && !(flags & VOLUME_NAME_NT) && ::GetLastError() == ERROR_PATH_NOT_FOUND)
//    {
//        // drive letter does not exist, obtain an NT path
//        flags |= VOLUME_NAME_NT;
//        size = ::GetFinalPathNameByHandleW(h.get(), data, capacity, flags);
//    }
//
//    if (size == 0)
//    {
//        return fail("GetFinalPathNameByHandleW");
//    }
//
//    if (size >= capacity) // too small: size is the required count including NUL
//    {
//        if (!scratch.reserve(size))
//        {
//            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//            return fail("canonical");
//        }
//
//        data = scratch.data();
//        capacity = size;
//        size = ::GetFinalPathNameByHandleW(h.get(), data, capacity, flags);
//
//        if (size == 0)
//        {
//            return fail("GetFinalPathNameByHandleW");
//        }
//
//        if (size >= capacity) // path changed underneath us
//        {
//            ::SetLastError(ERROR_INSUFFICIENT_BUFFER);
//            return fail("GetFinalPathNameByHandleW");
//        }
//    }
//
//    // size is now the length excluding NUL
//    if (flags & VOLUME_NAME_NT)
//    {
//        // result is in the NT namespace, so apply the DOS to NT namespace prefix
//        static const wchar_t prefix[] = L"\\\\?\\GLOBALROOT";
//        constexpr size_t plen = (sizeof(prefix) / sizeof(prefix[0])) - 1;
//        const size_t total = plen + size;
//
//        if (total < cap)
//        {
//            mem::copy(buf, prefix, plen * sizeof(wchar_t));
//            mem::copy(buf + plen, data, size * sizeof(wchar_t));
//            buf[total] = L'\0';
//        }
//
//        return total;
//    }
//
//    // If the input path did not contain a long path prefix, convert
//    // "\\?\X:" to "X:" and "\\?\UNC\" to "\\". Otherwise preserve the prefix.
//    // (the && chain stops at the NUL terminator, so short inputs are safe)
//    const bool input_has_prefix = is_sep(p[0]) && is_sep(p[1]) && p[2] == L'?' && is_sep(p[3]);
//
//    wchar_t* d = data;
//    size_t n = size;
//
//    if (!input_has_prefix && n > 6 && is_sep(d[0]) && is_sep(d[1]) && d[2] == L'?' && is_sep(d[3]))
//    {
//        if (is_letter(d[4]) && d[5] == L':')
//        {
//            // "\\?\X:"
//            d += 4;
//            n -= 4;
//        }
//        else if (n >= 8 &&
//            (d[4] == L'U' || d[4] == L'u') &&
//            (d[5] == L'N' || d[5] == L'n') &&
//            (d[6] == L'C' || d[6] == L'c') &&
//            is_sep(d[7]))
//        {
//            // "\\?\UNC\*": trim "\\?\UN" and turn the C into '\' to end up with "\\"
//            d += 6;
//            n -= 6;
//            d[0] = L'\\';
//        }
//    }
//
//    return emit(buf, cap, d, n);
//}
//
//expected<size_t, error> current_directory(native_char* buf, size_t cap)
//{
//    // Native size-query semantics: returns length (excl. NUL) on success, or the
//    // required size including NUL if the buffer is too small, 0 on failure.
//    const DWORD n = ::GetCurrentDirectoryW(static_cast<DWORD>(cap), buf);
//    if (n == 0)
//    {
//        return fail("GetCurrentDirectoryW");
//    }
//
//    return (n >= cap) ? static_cast<size_t>(n - 1) : static_cast<size_t>(n);
//}
//
//expected<size_t, error> temp_directory(native_char* buf, size_t cap)
//{
//    // GetTempPathW checks TMP, TEMP, USERPROFILE, then the Windows directory.
//    // Its result never exceeds MAX_PATH + 1 characters.
//    wchar_t tmp[MAX_PATH + 2];
//    DWORD n = ::GetTempPathW(static_cast<DWORD>(sizeof(tmp) / sizeof(tmp[0])), tmp);
//    if (n == 0)
//    {
//        return fail("GetTempPathW");
//    }
//
//    if (n >= sizeof(tmp) / sizeof(tmp[0]))
//    {
//        ::SetLastError(ERROR_INSUFFICIENT_BUFFER);
//        return fail("GetTempPathW");
//    }
//
//    // drop the trailing separator for consistency with POSIX, except for a root like "C:\"
//    if (n > 3 && is_sep(tmp[n - 1]))
//    {
//        --n;
//    }
//
//    return emit(buf, cap, tmp, n);
//}
//
//success set_current_directory(const native_cstring p)
//{
//    if (!::SetCurrentDirectoryW(p))
//    {
//        return fail("SetCurrentDirectoryW");
//    }
//
//    return success{};
//}
//
//expected<size_t, error> absolute(const native_cstring p, native_char* buf, size_t cap)
//{
//    // Lexical only: no symlink resolution and the path need not exist. Unlike a plain
//    // "cwd + p" join this also handles drive-relative ("C:foo") and root-relative ("\foo")
//    // paths, and normalizes "." / ".." segments.
//    // Returns length (excl. NUL) on success, the required size incl. NUL if too small, 0 on failure.
//    const DWORD n = ::GetFullPathNameW(p, static_cast<DWORD>(cap), buf, nullptr);
//    if (n == 0)
//    {
//        return fail("GetFullPathNameW");
//    }
//
//    return (n >= cap) ? static_cast<size_t>(n - 1) : static_cast<size_t>(n);
//}
//
//static DWORD win32_from_hresult(HRESULT hr) noexcept
//{
//    if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
//    {
//        return HRESULT_CODE(hr);
//    }
//
//    switch (hr)
//    {
//        case E_OUTOFMEMORY:
//        {
//            return ERROR_NOT_ENOUGH_MEMORY;
//        }
//        case E_ACCESSDENIED:
//        {
//            return ERROR_ACCESS_DENIED;
//        }
//        case E_INVALIDARG:
//        {
//            return ERROR_INVALID_PARAMETER;
//        }
//        case E_NOTIMPL:
//        {
//            return ERROR_NOT_SUPPORTED;
//        }
//        default:
//        {
//            return ERROR_INTERNAL_ERROR;
//        }
//    }
//}
//
//expected<size_t, error> user_directory(user_folder folder, native_char* buf, size_t cap)
//{
//    const KNOWNFOLDERID* id = nullptr;
//
//    switch (folder)
//    {
//        case user_folder::home:
//        {
//            id = &FOLDERID_Profile;
//            break;
//        }
//        case user_folder::desktop:
//        {
//            id = &FOLDERID_Desktop;
//            break;
//        }
//        case user_folder::documents:
//        {
//            id = &FOLDERID_Documents;
//            break;
//        }
//        case user_folder::downloads:
//        {
//            id = &FOLDERID_Downloads;
//            break;
//        }
//        case user_folder::music:
//        {
//            id = &FOLDERID_Music;
//            break;
//        }
//        case user_folder::pictures:
//        {
//            id = &FOLDERID_Pictures;
//            break;
//        }
//        case user_folder::videos:
//        {
//            id = &FOLDERID_Videos;
//            break;
//        }
//    }
//
//    if (!id)
//    {
//        ::SetLastError(ERROR_INVALID_PARAMETER);
//        return fail("user_directory");
//    }
//
//    PWSTR path = nullptr;
//    const HRESULT hr = ::SHGetKnownFolderPath(*id, KF_FLAG_DEFAULT, nullptr, &path);
//
//    if (FAILED(hr))
//    {
//        ::CoTaskMemFree(path); // may be set even on failure
//        ::SetLastError(win32_from_hresult(hr));
//        return fail("SHGetKnownFolderPath");
//    }
//
//    const size_t length = emit(buf, cap, path, std::wcslen(path));
//    ::CoTaskMemFree(path);
//
//    return length;
//}
//
////=============================================================================
//// space and copy
////=============================================================================
//
//expected<space_info, error> space(const native_cstring p)
//{
//    ULARGE_INTEGER available{};
//    ULARGE_INTEGER capacity{};
//    ULARGE_INTEGER free_bytes{};
//
//    if (!::GetDiskFreeSpaceExW(p, &available, &capacity, &free_bytes))
//    {
//        return fail("GetDiskFreeSpaceExW");
//    }
//
//    space_info info{};
//    info.capacity = capacity.QuadPart;
//    info.free = free_bytes.QuadPart;
//    info.available = available.QuadPart;
//    return info;
//}
//
//success copy_file_native(const native_cstring from, const native_cstring to, copy_options options)
//{
//    // none behaves like std::filesystem::copy_options::none: an existing target is an error
//    const DWORD flags = (options == copy_options::overwrite_existing) ? 0 : COPY_FILE_FAIL_IF_EXISTS;
//
//    if (!::CopyFileExW(from, to, NULL, NULL, NULL, flags))
//    {
//        return fail("CopyFileExW");
//    }
//
//    return {};
//}
//
//} // namespace os
//} // namespace vx
