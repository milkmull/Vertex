#pragma once

#define VX_ENABLE_ENUM_BITWISE_OPERATORS(enum_type) \
\
inline constexpr enum_type operator|(enum_type lhs, enum_type rhs) noexcept \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs)); \
} \
\
inline constexpr enum_type& operator|=(enum_type& lhs, enum_type rhs) noexcept \
{ \
    lhs = lhs | rhs; \
    return lhs; \
} \
\
inline constexpr enum_type operator&(enum_type lhs, enum_type rhs) noexcept \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs)); \
} \
\
inline constexpr enum_type& operator&=(enum_type& lhs, enum_type rhs) noexcept \
{ \
    lhs = lhs & rhs; \
    return lhs; \
} \
\
inline constexpr enum_type operator^(enum_type lhs, enum_type rhs) noexcept \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs)); \
} \
\
inline constexpr enum_type& operator^=(enum_type& lhs, enum_type rhs) noexcept \
{ \
    lhs = lhs ^ rhs; \
    return lhs; \
} \
\
inline constexpr enum_type operator~(enum_type value) noexcept \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(~static_cast<underlying_type>(value)); \
} \
