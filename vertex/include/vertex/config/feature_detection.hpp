#pragma once

#include "vertex/config/language_config.hpp"

#if defined(__has_builtin)
    #define VX_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define VX_HAS_BUILTIN(x) 0
#endif

#if defined(__has_include)
    #define VX_HAS_INCLUDE(header) __has_include(header)
#else
    #define VX_HAS_INCLUDE(header) 0
#endif

#if defined(__has_attribute)
    #define VX_HAS_COMPILER_ATTRIBUTE(x) __has_attribute(x)
#else
    #define VX_HAS_COMPILER_ATTRIBUTE(x) 0
#endif

#if defined(__has_cpp_attribute)
    #define VX_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define VX_HAS_ATTRIBUTE(x) 0
#endif

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
