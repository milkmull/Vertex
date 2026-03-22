#pragma once

#include <ratio>
#include <sstream>
#include <string>

#include "vertex/std/char_traits.hpp"
#include "vertex/std/string.hpp"

namespace vx {
namespace str {

template <size_t N, typename T>
class basic_static_string
{
private:

    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");
    VX_STATIC_ASSERT_MSG(N > 0, "N must be greater than 0");

    template <typename S>
    struct is_compatible_string : is_string_of<S, T>
    {};

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = char_traits<T>;

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = vx::_priv::pointer_iterator<basic_static_string, T>;
    using const_iterator = vx::_priv::pointer_iterator<basic_static_string, const T>;
    using reverse_iterator = vx::_priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = vx::_priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

private:

    struct buffer_type
    {
        T ptr[N + 1];
        size_type size;
    };

    buffer_type m_buffer = {};

private:

    //=========================================================================
    // allocation helpers
    //=========================================================================

    static void destroy_size(T* ptr, size_type size)
    {
        mem::destroy_range(ptr, size + 1);
    }

    void destroy_range()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size > 0)
        {
            destroy_size(ptr, size);
            size = 0;
        }
    }

    //=========================================================================
    // constructor helpers
    //=========================================================================

    bool construct_empty()
    {
        m_buffer.size = 0;
        mem::construct_in_place_maybe_trivial(m_buffer.ptr);
        traits_type::assign(*m_buffer.ptr, T());
        return true;
    }

    enum class construct_method
    {
        from_char,
        from_char_count,
        from_pointer,
        from_string,
        from_iterator_range
    };

