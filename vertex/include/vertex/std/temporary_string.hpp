#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/std/memory.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/string_traits.hpp"

namespace vx {
namespace str {

//==============================================================================
// basic_temporary_string
//==============================================================================

template <typename C, bool ASCII>
class basic_temporary_string
{
public:

    using char_type = C;
    static constexpr bool assumes_ascii = ASCII;

    //-- native, null-terminated: pointer only, no allocation, no conversion --

    basic_temporary_string(const C* cstr) noexcept
        : m_cstr(cstr)
    {}

    basic_temporary_string(const basic_string<C>& str) noexcept
        : m_cstr(str.c_str())
    {}

    basic_temporary_string(basic_cstring_view<C> view) noexcept
        : m_cstr(view.data() ? view.c_str() : _string_traits_priv::empty_string<C>())
    {}

    basic_temporary_string(basic_string_view<C> view) noexcept
    {
        copy_from(view.data(), view.size());
    }

    //-- foreign: convert into an owned buffer ------------------------------

    template <typename C2, VX_REQUIRES(type_traits::is_char<C2>::value && !std::is_same<C, C2>::value)>
    basic_temporary_string(const C2* cstr) noexcept
    {
        if (cstr)
        {
            convert_from(cstr, length(cstr));
        }
    }

    template <typename C2, VX_REQUIRES(type_traits::is_char<C2>::value && !std::is_same<C, C2>::value)>
    basic_temporary_string(const basic_string<C2>& str) noexcept
    {
        convert_from(str.data(), str.size());
    }

    template <typename C2, VX_REQUIRES(type_traits::is_char<C2>::value && !std::is_same<C, C2>::value)>
    basic_temporary_string(basic_string_view<C2> view) noexcept
    {
        convert_from(view.data(), view.size());
    }

    template <typename C2, VX_REQUIRES(type_traits::is_char<C2>::value && !std::is_same<C, C2>::value)>
    basic_temporary_string(basic_cstring_view<C2> view) noexcept
    {
        convert_from(view.data(), view.size());
    }

    ~basic_temporary_string()
    {
        if (m_owned)
        {
            mem::deallocate_raw(const_cast<C*>(m_cstr));
        }
    }

    basic_temporary_string(const basic_temporary_string& other) noexcept
        : m_cstr(other.m_cstr)
        , m_owned(false)
    {}

    basic_temporary_string(const basic_temporary_string&&) noexcept = delete;
    basic_temporary_string& operator=(const basic_temporary_string&) = delete;
    basic_temporary_string& operator=(basic_temporary_string&&) noexcept = delete;

    const C* c_str() const noexcept
    {
        return m_cstr;
    }

    bool is_valid() const noexcept
    {
        return m_cstr != nullptr;
    }

    explicit operator bool() const noexcept
    {
        return is_valid();
    }

private:

    // Allocates room for `len` units plus a terminator.
    // Returns nullptr on size overflow or allocation failure.
    static C* allocate_buffer(size_t len) noexcept
    {
        // +1 for the terminator; guard the byte-size multiplication
        if (len >= mem::max_array_size<C>())
        {
            return nullptr;
        }

        return static_cast<C*>(mem::allocate((len + 1) * sizeof(C)));
    }

    void copy_from(const C* src, size_t len) noexcept
    {
        if (len == 0)
        {
            // valid empty string, no allocation, can't fail
            m_cstr = _string_traits_priv::empty_string<C>();
            return;
        }

        C* buf = allocate_buffer(len);
        if (!buf)
        {
            return;
        }

        mem::copy(buf, src, len * sizeof(C));
        buf[len] = C(0);

        m_cstr = buf;
        m_owned = true;
    }

    template <typename C2>
    void convert_from(const C2* src, size_t len) noexcept
    {
        if (len == 0)
        {
            // valid empty string, no allocation, can't fail
            m_cstr = _string_traits_priv::empty_string<C>();
            return;
        }

        size_t out_len = 0;

        VX_IF_CONSTEXPR (ASCII)
        {
            out_len = string_cast_size_assume_ascii<C>(src, len);
        }
        else
        {
            out_len = string_cast_size<C>(src, len);
        }

        C* buf = allocate_buffer(out_len);
        if (!buf)
        {
            return;
        }

        string_cast_result<C2, C> r{};

        VX_IF_CONSTEXPR (ASCII)
        {
            r = string_cast_assume_ascii<C>(src, len, buf, out_len);
        }
        else
        {
            r = string_cast<C>(src, len, buf, out_len);
        }

        // size and cast should agree exactly; a mismatch means a utf_traits bug.
        // Harmless to the string itself, so debug-only.
        VX_ASSERT(r.dst_end == buf + out_len);

        if (!r.complete)
        {
            // truncated output: never hand out a partial string
            mem::deallocate_raw(buf);
            return;
        }

        *r.dst_end = C(0);
        m_cstr = buf;
        m_owned = true;
    }

private:

    const C* m_cstr = nullptr;
    bool m_owned = false;
};

template <typename C>
using temporary_string = basic_temporary_string<C, false>;

template <typename C>
using temporary_ascii_string = basic_temporary_string<C, true>;

} // namespace str
} // namespace vx
