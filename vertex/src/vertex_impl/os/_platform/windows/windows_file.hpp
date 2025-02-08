#pragma once

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace os {

class file::file_impl
{
public:

    file_impl() {}
    ~file_impl() { close(); }

public:

    static bool exists(const path& p);

public:

    bool open(const path& p, mode mode);
    bool is_open() const { return m_handle.is_valid(); }
    void close();

    size_t size() const;
    bool resize(size_t size);

    bool seek(int off, stream_position from);
    size_t tell() const;
    bool flush();

    size_t read(uint8_t* data, size_t size);
    size_t write(const uint8_t* data, size_t size);

public:

    static file from_handle(HANDLE handle, mode mode);
    HANDLE get_handle();

private:

    windows::handle m_handle;
};

} // namespace os
} // namespace vx
