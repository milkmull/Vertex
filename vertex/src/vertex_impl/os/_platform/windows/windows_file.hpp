#pragma once

#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_FILE)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/file.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

class file::file_impl
{
public:

    file_impl();
    ~file_impl();

public:

    static bool exists(const std::string& path);

public:

    bool open(const std::string& path, mode mode);
    bool is_open() const;
    void close();

    size_t size() const;

    bool seek(size_t off, stream_position from);
    size_t tell() const;

    size_t read(uint8_t* data, size_t size);
    size_t write(const uint8_t* data, size_t size);
    bool flush();

public:

    static file from_handle(HANDLE handle, mode mode);
    HANDLE get_handle();

private:

    HANDLE m_handle;
};

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_FILE
