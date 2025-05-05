#include <sys/utsname.h>
#include <unistd.h>

#if defined(HAVE_SYSCTL) || defined(HAVE_SYSCTLBYNAME)
#   include <sys/types.h>
#   include <sys/sysctl.h>
#endif

#include "vertex_impl/os/__platform/unix/unix_tools.hpp"
#include "vertex/os/file.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {

#if defined(HAVE_SYSCTLBYNAME)

static bool get_sysctl_string(const char* name, std::string& value)
{
    size_t size = 0;

    // First call to get the size
    if (::sysctlbyname(name, NULL, &size, NULL, 0) != 0)
    {
        unix_::error_message("sysctlbyname()");
        return false;
    }

    std::vector<char> buffer(size);

    // Second call to get the actual value
    if (::sysctlbyname(name, buffer.data(), &size, NULL, 0) != 0)
    {
        unix_::error_message("sysctlbyname()");
        return false;
    }

    // Trim any trailing nulls and return as string
    value = std::string(buffer.data(), std::strnlen(buffer.data(), size));
    return true;
}

#endif // HAVE_SYSCTLBYNAME

static std::string get_os_release_info_value(const char* key)
{
    os::file f;
    if (!f.open("/etc/os-release", os::file::mode::READ))
    {
        return {};
    }

    const std::string key_equals = key + '=';

    std::string line;
    while (f.read_line(line))
    {
        if (str::starts_with(line, key_equals))
        {
            // Strip whitespace and quotes
            const char* characters = " \t\n\v\f\r\"";
            const size_t off = key_equals.size();
            return str::strip(line, characters, off);
        }
    }

    return {};
}

std::string get_model_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
#if defined(HAVE_SYSCTLBYNAME)

        get_sysctl_string("hw.model", cache);

#elif defined(VX_OS_LINUX)

        const char* paths[] = {
            "/proc/device-tree/model",                  // Best for Raspberry Pi, Jetson, other ARM SBCs
            "/sys/class/dmi/id/product_name",           // Standard on x86 PCs
            "/sys/devices/virtual/dmi/id/product_name", // Alternate path for DMI info
            "/sys/firmware/devicetree/base/model"       // Alternate device-tree path on some ARM boards
        };

        for (const char* path : paths)
        {
            if (os::file::read_file(path, cache))
            {
                break;
            }
        }

#endif

        if (cache.empty())
        {
            cache = "Unknown";
        }
    }

    return cache;
}

std::string get_distribution_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
#if defined(VX_OS_LINUX)
        cache = get_os_release_info_value("NAME");
#endif

        if (cache.empty())
        {
            struct utsname u;
            ::uname(&u);
            cache = u.sysname;
        }

        if (cache.empty())
        {
            cache = "Unknown";
        }
    }

    return cache;
}

std::string get_system_name_impl()
{
    char hostname[256];
    if (::gethostname(hostname, sizeof(hostname)) != 0)
    {
        unix_::error_message("gethostname()");
        return {};
    }

    return std::string(hostname);
}

std::string get_processor_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
#if defined(HAVE_SYSCTLBYNAME)

        get_sysctl_string("machdep.cpu.brand_string", cache);

#elif defined(HAVE_SYSCTL)

        // https://github.com/godotengine/godot/blob/1cf573f44de045ee73fd938fbf2bdccc0e31b7bb/platform/linuxbsd/os_linuxbsd.cpp#L190C13-L190C24

        const int mib[2] = { CTL_HW, HW_MODEL };
        char buffer[4096]{ 0 };
        size_t size = sizeof(buffer);

        if (::sysctl(mib, 2, buffer, &size, NULL, 0) != -1)
        {
            cache.assign(buffer);
        }

#endif

        if (cache.empty())
        {
            do
            {
                os::file f;
                if (!f.open("/proc/cpuinfo", os::file::mode::READ))
                {
                    break;
                }

                const char* keys[] = {
                    "model name",
                    "model",
                    "hardware"
                };

                std::string line;
                std::string lower_line;

                while (f.read_line(line))
                {
                    lower_line = str::to_lower(line);
                    size_t off = std::string::npos;

                    for (const char* key : keys)
                    {
                        off = lower_line.find(key);
                        if (off != std::string::npos)
                        {
                            break;
                        }
                    }

                    if (off == std::string::npos)
                    {
                        continue;
                    }

                    off = line.find(':', off);
                    if (off != std::string::npos)
                    {
                        // Skip any whitespace
                        cache = str::strip(line, " \t", off + 1);
                        break;
                    }
                }

            } while (VX_NULL_WHILE_LOOP_CONDITION);
        }

        // Fallback
        if (cache.empty())
        {
            cache = get_model_name_impl();
        }
    }

    return cache;
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
    unsigned int maj = 0, min = 0, pat = 0;
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