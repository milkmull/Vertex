#pragma once

#include <string>
#include <memory>

namespace vx {
namespace os {

class library
{
public:

    library();
    ~library();

    library(const library&);
    library(library&&) noexcept;

    library& operator=(const library&);
    library& operator=(library&&) noexcept;

public:

    library(const std::string& name);

    bool load(const std::string& lib);
    bool is_loaded() const;
    void free();

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