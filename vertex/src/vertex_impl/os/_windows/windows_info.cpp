#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_INFO)

#include <vector>

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/os/library.hpp"

namespace vx {
namespace os {

struct value_cache
{
    bool set;
    std::string value;
};

static bool get_registry_value(const wchar_t* subkey, const wchar_t* name, std::string& value)
{
    bool success = false;

    HKEY key;
    LSTATUS status;
    DWORD type;

    std::vector<BYTE> data;
    DWORD size = 0;

    // Open the registry key for BIOS information
    status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &key);
    if (status != ERROR_SUCCESS)
    {
        windows_error_message("RegOpenKeyExW()");
        goto cleanup;
    }

    // Query for the required size of the data
    status = RegQueryValueExW(
        key,
        name,
        NULL,
        &type,
        NULL,
        &size
    );

    if (status != ERROR_SUCCESS)
    {
        windows_error_message("RegQueryValueExW()");
        goto cleanup;
    }

    // Make sure type is null terminated unicode string
    if (!(type == REG_SZ || type == REG_EXPAND_SZ))
    {
        VX_ERR(err::PLATFORM_ERROR)
            << "unsupported registry value type: " << type;
        goto cleanup;
    }

    data.resize(static_cast<size_t>(size));

    // Query for the actual data
    status = RegQueryValueExW(
        key,
        name,
        NULL,
        NULL,
        data.data(),
        &size
    );

    if (status != ERROR_SUCCESS)
    {
        windows_error_message("RegQueryValueExW()");
        goto cleanup;
    }

    value = str::string_cast<char>(
        reinterpret_cast<const wchar_t*>(data.data())
    );
    success = true;

    cleanup:
    {
        RegCloseKey(key);
    }

    return success;
}

VX_API const char* get_name()
{
    return "Windows";
}

VX_API std::string get_model_name()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        get_registry_value(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"SystemProductName",
            cache.value
        );
        cache.set = true;
    }

    return cache.value;
}

VX_API std::string get_system_name()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        WCHAR data[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

        if (!GetComputerNameW(data, &size))
        {
            windows_error_message("GetComputerNameW()");
        }
        else
        {
            cache.value = str::string_cast<char>(data);
        }

        cache.set = true;
    }

    return cache.value;
}

VX_API std::string get_processor_name()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        get_registry_value(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString",
            cache.value
        );
        cache.set = true;
    }

    return cache.value;
}

VX_API uint32_t get_processor_count()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<uint32_t>(sysInfo.dwNumberOfProcessors);
}

VX_API bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    using RtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW lpVersionInformation);

    library ntdll;
    if (!ntdll.load("ntdll.dll"))
    {
        return false;
    }

    RtlGetVersionPtr RtlGetVersion = ntdll.get_function<RtlGetVersionPtr>("RtlGetVersion");
    if (!RtlGetVersion)
    {
        return false;
    }

    RTL_OSVERSIONINFOW info{ sizeof(RTL_OSVERSIONINFOW) };
    if (RtlGetVersion(&info) != STATUS_SUCCESS)
    {
        windows_error_message("RtlGetVersion");
        return false;
    }

    if (major)
    {
        *major = static_cast<uint32_t>(info.dwMajorVersion);
    }
    if (minor)
    {
        *minor = static_cast<uint32_t>(info.dwMinorVersion);
    }
    if (patch)
    {
        *patch = static_cast<uint32_t>(info.dwBuildNumber);
    }

    return true;
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_INFO