    template <construct_method M, typename... Args>
    void construct_n(size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (count > max_size())
        {
            err::set(err::size_error);
            return;
        }

        const size_type alloc_count = count + 1;
        size = count;

        mem::construct_range_maybe_trivial(ptr, alloc_count);

        VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _priv::copy_batch(ptr, std::forward<Args>(args)..., count);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            traits_type::copy(ptr, std::forward<Args>(args)..., alloc_count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
    }

public:

    //=========================================================================
    // constructors
    //=========================================================================

    basic_static_string()
    {
        construct_empty();
    }

    basic_static_string(const basic_static_string& other)
    {
        construct_n<construct_method::from_string>(other.size(), other.data());
    }

    //=========================================================================

    basic_static_string(const basic_static_string& other, size_type off)
    {
        if (_priv::check_offset(other.size(), off))
        {
            construct_n<construct_method::from_pointer>(other.size() - off, other.data() + off);
        }
        else
        {
            construct_empty();
        }
    }

    basic_static_string(const basic_static_string& other, size_type off, size_type count)
    {
        if (_priv::check_offset(other.size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), off, count));
            construct_n<construct_method::from_pointer>(count, other.data() + off);
        }
        else
        {
            construct_empty();
        }
    }

    //=========================================================================

    basic_static_string(size_type count, const T value)
    {
        construct_n<construct_method::from_char_count>(count, value);
    }

    //=========================================================================

    basic_static_string(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        construct_n<construct_method::from_pointer>(count, ptr);
    }

    basic_static_string(const T* const ptr, size_type count)
    {
        construct_n<construct_method::from_pointer>(count, ptr);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            construct_n<construct_method::from_pointer>(count, first.ptr());
        }
        else
        {
            construct_n<construct_method::from_iterator_range>(count, first, last);
        }
    }

    //=========================================================================

    basic_static_string(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        construct_n<construct_method::from_pointer>(count, init.begin());
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string(const S& t)
    {
        const size_type count = static_cast<size_type>(t.size());
        construct_n<construct_method::from_pointer>(count, t.data());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string(const S& t, size_type off, size_type count = npos)
    {
        if (_priv::check_offset(t.size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), off, count));
            construct_n<construct_method::from_pointer>(count, t.data() + off);
        }
        else
        {
            construct_empty();
        }
    }

    //=========================================================================

    template <typename Traits2, typename Allocator2>
    basic_static_string(const std::basic_string<T, Traits2, Allocator2>& str)
    {
        const size_type count = static_cast<size_type>(str.size());
        construct_n<construct_method::from_pointer>(count, str.data());
    }

    template <typename Traits2>
    basic_static_string(const std::basic_string_view<T, Traits2>& str)
    {
        const size_type count = static_cast<size_type>(str.size());
        construct_n<construct_method::from_pointer>(count, str.data());
    }

    //=========================================================================

    bool is_valid() const noexcept
    {
        return true;
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~basic_static_string()
    {
        destroy_range();
    }

    //=========================================================================
    // operators
    //=========================================================================

    operator basic_string_view<T>() const noexcept
    {
        return basic_string_view<T>(data(), size());
    }

    template <typename Traits2, typename Allocator2>
    operator std::basic_string<T, Traits2, Allocator2>() const
    {
        return std::basic_string<T, Traits2, Allocator2>(data(), size());
    }

#if defined(__cpp_lib_string_view)

    template <typename Traits2>
    operator std::basic_string_view<T, Traits2>() const noexcept
    {
        return std::basic_string_view<T, Traits2>(data(), size());
    }

#endif // defined(__cpp_lib_string_view)

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool assign_from(const size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (count > max_size())
        {
            err::set(err::size_error);
            return false;
        }

        if (count > size)
        {
            mem::construct_range_maybe_trivial(ptr + size + 1, count - size);
        }
        if (count < size)
        {
            mem::destroy_range(ptr + count + 1, size - count);
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _priv::copy_batch(ptr, std::forward<Args>(args)..., count);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            traits_type::copy(ptr, std::forward<Args>(args)..., count + 1);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            traits_type::copy_range(ptr, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }

        size = count;
        return true;
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    basic_static_string& operator=(const basic_static_string& other)
    {
        assign_from<construct_method::from_string>(other.size(), other.data());
        return *this;
    }

    basic_static_string& operator=(const T c)
    {
        assign_from<construct_method::from_char>(1, c);
        return *this;
    }

    basic_static_string& operator=(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        assign_from<construct_method::from_pointer>(count, ptr);
        return *this;
    }

    basic_static_string& operator=(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        assign_from<construct_method::from_pointer>(count, init.begin());
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& operator=(const S& t)
    {
        const size_type count = static_cast<size_type>(t.size());
        assign_from<construct_method::from_pointer>(count, t.data());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    basic_static_string& assign(const basic_static_string& other)
    {
        return operator=(other);
    }

    //=========================================================================

    basic_static_string& assign(const T c)
    {
        return operator=(c);
    }

    basic_static_string& assign(const size_type count, const T c)
    {
        assign_from<construct_method::from_char_count>(count, c);
        return *this;
    }

    //=========================================================================

    basic_static_string& assign(const T* const ptr)
    {
        return operator=(ptr);
    }

    basic_static_string& assign(const T* const ptr, size_type count)
    {
        assign_from<construct_method::from_pointer>(count, ptr);
        return *this;
    }

    //=========================================================================

    basic_static_string& assign(std::initializer_list<T> init)
    {
        return operator=(init);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string& assign(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            assign_from<construct_method::from_pointer>(count, first.ptr());
        }
        else
        {
            assign_from<construct_method::from_iterator_range>(count, first, last);
        }

        return *this;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& assign(const S& t)
    {
        return operator=(t);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& assign(const S& t, size_type off, size_type count = npos)
    {
        if (_priv::check_offset(t.size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), off, count));
            assign_from<construct_method::from_pointer>(count, t.data() + off);
        }
        else
        {
            clear();
        }
        return *this;
    }

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        VX_ASSERT(m_buffer.size > 0);
        return m_buffer.ptr[0];
    }

    const T& front() const noexcept
    {
        VX_ASSERT(m_buffer.size > 0);
        return m_buffer.ptr[0];
    }

    T& back() noexcept
    {
        VX_ASSERT(m_buffer.size > 0);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    const T& back() const noexcept
    {
        VX_ASSERT(m_buffer.size > 0);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    T* data() noexcept
    {
        return m_buffer.ptr;
    }

    const T* data() const noexcept
    {
        return m_buffer.ptr;
    }

    T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_buffer.size);
        return m_buffer.ptr[i];
    }

    const T* c_str() const noexcept
    {
        return m_buffer.ptr;
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_buffer.ptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_buffer.ptr);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_buffer.ptr + m_buffer.size);
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

private:

    //=========================================================================
    // append helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool append_n(const size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type available = N - size;
        if (count > available)
        {
            err::set(err::size_error);
            return false;
        }

        T* const dst = ptr + size;

        // we increase the size early so we can easily assign the null terminator at the end
        size += count;
        mem::construct_range_maybe_trivial(dst + 1, count);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _priv::copy_batch(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(dst, std::forward<Args>(args)...);
        }

        traits_type::assign(ptr[size], T());
        return true;
    }

public:

    //=========================================================================
    // append
    //=========================================================================

    basic_static_string& append(const basic_static_string& other)
    {
        append_n<construct_method::from_pointer>(other.size(), other.data());
        return *this;
    }

    //=========================================================================

    basic_static_string& append(const basic_static_string& other, size_type off, size_type count = npos)
    {
        if (_priv::check_offset(other.size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), off, count));
            append_n<construct_method::from_pointer>(count, other.data() + off);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& append(const T c)
    {
        append_n<construct_method::from_char>(1, c);
        return *this;
    }

    basic_static_string& append(size_type count, const T c)
    {
        append_n<construct_method::from_char_count>(count, c);
        return *this;
    }

    //=========================================================================

    basic_static_string& append(const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        append_n<construct_method::from_pointer>(count, s);
        return *this;
    }

    basic_static_string& append(const T* const s, const size_type count)
    {
        append_n<construct_method::from_pointer>(count, s);
        return *this;
    }

    //=========================================================================

    basic_static_string& append(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        append_n<construct_method::from_pointer>(count, init.begin());
        return *this;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string& append(IT first, IT last)
    {
        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            append_n<construct_method::from_pointer>(count, first.ptr());
        }
        else
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            append_n<construct_method::from_iterator_range>(count, first, last);
        }
        return *this;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& append(const S& t)
    {
        const size_type count = static_cast<size_type>(t.size());
        append_n<construct_method::from_pointer>(count, t.data());
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& append(const S& t, size_type off, size_type count = npos)
    {
        if (_priv::check_offset(t.size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), off, count));
            append_n<construct_method::from_pointer>(count, t.data() + off);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& operator+=(const basic_static_string& other)
    {
        return append(other);
    }

    basic_static_string& operator+=(const T c)
    {
        return append(c);
    }

    basic_static_string& operator+=(const T* const s)
    {
        return append(s);
    }

    basic_static_string& operator+=(std::initializer_list<T> init)
    {
        return append(init);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& operator+=(const S& t)
    {
        return append(t);
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    T* insert_n(const T* cpos, const size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type available = N - size;
        if (count > available)
        {
            err::set(err::size_error);
            return nullptr;
        }

        auto pos = const_cast<T*>(cpos);

        // initialize the new elements that will be moved into uninitialized memory
        const pointer back = ptr + size;
        mem::construct_range_maybe_trivial(back + 1, count);

        // move the tail backward to make room for the new elements
        const size_type tail_count = static_cast<size_type>(back - pos) + 1;
        _priv::move_batch(pos + count, pos, tail_count);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(*pos, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(pos, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _priv::copy_batch(pos, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(pos, std::forward<Args>(args)...);
        }

        size += count;
        return pos;
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    basic_static_string& insert(size_type off, const basic_static_string& other)
    {
        return insert(off, other.data(), other.size());
    }

    basic_static_string& insert(size_type off, const basic_static_string& other, size_type other_off, size_type count = npos)
    {
        if (!_priv::check_offset(other.size(), off))
        {
            return *this;
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), other_off, count));
        return insert(off, other.data() + other_off, count);
    }

    //=========================================================================

    basic_static_string& insert(size_type off, const T c)
    {
        insert_n<construct_method::from_char>(m_buffer.ptr + off, 1, c);
        return *this;
    }

    basic_static_string& insert(size_type off, size_type count, const T c)
    {
        insert_n<construct_method::from_char_count>(m_buffer.ptr + off, count, c);
        return *this;
    }

    //=========================================================================

    basic_static_string& insert(size_type off, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, s);
        return *this;
    }

    basic_static_string& insert(size_type off, const T* const s, size_type count)
    {
        insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, s);
        return *this;
    }

    //=========================================================================

    basic_static_string& insert(size_type off, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, init.begin());
        return *this;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string& insert(size_type off, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, first.ptr());
        }
        else
        {
            insert_n<construct_method::from_iterator_range>(m_buffer.ptr + off, count, first, last);
        }
        return *this;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& insert(size_type off, const S& t)
    {
        const size_type count = static_cast<size_type>(t.size());
        insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, t.data());
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& insert(size_type off, const S& t, size_type t_off, size_type count = npos)
    {
        if (_priv::check_offset(t.size(), t_off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), t_off, count));
            insert_n<construct_method::from_pointer>(m_buffer.ptr + off, count, t.data() + t_off);
        }
        return *this;
    }

    //=========================================================================
    //=========================================================================

    template <size_t M, VX_REQUIRES(M <= N)>
    iterator insert(const_iterator pos, const basic_static_string<M, T>& other)
    {
        return insert(pos, other.data(), other.size());
    }

    template <size_t M>
    iterator insert(const_iterator pos, const basic_static_string<M, T>& other, size_type other_off, size_type count = npos)
    {
        if (!_priv::check_offset(other.size(), other_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), other_off, count));
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, other.data() + other_off);
        return iterator(new_pos);
    }

    //=========================================================================

    iterator insert(const_iterator pos, const T c)
    {
        pointer new_pos = insert_n<construct_method::from_char>(pos.ptr(), 1, c);
        return iterator(new_pos);
    }

    iterator insert(const_iterator pos, const size_type count, const T c)
    {
        pointer new_pos = insert_n<construct_method::from_char_count>(pos.ptr(), count, c);
        return iterator(new_pos);
    }

    //=========================================================================

    iterator insert(const_iterator pos, const T* const s, size_type count)
    {
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, s);
        return iterator(new_pos);
    }

    iterator insert(const_iterator pos, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, s);
        return iterator(new_pos);
    }

    //=========================================================================

    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, init.begin());
        return iterator(new_pos);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        pointer new_pos;
        const size_type count = static_cast<size_type>(std::distance(first, last));
        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, first.ptr());
        }
        else
        {
            new_pos = insert_n<construct_method::from_iterator_range>(pos.ptr(), count, first, last);
        }
        return iterator(new_pos);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    iterator insert(const_iterator pos, const S& t)
    {
        const size_type count = static_cast<size_type>(t.size());
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, t.data());
        return iterator(new_pos);
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    iterator insert(const_iterator pos, const S& t, size_type t_off, size_type count = npos)
    {
        if (!_priv::check_offset(t.size(), t_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), t_off, count));
        pointer new_pos = insert_n<construct_method::from_pointer>(pos.ptr(), count, t.data() + t_off);
        return iterator(new_pos);
    }

