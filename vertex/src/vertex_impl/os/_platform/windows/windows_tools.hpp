#pragma once

#include "vertex_impl/os/_platform/windows/windows_header.hpp"
#include "vertex/os/handle.hpp"

namespace vx {
namespace os {
namespace windows {

//=============================================================================
// Alignment
//=============================================================================

// See https://github.com/libsdl-org/SDL/pull/7607
// force_align_arg_pointer attribute requires gcc >= 4.2.x.
#if defined(__clang__)
#   define _HAVE_FORCE_ALIGN_ARG_POINTER
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#   define _HAVE_FORCE_ALIGN_ARG_POINTER
#endif

#if defined(__GNUC__) && defined(__i386__) && defined(_HAVE_FORCE_ALIGN_ARG_POINTER)
#   define VX_MINGW32_FORCE_ALIGN __attribute__((force_align_arg_pointer))
#else
#   define VX_MINGW32_FORCE_ALIGN
#endif

#undef _HAVE_FORCE_ALIGN_ARG_POINTER

//=============================================================================
// helpers
//=============================================================================

const char* check_default_args(int* pargc, char*** pargv, void** pallocated);

// NTSTATUS success check: non-negative means success
constexpr bool nt_success(LONG status) noexcept
{
    return status >= 0;
}

//=============================================================================
// Scoped Manager
//=============================================================================

template <typename T>
class scoped_manager
{
public:

    scoped_manager() { m_initializer.initialize(); }
    ~scoped_manager() { m_initializer.uninitialize(); }

    scoped_manager(const scoped_manager&) = delete;
    scoped_manager& operator=(const scoped_manager&) = delete;

    bool succeeded() const { return m_initializer.succeeded(); }

private:

    T m_initializer;
};

//=============================================================================
// COM
//=============================================================================

class com_initializer
{
public:

    com_initializer() noexcept = default;
    ~com_initializer() noexcept = default;

    com_initializer(const com_initializer&) = delete;
    com_initializer& operator=(const com_initializer&) = delete;

    bool initialize() noexcept;
    bool succeeded() const noexcept { return SUCCEEDED(m_hr); }
    void uninitialize() noexcept;

private:

    HRESULT m_hr = E_FAIL;
};

using com_scoped_initializer = scoped_manager<com_initializer>;

//=============================================================================
// OLE
//=============================================================================

class ole_initializer
{
public:

    ole_initializer() noexcept = default;
    ~ole_initializer() noexcept = default;

    ole_initializer(const ole_initializer&) = delete;
    ole_initializer& operator=(const ole_initializer&) = delete;

    bool initialize() noexcept;
    bool succeeded() const noexcept { return SUCCEEDED(m_hr); }
    void uninitialize() noexcept;

private:

    HRESULT m_hr = E_FAIL;
};

using ole_scoped_initializer = scoped_manager<ole_initializer>;

} // namespace windows
} // namespace os
} // namespace vx
