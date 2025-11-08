#pragma once

#include "vertex/config/language_config.hpp"

//=============================================================================
// scoped flag enum
//=============================================================================

#define VX_FLAGS_UT_DECLARE_BEGIN(name, ut) \
struct name \
{ \
    using underlying_type = ut; \
\
    VX_DISABLE_WARNING_PUSH() \
    VX_DISABLE_WARNING("-Wuninitialized", 26495) \
    name() noexcept = default; \
    VX_DISABLE_WARNING_POP() \
\
    constexpr explicit name(underlying_type x) noexcept : m_value(x) {} \
    constexpr explicit operator underlying_type() const noexcept { return m_value; } \
\
private: \
\
    underlying_type m_value; \
\
public: \
\
    enum enum_type : underlying_type

#define VX_FLAGS_DECLARE_BEGIN(name) VX_FLAGS_UT_DECLARE_BEGIN(name, int)

#define _VX_FLAGS_DECLARE_FUNCTIONS(name) \
\
constexpr name(enum_type x) noexcept : m_value(static_cast<underlying_type>(x)) {} \
constexpr explicit operator enum_type() const noexcept {return static_cast<enum_type>(m_value); } \
\
constexpr explicit operator bool() const noexcept { return !!m_value; } \
\
friend constexpr bool operator==(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) == static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator!=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) != static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator<(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) < static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator<=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) <= static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator>(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) > static_cast<enum_type>(rhs.m_value); } \
friend constexpr bool operator>=(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value) >= static_cast<enum_type>(rhs.m_value); } \
\
friend constexpr bool operator!(name lhs) noexcept { return !static_cast<underlying_type>(lhs.m_value); } \
\
friend constexpr enum_type operator|(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value | rhs.m_value); } \
friend constexpr enum_type operator|(enum_type lhs, enum_type rhs) noexcept { return static_cast<enum_type>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs)); } \
\
friend constexpr enum_type operator&(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value & rhs.m_value); } \
friend constexpr enum_type operator&(enum_type lhs, enum_type rhs) noexcept { return static_cast<enum_type>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs)); } \
\
friend constexpr enum_type operator^(name lhs, name rhs) noexcept { return static_cast<enum_type>(lhs.m_value ^ rhs.m_value); } \
friend constexpr enum_type operator^(enum_type lhs, enum_type rhs) noexcept { return static_cast<enum_type>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs)); } \
\
friend constexpr enum_type operator~(name lhs) noexcept { return static_cast<enum_type>(~lhs.m_value); } \
friend constexpr enum_type operator~(enum_type lhs) noexcept { return static_cast<enum_type>(~static_cast<underlying_type>(lhs)); } \
\
friend constexpr name& operator|=(name& lhs, name rhs) noexcept { lhs = lhs | rhs; return lhs; } \
friend constexpr name& operator|=(name& lhs, enum_type rhs) noexcept { lhs = lhs | rhs; return lhs; } \
\
friend constexpr name& operator&=(name& lhs, name rhs) noexcept { lhs = lhs & rhs; return lhs; } \
friend constexpr name& operator&=(name& lhs, enum_type rhs) noexcept { lhs = lhs & rhs; return lhs; } \
\
friend constexpr name& operator^=(name& lhs, name rhs) noexcept { lhs = lhs ^ rhs; return lhs; } \
friend constexpr name& operator^=(name& lhs, enum_type rhs) noexcept { lhs = lhs ^ rhs; return lhs; }

#define VX_FLAGS_DECLARE_END(name) \
; \
_VX_FLAGS_DECLARE_FUNCTIONS(name) \
};