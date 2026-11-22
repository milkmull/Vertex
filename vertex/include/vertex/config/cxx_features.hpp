#pragma once

#include "vertex/config/language.hpp"

//=========================================================================
// Const Eval
//=========================================================================

// VX_HAS_IS_CONSTANT_EVALUATED: 1 if VX_IS_CONSTANT_EVALUATED() is a real check, else 0.
#if defined(__cpp_lib_is_constant_evaluated) && (__cpp_lib_is_constant_evaluated >= 201811L)

    #define VX_IS_CONSTANT_EVALUATED()   std::is_constant_evaluated()
    #define VX_HAS_IS_CONSTANT_EVALUATED 1

#elif defined(__clang__) && VX_HAS_BUILTIN(__builtin_is_constant_evaluated) // includes clang-cl

    #define VX_IS_CONSTANT_EVALUATED()   __builtin_is_constant_evaluated()
    #define VX_HAS_IS_CONSTANT_EVALUATED 1

#elif (defined(__GNUC__) && !defined(__clang__) && (__GNUC__ >= 9)) || \
    (defined(_MSC_VER) && !defined(__clang__) && (_MSC_VER >= 1925))

    #define VX_IS_CONSTANT_EVALUATED()   __builtin_is_constant_evaluated()
    #define VX_HAS_IS_CONSTANT_EVALUATED 1

#else

    #define VX_IS_CONSTANT_EVALUATED()   false
    #define VX_HAS_IS_CONSTANT_EVALUATED 0

#endif

//=========================================================================
// Other Features
//=========================================================================

#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
    #define VX_HAVE_STD_LAUNDER 1
#else
    #define VX_HAVE_STD_LAUNDER 0
#endif

#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
    #define VX_HAVE_STD_CHAR8_T 1
#else
    #define VX_HAVE_STD_CHAR8_T 0
#endif

#if defined(__cpp_lib_string_view) && __cpp_lib_string_view >= 201606L
    #define VX_HAVE_STD_STRING_VIEW 1
#else
    #define VX_HAVE_STD_STRING_VIEW 0
#endif

// std::is_invocable / std::is_invocable_r
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703L
    #define VX_HAVE_STD_IS_INVOKABLE 1
#else
    #define VX_HAVE_STD_IS_INVOKABLE 0
#endif

// std::byte
#if defined(__cpp_lib_byte) && __cpp_lib_byte >= 201603L
    #define VX_HAVE_STD_BYTE 1
#else
    #define VX_HAVE_STD_BYTE 0
#endif

// std::is_pointer
#if defined(__cpp_lib_is_pointer) && __cpp_lib_is_pointer >= 201411L
    #define VX_HAVE_STD_IS_POINTER 1
#else
    #define VX_HAVE_STD_IS_POINTER 0
#endif

// std::is_pointer_interconvertible_base_of
#if defined(__cpp_lib_is_pointer_interconvertible) && \
    __cpp_lib_is_pointer_interconvertible >= 201907L
    #define VX_HAVE_STD_IS_POINTER_INTERCONVERTIBLE 1
#else
    #define VX_HAVE_STD_IS_POINTER_INTERCONVERTIBLE 0
#endif

// std::invoke
#if defined(__cpp_lib_invoke) && __cpp_lib_invoke >= 201411L
    #define VX_HAVE_STD_INVOKE 1
#else
    #define VX_HAVE_STD_INVOKE 0
#endif
