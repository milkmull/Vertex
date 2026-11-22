#pragma once

//=========================================================================
// Warning Control
//=========================================================================

#if defined(_MSC_VER)

    #define VX_DISABLE_WARNING_PUSH()                        __pragma(warning(push, 0))
    #define VX_DISABLE_WARNING(warning_name, warning_number) __pragma(warning(disable : warning_number))
    #define VX_DISABLE_WARNING_POP()                         __pragma(warning(pop))

    #define VX_DISABLE_MSVC_WARNING_PUSH()          VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number) VX_DISABLE_WARNING("", warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()           VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()

#elif defined(__clang__)

    #define VX_DISABLE_WARNING_PUSH()                        _Pragma("clang diagnostic push")
    #define VX_DISABLE_WARNING(warning_name, warning_number) VX_PRIV_PRAGMA(clang diagnostic ignored warning_name)
    #define VX_DISABLE_WARNING_POP()                         _Pragma("clang diagnostic pop")

    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

#elif defined(__GNUC__)

    #define VX_DISABLE_WARNING_PUSH()                        _Pragma("GCC diagnostic push")
    #define VX_DISABLE_WARNING(warning_name, warning_number) VX_PRIV_PRAGMA(GCC diagnostic ignored warning_name)
    #define VX_DISABLE_WARNING_POP()                         _Pragma("GCC diagnostic pop")

    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_WARNING_POP()         VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

#else

    #define VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_WARNING(warning_name, warning_number)
    #define VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()

#endif
