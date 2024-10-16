#pragma once

#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/stdlib/file/file.hpp"

namespace vx {

class file::file_impl
{
public:

    file_impl();
    ~file_impl();

    bool open(const std::string& path, file_mode mode);
    bool is_open() const;
    void close();

    size_t size() const;

    bool seek(size_t off, stream_position from);
    size_t tell() const;

    size_t read(uint8_t* data, size_t size);
    size_t write(const uint8_t* data, size_t size);
    bool flush();

private:

    HANDLE m_handle;
};

} // namespace vx

#endif
