#pragma once

namespace vx {
namespace test {

//=============================================================================
// string literal helpers
//=============================================================================

template <typename char_t>
constexpr const char_t* get_string_literal(
    const char*,
    const wchar_t*,
#if defined(__cpp_lib_char8_t)
    const char8_t*,
#endif
    const char16_t*,
    const char32_t*);

template <>
constexpr const char* get_string_literal<char>(
    const char* c,
    const wchar_t*,
#if defined(__cpp_lib_char8_t)
    const char8_t*,
#endif
    const char16_t*,
    const char32_t*)
{
    return c;
}

template <>
constexpr const wchar_t* get_string_literal<wchar_t>(
    const char*,
    const wchar_t* w,
#if defined(__cpp_lib_char8_t)
    const char8_t*,
#endif
    const char16_t*,
    const char32_t*)
{
    return w;
}

#if defined(__cpp_lib_char8_t)

template <>
constexpr const char8_t* get_string_literal<char8_t>(
    const char*,
    const wchar_t*,
    const char8_t* c8,
    const char16_t*,
    const char32_t*)
{
    return c8;
}

#endif

template <>
constexpr const char16_t* get_string_literal<char16_t>(
    const char*,
    const wchar_t*,
#if defined(__cpp_lib_char8_t)
    const char8_t*,
#endif
    const char16_t* c16,
    const char32_t*)
{
    return c16;
}

template <>
constexpr const char32_t* get_string_literal<char32_t>(
    const char*,
    const wchar_t*,
#if defined(__cpp_lib_char8_t)
    const char8_t*,
#endif
    const char16_t*,
    const char32_t* c32)
{
    return c32;
}

#if defined(__cpp_lib_char8_t)
    #define VX_LIT(T, s) ::vx::test::get_string_literal<T>(s, L##s, u8##s, u##s, U##s)
#else
    #define VX_LIT(T, s) ::vx::test::get_string_literal<T>(s, L##s, u##s, U##s)
#endif

//=============================================================================
// specialized int types
//=============================================================================

class trivial_int
{
public:

    int val;

    operator int() const
    {
        return val;
    }

    bool operator==(const trivial_int& x) const
    {
        return val == x.val;
    }

    bool operator!=(const trivial_int& x) const
    {
        return val != x.val;
    }

    bool operator<(const trivial_int& x) const
    {
        return val < x.val;
    }
};

class copyable_int
{
public:

    int val;

    copyable_int(int v = 0)
        : val(v)
    {}

    copyable_int(const copyable_int& x)
        : val(x.val)
    {}

    copyable_int& operator=(const copyable_int& x)
    {
        val = x.val;
        return *this;
    }

    operator int() const
    {
        return val;
    }

    bool operator==(const copyable_int& x) const
    {
        return val == x.val;
    }

    bool operator!=(const copyable_int& x) const
    {
        return val != x.val;
    }

    bool operator<(const copyable_int& x) const
    {
        return val < x.val;
    }

    copyable_int(copyable_int&& x) noexcept
        : val(x.val)
    {
        x.val = -1;
    }

    copyable_int& operator=(copyable_int&& x) noexcept
    {
        val = x.val;
        x.val = -1;
        return *this;
    }
};

class movable_int
{
public:

    int val;

    movable_int(int v = 0)
        : val(v)
    {}

    movable_int(int v1, int v2)
        : val(v2 + (v1 << 4))
    { // construct from two values
    }

    movable_int(int v1, int v2, int v3)
        : val(v3 + (v2 << 4) + (v1 << 8))
    { // construct from three values
    }

    movable_int(int v1, int v2, int v3, int v4)
        : val(v4 + (v3 << 4) + (v2 << 8) + (v1 << 12))
    { // construct from four values
    }

    movable_int(int v1, int v2, int v3, int v4, int v5)
        : val(v5 + (v4 << 4) + (v3 << 8) + (v2 << 12) + (v1 << 16))
    { // construct from five values
    }

    movable_int(movable_int&& right) noexcept
        : val(right.val)
    {
        right.val = -1;
    }

    movable_int& operator=(movable_int&& right) noexcept
    {
        if (this != &right)
        {
            val = right.val;
            right.val = -1;
        }
        return *this;
    }

    operator int() const
    {
        return val;
    }

    bool operator==(const movable_int& x) const
    {
        return val == x.val;
    }

    bool operator!=(const movable_int& x) const
    {
        return val != x.val;
    }

    bool operator<(const movable_int& x) const
    {
        return val < x.val;
    }

    movable_int(const movable_int&) = delete;
    movable_int& operator=(const movable_int&) = delete;
};

} // namespace test
} // namespace vx
