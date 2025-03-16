#pragma once

namespace vx {
namespace os {
namespace windows {

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/efda8314-6e41-4837-8299-38ba0ee04b92

typedef void* HANDLE_;
typedef HANDLE_ HINSTANCE_;
typedef HINSTANCE_ HMODULE_;

typedef __int64 LONG_PTR_;

typedef unsigned long DWORD_, *PDWORD_, *LPDWORD_;

} // namespace windows
} // namespace os
} // namespace vx
