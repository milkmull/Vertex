#include <vector>

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/os/shared_library.hpp"

namespace vx {
namespace os {

static bool get_registry_value(const wchar_t* subkey, const wchar_t* name, std::string& value)
{
    bool success = false;

    HKEY key;
    LSTATUS status;
    DWORD type;

    std::vector<BYTE> data;
    DWORD size = 0;

    // Open the registry key for BIOS information
    status = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &key);
    if (status != ERROR_SUCCESS)
    {
        windows::error_message("RegOpenKeyExW()");
        goto cleanup;
    }

    // Query for the required size of the data
    status = ::RegQueryValueExW(
        key,
        name,
        NULL,
        &type,
        NULL,
        &size
    );

    if (status != ERROR_SUCCESS)
    {
        windows::error_message("RegQueryValueExW()");
        goto cleanup;
    }

    // Make sure type is null terminated unicode string
    if (!(type == REG_SZ || type == REG_EXPAND_SZ))
    {
        VX_ERR(err::SYSTEM_ERROR)
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
        windows::error_message("RegQueryValueExW()");
        goto cleanup;
    }

    value = str::string_cast<char>(
        reinterpret_cast<const wchar_t*>(data.data())
    );
    success = true;

    cleanup:
    {
        ::RegCloseKey(key);
    }

    return success;
}

std::string get_model_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
        if (!get_registry_value(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"SystemProductName",
            cache
        ))
        {
            cache = "Unknown";
        }
    }

    return cache;
}

std::string get_system_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
        WCHAR data[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

        if (!::GetComputerNameW(data, &size))
        {
            windows::error_message("GetComputerNameW()");
            cache = "Unknown";
        }
        else
        {
            cache = str::string_cast<char>(data);
        }
    }

    return cache;
}

std::string get_distribution_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
        if (!get_registry_value(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"ProductName",
            cache
        ))
        {
            cache = "Windows";
        }
    }

    return cache;
}

std::string get_processor_name_impl()
{
    static std::string cache;

    if (cache.empty())
    {
        if (!get_registry_value(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString",
            cache
        ))
        {
            cache = "Unknown";
        }
    }

    return cache;
}

uint32_t get_processor_count_impl()
{
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    return static_cast<uint32_t>(sysInfo.dwNumberOfProcessors);
}

bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    using RtlGetVersion_t = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW lpVersionInformation);

    shared_library ntdll;
    if (!ntdll.load("ntdll.dll"))
    {
        return false;
    }

    auto RtlGetVersion = ntdll.get<RtlGetVersion_t>("RtlGetVersion");
    if (!RtlGetVersion)
    {
        return false;
    }

    RTL_OSVERSIONINFOW info{ sizeof(RTL_OSVERSIONINFOW) };
    if (RtlGetVersion(&info) != STATUS_SUCCESS)
    {
        windows::error_message("RtlGetVersion");
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