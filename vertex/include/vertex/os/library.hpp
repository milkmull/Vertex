#pragma once

#include <string>
#include <memory>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

class library
{
public:

    VX_API library();
    VX_API ~library();

    VX_API library(const library&);
    VX_API library(library&&) noexcept;

    VX_API library& operator=(const library&);
    VX_API library& operator=(library&&) noexcept;

public:

    VX_API library(const std::string& name);

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

    class library_impl;
    std::shared_ptr<library_impl> m_impl;
};

} // namespace os
} // namespace vx