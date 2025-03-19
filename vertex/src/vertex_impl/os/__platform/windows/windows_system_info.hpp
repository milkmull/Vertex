#pragma once

#if defined(VX_OS_SYSTEM_INFO_IMPL)
#   error "system info implemnation has already been included!"
#endif
#define VX_OS_SYSTEM_INFO_IMPL

#include <vector>

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/os/shared_library.hpp"

namespace vx {
namespace os {

namespace {

struct value_cache
{
    bool set;
    std::string value;
};

} // namespace

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
        windows::error_message("RegOpenKeyExW()");
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
        windows::error_message("RegQueryValueExW()");
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
        windows::error_message("RegQueryValueExW()");
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

static std::string get_model_name_impl()
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

static std::string get_system_name_impl()
{
    static value_cache cache = { false, "Unknown" };

    if (!cache.set)
    {
        WCHAR data[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

        if (!GetComputerNameW(data, &size))
        {
            windows::error_message("GetComputerNameW()");
        }
        else
        {
            cache.value = str::string_cast<char>(data);
        }

        cache.set = true;
    }

    return cache.value;
}

static std::string get_processor_name_impl()
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

static uint32_t get_processor_count_impl()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<uint32_t>(sysInfo.dwNumberOfProcessors);
}

static bool get_version_impl(uint32_t* major, uint32_t* minor, uint32_t* patch)
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