#include <sys/utsname.h>

#include "vertex_impl/os/__platform/unix/unix_tools.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace os {

struct value_cache
{
    bool set;
    std::string value;
};

std::string get_model_name_impl()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        os::file::read_file("/sys/devices/virtual/dmi/id/product_name", cache.value);
        cache.set = true;
    }

    return cache.value;
}

std::string get_system_name_impl()
{
    struct utsname u;
    if (::uname(&u) != 0)
    {
        unix_::error_message("uname()");
        return {};
    }

    return u.sysname;
}

std::string get_processor_name_impl()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        do
        {
            os::file f;
            if (!f.open("/proc/cpuinfo", os::file::mode::READ))
            {
                break;
            }

            // Search for line like "model name: model"

            std::string line;
            while (f.read_line(line))
            {
                size_t off = line.find("model name");

                if (off == std::string::npos)
                {
                    continue;
                }

                off = line.find(':', off);
                if (off != std::string::npos)
                {
                    cache.value = line.substr(off + 2); // Skip ": "
                    break;
                }
            }

            cache.set = true;

        } while (VX_NULL_WHILE_LOOP_CONDITION);
    }

    return cache.value;
}

uint32_t get_processor_count_impl()
{
    const long count = ::sysconf(_SC_NPROCESSORS_ONLN);
    return count > 0 ? static_cast<uint32_t>(count) : 0;
}

bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    struct utsname u;
    if (::uname(&u) != 0)
    {
        unix_::error_message("uname()");
        return {};
    }

    // Linux kernel version like "5.15.0-105-generic"
    const unsigned int maj = 0, min = 0, pat = 0;
    if (std::sscanf(u.release, "%u.%u.%u", &maj, &min, &pat) < 2)
    {
        err::set(err::SYSTEM_ERROR, "invalid version file");
        return false;
    }

    if (major)
    {
        *major = static_cast<uint32_t>(maj);
    }
    if (minor)
    {
        *minor = static_cast<uint32_t>(min);
    }
    if (patch)
    {
        *patch = static_cast<uint32_t>(pat);
    }

    return true;
}

} // namespace os
} // namespace vx