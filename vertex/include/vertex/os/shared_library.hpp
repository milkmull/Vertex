#pragma once

#include "vertex/os/__platform/shared_library_impl_data.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// shared_library
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

class shared_library_impl;

} // namespace __detail

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
    shared_library(shared_library&&) noexcept = default;

    shared_library& operator=(const shared_library&) = default;
    shared_library& operator=(shared_library&&) noexcept = default;

    void swap(shared_library& other) noexcept { std::swap(m_impl_data, other.m_impl_data); }

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
        return static_cast<Func>(get_void(symbol_name));
    }

    VX_API void* get_void(const char* symbol_name) const noexcept;

private:

    using shared_library_impl = __detail::shared_library_impl;
    friend shared_library_impl;

    using impl_data = __detail::shared_library_impl_data;
    impl_data m_impl_data;
};

} // namespace os
} // namespace vx