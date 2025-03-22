#pragma once

#include "vertex/os/handle.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// shared_library
///////////////////////////////////////////////////////////////////////////////

class shared_library
{
public:

    // https://beta.boost.org/doc/libs/1_61_0/doc/html/boost/dll/shared_library.html

    static constexpr const char* suffix() noexcept
    {
#   if defined(VX_PLATFORM_WINDOWS)
        return ".dll";
#   elif defined(VX_PLATFORM_APPLE)
        return ".dylib";
#   else
        return ".so";
#   endif
    }

public:

    shared_library() = default;
    shared_library(const char* name) { load(name); }
    ~shared_library() { free(); }

    shared_library(const shared_library&) = default;
    shared_library& operator=(const shared_library&) = default;

    shared_library(shared_library&& other) noexcept
        : m_handle(std::move(other.m_handle))
    {
        other.free();
    }

    shared_library& operator=(shared_library&& other) noexcept
    {
        if (this != &other)
        {
            free();
            m_handle = std::move(other.m_handle);
            other.free();
        }

        return *this;
    }

    void swap(shared_library& other) noexcept { std::swap(m_handle, other.m_handle); }

public:

    VX_API bool load(const char* lib);
    VX_API bool is_loaded() const noexcept;
    VX_API void free() noexcept;

public:

    bool has(const char* symbol_name) const noexcept
    {
        return get_void(symbol_name) != nullptr;
    }

    template <typename Func>
    Func get(const char* symbol_name) const noexcept
    {
        return reinterpret_cast<Func>(get_void(symbol_name));
    }

    VX_API void* get_void(const char* symbol_name) const noexcept;

private:

    handle m_handle;
};

} // namespace os
} // namespace vx