#pragma once

#include "vertex/config/type_traits.hpp"

//=============================================================================
// scoped enum
//=============================================================================

#define VX_SCOPED_ENUM_UT_DECLARE_BEGIN(name, ut) \
struct name \
{ \
    using underlying_type = ut; \
\
    name() = default; \
    explicit constexpr name(underlying_type x) noexcept : m_value(x) {} \
    explicit constexpr operator underlying_type() const noexcept { return m_value; } \
\
private: \
\
    underlying_type m_value; \
\
public: \
\
    enum enum_type : underlying_type

#define VX_SCOPED_ENUM_DECLARE_BEGIN(name) VX_SCOPED_ENUM_UT_DECLARE_BEGIN(name, int)

#define _VX_SCOPED_ENUM_DECLARE_FUNCTIONS(name) \
constexpr name(enum_type x) noexcept : m_value(x) {} \
constexpr explicit operator bool() const noexcept { return static_cast<bool>(m_value); } \
\
friend constexpr bool operator==(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) == static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator==(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) == rhs; } \
friend constexpr bool operator==(enum_type lhs, name rhs) noexcept { return lhs == static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator!=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) != static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator!=(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) != rhs; } \
friend constexpr bool operator!=(enum_type lhs, name rhs) noexcept { return lhs != static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator<(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) < static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator<(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) < rhs; } \
friend constexpr bool operator<(enum_type lhs, name rhs) noexcept { return lhs < static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator<=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) <= static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator<=(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) <= rhs; } \
friend constexpr bool operator<=(enum_type lhs, name rhs) noexcept { return lhs <= static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator>(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) > static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator>(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) > rhs; } \
friend constexpr bool operator>(enum_type lhs, name rhs) noexcept { return lhs > static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator>=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) >= static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator>=(name lhs, enum_type rhs) noexcept { return static_cast<enum_type>(lhs.m_value) >= rhs; } \
friend constexpr bool operator>=(enum_type lhs, name rhs) noexcept { return lhs >= static_cast<enum_type>(rhs.m_value); }

#define VX_SCOPED_ENUM_DECLARE_END(name) \
; \
_VX_SCOPED_ENUM_DECLARE_FUNCTIONS(name) \
};