#pragma once

#define VX_ENABLE_ENUM_BITWISE_OPERATORS(enum_type) \
\
constexpr enum_type operator|(enum_type lhs, enum_type rhs) \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs)); \
} \
\
constexpr enum_type& operator|=(enum_type& lhs, enum_type rhs) \
{ \
    lhs = lhs | rhs; \
    return lhs; \
} \
\
constexpr enum_type operator&(enum_type lhs, enum_type rhs) \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs)); \
} \
\
constexpr enum_type& operator&=(enum_type& lhs, enum_type rhs) \
{ \
    lhs = lhs & rhs; \
    return lhs; \
} \
\
constexpr enum_type operator^(enum_type lhs, enum_type rhs) \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs)); \
} \
\
constexpr enum_type& operator^=(enum_type& lhs, enum_type rhs) \
{ \
    lhs = lhs ^ rhs; \
    return lhs; \
} \
\
constexpr enum_type operator~(enum_type value) \
{ \
    using underlying_type = typename std::underlying_type<enum_type>::type; \
    return static_cast<enum_type>(~static_cast<underlying_type>(value)); \
} \
