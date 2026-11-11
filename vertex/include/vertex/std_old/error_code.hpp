#pragma once

#include "vertex/os/error.hpp"

namespace vx {

using error_type = os::native_error;

namespace err {

//=============================================================================
// error code
//=============================================================================

enum code : error_type
{
    // General success and error indicators
    none = error_type{}, // No error; indicates a successful operation or no action needed
    failed = 1,          // General error indicator

    // Runtime-specific errors
    runtime_error,  // Error occurring during runtime, often due to unexpected conditions
    not_configured, // Attempted operation without required configuration settings

    // Boundary and memory errors
    out_of_range,  // Attempted to access beyond valid range, e.g., index out of bounds
    out_of_memory, // Insufficient memory for requested operation
    size_error,    // Inappropriate or invalid size for an operation

    // Arguments
    invalid_data,     // Data provided is invalid or corrupted
    invalid_argument, // Provided argument is invalid or inappropriate for the function

    // Unsupported operations
    unsupported_format,     // Format of input or output is not supported
    unsupported_conversion, // Attempted conversion between incompatible types
    unsupported_operation,  // Operation requested is unsupported or not implemented

    // Resource-related errors
    resource_not_found,      // Resource, such as a file or database entry, could not be found
    resource_already_exists, // Resource already exists, e.g., trying to create a file that already exists
    permission_denied,

    // io errors
    eof, // End of input reached; no more data is available

    // Platform-specific error
    system_error, // Error specific to the platform, e.g., system-level failures

    // Count
    _count
};

/**
 * @brief Converts an error code to its human-readable string representation.
 *
 * Provides a static string description of the error code. Intended for debugging,
 * logging, or displaying user-friendly error messages.
 *
 * @param err The error code to describe.
 * @return A constant C-string describing the error.
 */
constexpr const char* code_to_string(code c) noexcept
{
    constexpr const char* strings[] = {
        "",       // none
        "failed", // failed

        "runtime error",  // runtime_error
        "not configured", // not_configured

        "out of range",  // out_of_range
        "out of memory", // out_of_memory
        "size error",    // size_error

        "invalid data",     // invalid_data
        "invalid argument", // invalid_argument

        "unsupported format",     // unsupported_format
        "unsupported conversion", // unsupported_conversion
        "unsupported operation",  // unsupported_operation

        "resource not found",      // resource_not_found
        "resource already exists", // resource_already_exists
        "permission denied",       // permission_denied

        "eof", // eof

        "system error" // system_error
    };

    constexpr size_t count = code::_count;
    const size_t index = static_cast<size_t>(c);
    return (index < count) ? strings[index] : strings[0];
}

//=============================================================================
// error / success shared base
//=============================================================================

class error;
class success;

namespace _error_priv {

template <typename Derived, typename Other>
class error_code_base
{
public:

    constexpr error_code_base() noexcept : m_code(err::none)
    {}
    constexpr explicit error_code_base(error_type c) noexcept : m_code(c)
    {}
    constexpr error_code_base(code c) noexcept : m_code(static_cast<error_type>(c))
    {}

    constexpr error_code_base(const Other& other) noexcept
        : m_code(other.value())
    {}

    constexpr error_code_base(const error_code_base&) noexcept = default;
    constexpr error_code_base(error_code_base&&) noexcept = default;

    constexpr error_code_base& operator=(const error_code_base&) noexcept = default;
    constexpr error_code_base& operator=(error_code_base&&) noexcept = default;

    constexpr error_type value() const noexcept
    {
        return m_code;
    }

    constexpr const char* message() const noexcept
    {
        return code_to_string(static_cast<code>(m_code));
    }

    constexpr explicit operator Other() const noexcept
    {
        return Other(m_code);
    }

    constexpr friend bool operator==(const Derived& lhs, const Derived& rhs) noexcept
    {
        return lhs.value() == rhs.value();
    }

    constexpr friend bool operator!=(const Derived& lhs, const Derived& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator==(const Derived& lhs, code rhs) noexcept
    {
        return lhs.value() == static_cast<error_type>(rhs);
    }

    constexpr friend bool operator==(code lhs, const Derived& rhs) noexcept
    {
        return rhs == lhs;
    }

private:

    error_type m_code;
};

} // namespace _error_priv

//=============================================================================
// error wrapper
//=============================================================================

class error : public _error_priv::error_code_base<error, success>
{
    using base = _error_priv::error_code_base<error, success>;

public:

    using base::base;

    // true if this represents an actual error (i.e. not err::none)
    constexpr explicit operator bool() const noexcept
    {
        return value() != err::none;
    }
};

//=============================================================================
// success wrapper
//=============================================================================

class success : public _error_priv::error_code_base<success, error>
{
    using base = _error_priv::error_code_base<success, error>;

public:

    using base::base;

    // true if this represents success (i.e. err::none)
    constexpr explicit operator bool() const noexcept
    {
        return value() == err::none;
    }
};

} // namespace err

using error = err::error;
using success = err::success;

} // namespace vx