public:

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        destroy_size(m_buffer.ptr, m_buffer.size);
        m_buffer.size = 0;
        traits_type::assign(*m_buffer.ptr, T());
    }

    void clear_and_deallocate()
    {
        clear();
    }

    static bool shrink_to_fit()
    {
        return true;
    }

    void swap(basic_static_string& other) noexcept
    {
        std::swap(m_buffer, other.m_buffer);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_buffer.size == 0;
    }

    bool full() const noexcept
    {
        return m_buffer.size == N;
    }

    size_type size() const noexcept
    {
        return m_buffer.size;
    }

    size_type length() const noexcept
    {
        return size();
    }

    size_type size_bytes() const noexcept
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size() noexcept
    {
        return N;
    }

    static constexpr size_type capacity() noexcept
    {
        return N;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    static constexpr bool reserve(size_type new_capacity) noexcept
    {
        if (new_capacity > max_size())
        {
            err::set(err::size_error);
            return false;
        }

        return true;
    }

    //=========================================================================
    // resize
    //=========================================================================

    bool resize(size_type new_size, const T c = T())
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (new_size <= size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            mem::destroy_range(end_ptr + 1, shrink_count);
            traits_type::assign(*end_ptr, T());
            size = new_size;
            return true;
        }

        const size_type count = new_size - size;
        return append_n<construct_method::from_char_count>(count, c);
    }

    //=========================================================================
    // push back
    //=========================================================================

    bool push_back(const T c)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size == N)
        {
            err::set(err::size_error);
            return false;
        }

        T* const dst = ptr + size;
        mem::construct_in_place_maybe_trivial(dst);
        traits_type::assign(dst[0], c);
        traits_type::assign(dst[1], T());
        ++size;
        return true;
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    T* erase_n(T* pos, size_type count)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type new_size = size - count;
        const size_type tail_count = size - off - count;

        // Move the tail plus the null terminator
        // old end:  ptr[size] == '\0'
        // new end:  ptr[new_size] must become '\0'
        _priv::move_batch(ptr + off, ptr + off + count, tail_count + 1);

        // Destroy removed objects
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    basic_static_string& erase(size_type off = 0, size_type count = npos)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            erase_n(m_buffer.ptr + off, count);
        }

        return *this;
    }

    iterator erase(const_iterator pos)
    {
        auto ptr = const_cast<T*>(pos.ptr());
        return iterator(erase_n(ptr, 1));
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        auto ptr = const_cast<T*>(first.ptr());
        return iterator(erase_n(ptr, count));
    }

    //=========================================================================
    // pop_back
    //=========================================================================

    void pop_back()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size > 0)
        {
            mem::destroy_in_place(ptr + size);
            --size;
            traits_type::assign(ptr[size], T());
        }
    }

    //=========================================================================
    // copy
    //=========================================================================

    size_type copy(T* dst, size_type count, size_type off = 0) const
    {
        if (!_priv::check_offset(size(), off))
        {
            return 0;
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
        traits_type::copy(dst, m_buffer.ptr + off, count);
        return count;
    }

    basic_static_string substr(size_type off = 0, size_type count = npos) const
    {
        if (!_priv::check_offset(size(), off))
        {
            return basic_static_string();
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
        return basic_static_string(m_buffer.ptr + off, count);
    }

    basic_string_view<T> view(size_type off = 0, size_type count = npos) const
    {
        if (!_priv::check_offset(size(), off))
        {
            return basic_string_view<T>();
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
        return basic_string_view<T>(m_buffer.ptr + off, count);
    }

    //=========================================================================
    // replace
    //=========================================================================

private:

    template <construct_method M, typename... Args>
    bool replace_n(const T* cpos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto pos = const_cast<T*>(cpos);
        const size_type available = N - m_buffer.size;

        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (in_count > out_count)
        {
            const size_type diff = in_count - out_count;
            if (diff > available)
            {
                err::set(vx::err::size_error);
                return false;
            }

            const pointer back = ptr + size + 1;
            mem::construct_range_maybe_trivial(back, diff);

            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _priv::move_batch(pos + in_count, pos + out_count, tail_count);

            size += diff;
        }

        if (in_count < out_count)
        {
            const size_type diff = out_count - in_count;
            const pointer back = ptr + size + 1;
            const size_type tail_count = static_cast<size_type>(back - (pos + out_count));
            _priv::move_batch(pos + in_count, pos + out_count, tail_count);

            mem::destroy_range(back - diff, diff);

            size -= diff;
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char_count)
        {
            traits_type::assign(pos, in_count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            _priv::copy_batch(pos, std::forward<Args>(args)..., in_count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::copy_range(pos, std::forward<Args>(args)...);
        }

        return true;
    }

public:

    basic_static_string& replace(size_type off, size_type count, const basic_static_string& other)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, other.size(), count, other.data());
        }
        return *this;
    }

    basic_static_string& replace(size_type off, size_type count, const basic_static_string& other, size_type other_off, size_type count2 = npos)
    {
        if (_priv::check_offset(size(), off) && _priv::check_offset(other.size(), other_off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            count2 = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), other_off, count2));
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, other.data() + other_off);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(size_type off, size_type count, size_type count2, const T c)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            replace_n<construct_method::from_char_count>(m_buffer.ptr + off, count2, count, c);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(size_type off, size_type count, const T* const s)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            const size_type count2 = static_cast<size_type>(traits_type::length(s));
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, s);
        }
        return *this;
    }

    basic_static_string& replace(size_type off, size_type count, const T* const s, size_type count2)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, s);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(size_type off, size_type count, std::initializer_list<T> init)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            const size_type count2 = static_cast<size_type>(init.size());
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, init.begin());
        }
        return *this;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string& replace(size_type off, size_type count, IT first, IT last)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            const size_type count2 = static_cast<size_type>(std::distance(first, last));
            VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
            {
                replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, first.ptr());
            }
            else
            {
                replace_n<construct_method::from_iterator_range>(m_buffer.ptr + off, count2, count, first, last);
            }
        }
        return *this;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& replace(size_type off, size_type count, const S& t)
    {
        if (_priv::check_offset(size(), off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            const size_type count2 = static_cast<size_type>(t.size());
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, t.data());
        }
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& replace(size_type off, size_type count, const S& t, size_type t_off, size_type count2 = npos)
    {
        if (_priv::check_offset(size(), off) && _priv::check_offset(t.size(), t_off))
        {
            count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
            count2 = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), t_off, count2));
            replace_n<construct_method::from_pointer>(m_buffer.ptr + off, count2, count, t.data() + t_off);
        }
        return *this;
    }

    //=========================================================================
    //=========================================================================

    basic_static_string& replace(const_iterator first, const_iterator last, const basic_static_string& other)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        replace_n<construct_method::from_pointer>(first.ptr(), other.size(), out_count, other.data());
        return *this;
    }

    basic_static_string& replace(const_iterator first, const_iterator last, const basic_static_string& other, size_type other_off, size_type count2 = npos)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        if (_priv::check_offset(other.size(), other_off))
        {
            count2 = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), other_off, count2));
            replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, other.data() + other_off);
        }
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(const_iterator first, const_iterator last, size_type count2, const T c)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        replace_n<construct_method::from_char_count>(first.ptr(), count2, out_count, c);
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(const_iterator first, const_iterator last, const T* const s)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, s);
        return *this;
    }

    basic_static_string& replace(const_iterator first, const_iterator last, const T* const s, size_type count2)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, s);
        return *this;
    }

    //=========================================================================

    basic_static_string& replace(const_iterator first, const_iterator last, std::initializer_list<T> init)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(init.size());
        replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, init.begin());
        return *this;
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_static_string& replace(const_iterator first, const_iterator last, IT first2, IT last2)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(std::distance(first2, last2));

        VX_IF_CONSTEXPR (vx::_priv::is_forward_pointer_iterator<IT>::value)
        {
            replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, first2.ptr());
        }
        else
        {
            replace_n<construct_method::from_iterator_range>(first.ptr(), count2, out_count, first2, last2);
        }
        return *this;
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& replace(const_iterator first, const_iterator last, const S& t)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(t.size());
        replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, t.data());
        return *this;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    basic_static_string& replace(const_iterator first, const_iterator last, const S& t, size_type t_off, size_type count2 = npos)
    {
        const size_type out_count = static_cast<size_type>(std::distance(first, last));
        if (_priv::check_offset(t.size(), t_off))
        {
            count2 = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), t_off, count2));
            replace_n<construct_method::from_pointer>(first.ptr(), count2, out_count, t.data() + t_off);
        }
        return *this;
    }

    //=========================================================================
    // searching
    //=========================================================================

    bool contains(const basic_static_string& other) const noexcept
    {
        return find(other) != npos;
    }

    bool contains(const T c) const noexcept
    {
        return find(c) != npos;
    }

    bool contains(const T* const s) const noexcept
    {
        return find(s) != npos;
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    bool contains(const S& t) const noexcept
    {
        return find(t) != npos;
    }

    //=========================================================================

    size_type find(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find(const S& t, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================

    size_type rfind(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type rfind(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type rfind(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type rfind(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type rfind(const S& t, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================

    size_type find_first_of(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_first_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_first_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_first_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_first_of(const S& t, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================

    size_type find_last_of(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_last_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_last_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_last_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_last_of(const S& t, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================

    size_type find_first_not_of(const basic_static_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_first_not_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_not_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_first_not_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_first_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_first_not_of(const S& t, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================

    size_type find_last_not_of(const basic_static_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_last_not_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_rfind_not_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_last_not_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_last_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    size_type find_last_not_of(const S& t, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, t.data(), t.size()));
    }

    //=========================================================================
    // comparison
    //=========================================================================

    int compare(const basic_static_string& other) const noexcept
    {
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            other.data(), other.size());
    }

    int compare(size_type off, size_type count, const basic_static_string& other) const noexcept
    {
        if (!_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr + off, count,
            other.data(), other.size());
    }

    int compare(size_type off1, size_type count1, const basic_static_string& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_priv::check_offset(size(), off1) || !_priv::check_offset(other.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_priv::clamp_suffix_size(other.size(), off2, count2));
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr + off1, count1,
            other.data() + off2, count2);
    }

    //=========================================================================

    int compare(const T* const s) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            s, s_len);
    }

    int compare(size_type off, size_type count, const T* const s) const noexcept
    {
        if (!_priv::check_offset(size(), off))
        {
            return 1;
        }
        count = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count));
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return compare(off, count, s, s_len);
    }

    int compare(size_type off, size_type count1, const T* const s, size_type count2) const noexcept
    {
        if (!_priv::check_offset(size(), off))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_priv::clamp_suffix_size(size(), off, count1));
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr + off, count1,
            s, count2);
    }

    //=========================================================================

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    int compare(const S& t) const noexcept
    {
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            t.data(), t.size());
    }

    template <typename S, VX_REQUIRES(is_compatible_string<S>::value)>
    int compare(size_type off1, size_type count1, const S& t, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!_priv::check_offset(size(), off1) || !_priv::check_offset(t.size(), off2))
        {
            return 1;
        }
        count1 = static_cast<size_type>(_priv::clamp_suffix_size(size(), off1, count1));
        count2 = static_cast<size_type>(_priv::clamp_suffix_size(t.size(), off2, count2));
        return _priv::traits_compare<traits_type>(
            m_buffer.ptr + off1, count1,
            t.data() + off2, count2);
    }
};

//=========================================================================
// binary + operators
//=========================================================================

template <size_t N, typename T>
basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

//=========================================================================

template <size_t N, typename T>
basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const T rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

template <size_t N, typename T>
basic_static_string<N, T> operator+(const T lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(1, lhs);
    return result.append(rhs);
}

//=========================================================================

template <size_t N, typename T>
basic_static_string<N, T> operator+(const basic_static_string<N, T>& lhs, const T* const rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

template <size_t N, typename T>
basic_static_string<N, T> operator+(const T* const lhs, const basic_static_string<N, T>& rhs)
{
    basic_static_string<N, T> result(lhs);
    return result.append(rhs);
}

//=========================================================================
// comparison operators
//=========================================================================

template <size_t N, typename T>
bool operator==(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <size_t N, typename T>
bool operator==(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <size_t N, typename T>
bool operator==(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) == 0;
}

template <size_t N, typename T>
bool operator!=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <size_t N, typename T>
bool operator!=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <size_t N, typename T>
bool operator!=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) != 0;
}

template <size_t N, typename T>
bool operator<(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <size_t N, typename T>
bool operator<(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <size_t N, typename T>
bool operator<(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) > 0;
}

template <size_t N, typename T>
bool operator>(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <size_t N, typename T>
bool operator>(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <size_t N, typename T>
bool operator>(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) < 0;
}

template <size_t N, typename T>
bool operator<=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <size_t N, typename T>
bool operator<=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <size_t N, typename T>
bool operator<=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) >= 0;
}

template <size_t N, typename T>
bool operator>=(const basic_static_string<N, T>& lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <size_t N, typename T>
bool operator>=(const basic_static_string<N, T>& lhs, const T* const rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <size_t N, typename T>
bool operator>=(const T* const lhs, const basic_static_string<N, T>& rhs) noexcept
{
    return rhs.compare(lhs) <= 0;
}

//=========================================================================
// stream operators
//=========================================================================

template <size_t N, typename T, typename Traits2>
std::basic_istream<T, Traits2>& operator>>(
    std::basic_istream<T, Traits2>& iss,
    basic_static_string<N, T>& s)
{
    std::string is;
    iss >> is;
    s = std::move(is);
    return iss;
}

template <size_t N, typename T, typename Traits2>
std::basic_ostream<T, Traits2>& operator<<(
    std::basic_ostream<T, Traits2>& oss,
    const basic_static_string<N, T>& s)
{
    std::string os(s.data(), s.size());
    oss << os;
    return oss;
}

} // namespace str

//=========================================================================

template <size_t N>
using static_string = str::basic_static_string<N, char>;

template <size_t N>
using static_wstring = str::basic_static_string<N, wchar_t>;

#if defined(__cpp_lib_char8_t)
template <size_t N>
using static_u8string = str::basic_static_string<N, char8_t>;
#endif // defined(__cpp_lib_char8_t)

template <size_t N>
using static_u16string = str::basic_static_string<N, char16_t>;

template <size_t N>
using static_u32string = str::basic_static_string<N, char32_t>;

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <typename T>
struct hash;

template <size_t N, typename T>
struct hash<str::basic_static_string<N, T>>
{
    size_t operator()(const vx::str::basic_static_string<N, T>& s) const noexcept
    {
        using traits = typename vx::str::basic_static_string<N, T>::traits_type;
        return traits::hash(s.data(), s.size());
    }
};

} // namespace vx

namespace std {

template <size_t N, typename T>
struct hash<vx::str::basic_static_string<N, T>>
{
    size_t operator()(const vx::str::basic_static_string<N, T>& s) const noexcept
    {
        return vx::hash<vx::str::basic_static_string<N, T>>{}(s);
    }
};

} // namespace std
