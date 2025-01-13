#pragma once

#include <string>
#include <memory>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

class shared_library
{
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

    template <typename Func>
    Func get_function(const std::string& name)
    {
        return static_cast<Func>(get_function_internal(name));
    }

private:

    void* get_function_internal(const std::string& name);

private:

    class shared_library_impl;
    std::shared_ptr<shared_library_impl> m_impl;
};

} // namespace os
} // namespace vx