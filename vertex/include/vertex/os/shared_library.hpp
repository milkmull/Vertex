#pragma once

#include "vertex/os/path.hpp"

namespace vx {
namespace os {

class shared_library
{
public:

    // https://beta.boost.org/doc/libs/1_61_0/doc/html/boost/dll/shared_library.html

    static constexpr const char* suffix()
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

    VX_API shared_library();
    VX_API ~shared_library();

    VX_API shared_library(const shared_library&);
    VX_API shared_library(shared_library&&) noexcept;

    VX_API shared_library& operator=(const shared_library&);
    VX_API shared_library& operator=(shared_library&&) noexcept;

public:

    VX_API shared_library(const std::string& name);

    VX_API bool load(const std::string& lib);
    VX_API bool is_loaded() const;
    VX_API void free();

    VX_API path location() const;

public:

    inline bool has(const std::string& symbol_name) const
    {
        return get_void(symbol_name);
    }

    template <typename Func>
    inline Func get(const std::string& symbol_name) const
    {
        return static_cast<Func>(get_void(symbol_name));
    }

private:

    void* get_void(const std::string& symbol_name) const;

private:

    class shared_library_impl;
    std::shared_ptr<shared_library_impl> m_impl;
};

} // namespace os
} // namespace vx