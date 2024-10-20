#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include <vector>

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/stdlib/string/string.hpp"
#include "vertex/os/library.hpp"

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
    status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &key);
    if (status != ERROR_SUCCESS)
    {
        WindowsErrorMessage("RegOpenKeyExW()");
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
        WindowsErrorMessage("RegQueryValueExW()");
        goto cleanup;
    }

    // Make sure type is null terminated unicode string
    if (!(type == REG_SZ || type == REG_EXPAND_SZ))
    {
        VX_ERROR(error::error_code::PLATFORM_ERROR)
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
        WindowsErrorMessage("RegQueryValueExW()");
        goto cleanup;
    }

    value = str::wstring_to_string(
        reinterpret_cast<const wchar_t*>(data.data())
    );
    success = true;

    cleanup:
    {
        RegCloseKey(key);
    }

    return success;
}

const char* get_name()
{
    return "Windows";
}

std::string get_model_name()
{
    std::string value("Unknown");

    get_registry_value(
        L"HARDWARE\\DESCRIPTION\\System\\BIOS",
        L"SystemProductName",
        value
    );

    return value;
}

std::string get_system_name()
{
    std::string name;

    WCHAR data[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

    if (!GetComputerNameW(data, &size))
    {
        WindowsErrorMessage("GetComputerNameW()");
        return name;
    }

    name = str::wstring_to_string(data);
    return name;
}

std::string get_processor_name()
{
    std::string value("Unknown");

    get_registry_value(
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        L"ProcessorNameString",
        value
    );

    return value;
}

uint32_t get_processor_count()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<uint32_t>(sysInfo.dwNumberOfProcessors);
}

bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch)
{
    typedef long NTSTATUS;
    using RtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW lpVersionInformation);
#   define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

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
        WindowsErrorMessage("RtlGetVersion");
        return false;
    }

#   undef STATUS_SUCCESS

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

#endif // VX_PLATFORM_WINDOWS
