#pragma once

#include <string>

#include "vertex/std/char_traits.hpp"
#include "vertex/std/string_view.hpp"

namespace vx {

template <typename T, typename Allocator = mem::default_allocator<T>>
class basic_string
{
private:

    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");

    using traits_type = str::char_traits<T>;

    using array_type = _priv::dynamic_array_base<T, Allocator>;
    array_type m_array;

public:

    //=========================================================================
    // member types
    //=========================================================================

    template <intmax_t N, intmax_t D = 1>
    using growth_rate_type = std::ratio<N, D>;
    using default_growth_rate = growth_rate_type<3, 2>;

    using value_type = T;
    using allocator_type = Allocator;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = _priv::pointer_iterator<T*, basic_string>;
    using const_iterator = _priv::pointer_iterator<const T*, basic_string>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

private:

    //=========================================================================
    // bounds checkers
    //=========================================================================

    constexpr size_type clamp_suffix_size(const size_type off, const size_type size) const noexcept
    {
        return std::min(size, m_size - off);
    }

    bool check_offset(const size_type off) const
    {
        return (off <= m_size);
    }

    //=========================================================================
    // constructors
    //=========================================================================

    enum class construct_method
    {
        from_char,
        from_pointer,
        from_string,
        from_iterator_range
    };

    bool construct_empty()
    {
        auto new_ptr = allocator_type::allocate(1);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        m_array.m_buffer.ptr = new_ptr;
        m_array.m_buffer.size = 1;
        m_array.m_buffer.capacity = 1;

        mem::construct_in_place(m_array.data());
        traits_type::assign(m_array.data(), T());
    }

    template <construct_method M, typename... Args>
    void construct_n(Args&&... args, size_type count)
    {
        const size_type alloc_count = count + 1;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(alloc_count > m_array.max_size(),
            {
                err::set(err::size_error);
                return;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        auto new_ptr = allocator_type::allocate(alloc_count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        m_array.m_buffer.ptr = new_ptr;
        m_array.m_buffer.size = alloc_count;
        m_array.m_buffer.capacity = alloc_count;

        mem::construct_range(m_array.data(), alloc_count);

        VX_IF_CONSTEXPR(M == construct_method::from_char)
        {
            traits_type::assign(m_array.data(), count, std::forward<Args>(args)...);
            type_traits::assign(m_array[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            traits_copy_batch(m_array.data(), std::forward<Args>(args)..., count);
            type_traits::assign(m_array[count], T());
        }
        else VX_IF_CONSTEXPR(M == construct_method::from_string)
        {
            traits_type::copy(m_array.data(), std::forward<Args>(args)..., alloc_count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");

            T* dst = m_array.data();
            for (; first != last; ++first)
            {
                traits_type::assign(*dst, *first);
                ++dst;
            }

            traits_type::assign(*dst, T());
        }
    }

public:

    basic_string()
    {
        construct_empty();
    }

    ~basic_string() = default;
    basic_string(basic_string&&) noexcept = default;

    basic_string(const basic_string& other)
    {
        construct_n<construct_method::from_string>(other.data(), other.size());
    }

    basic_string(size_type count, const T& value)
    {
        construct_n<construct_method::from_char>(value, count);
    }

    basic_string(std::initializer_list<T> init)
    {
        construct_n<construct_method::from_pointer>(init.begin(), init.size());
    }

    basic_strint(const basic_string& other, size_t off)
    {
        if (other.check_offset(off))
        {
            construct_n<construct_method::from_pointer>(other.data() + off, other.size() - off);
        }
        else
        {
            construct_empty();
        }
    }

    basic_string(const basic_string& other, size_t off, size_t count)
    {
        if (other.check_offset(off))
        {
            const size_type suffix_size = other.clamp_suffix_size(off, count);
            construct_n<construct_method::from_pointer>(other.data() + off, suffix_size);
        }
        else
        {
            construct_empty();
        }
    }

    basic_string(const T* const ptr, size_type count)
    {
        construct_n<construct_method::from_pointer>(ptr, count);
    }

    basic_string(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length());
        construct_n<construct_method::from_pointer>(ptr, count);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            construct_n<construct_method::from_pointer>(first.ptr(), count);
        }
        else
        {
            construct_n<construct_method::from_iterator_range>(first, last, count);
        }
    }

    basic_string(const basic_string_view<T> view)
    {
        construct_n<construct_method::from_pointer>(view.data(), view.size());
    }

    bool is_valid() const noexcept
    {
        return m_array.size() > 0;
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool assign_from(const size_type count, Args&&... args)
    {
        auto& ptr = m_array.m_buffer.ptr;
        auto& size = m_array.m_buffer.size;
        auto& capacity = m_array.m_buffer.capacity;

        const size_type alloc_count = count + 1;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(alloc_count > m_array.max_size(),
            {
                err::set(err::size_error);
                return;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        if (alloc_count > capacity)
        {
            pointer new_ptr = allocator_type::allocate(alloc_count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            VX_UNLIKELY_COLD_PATH(!new_ptr,
                {
                    return false;
                });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            mem::destroy_range(ptr, size);
            allocator_type::deallocate(ptr, capacity);
            ptr = new_ptr;
            capacity = alloc_count;
        }
        else
        {
            mem::destroy_range(ptr, size);
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            traits_type::assign(ptr, std::forward<Args>(args)..., count);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_string)
        {
            traits_type::copy(ptr, std::forward<Args>(args)..., alloc_count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

            T* dst = ptr;
            for (; first != last; ++first)
            {
                traits_type::assign(*dst, *first);
                ++dst;
            }

            traits_type::assign(*dst, T());
        }

        size = alloc_count;
        return true;
    }

public:

    //=========================================================================
    // assignment operators
    //=========================================================================

    basic_string& operator=(basic_string&&) noexcept = default;

    basic_string& operator=(const basic_string&)
    {
        assign_from<construct_method::from_string>(other.data(), other.size());
        return *this;
    }

    basic_string& operator=(std::initializer_list<T> init)
    {
        assign_from<construct_method::from_pointer>(init.begin(), init.size());
    }

    basic_string& operator=(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        assign_from<construct_method::from_pointer>(ptr, count);
        return *this;
    }

    basic_string& operator=(const T c)
    {
        assign_from<construct_method::from_char>(c, 1);
        return *this;
    }

    basic_string& operator=(const basic_string_view<T> view)
    {
        assign_from<construct_method::from_pointer>(view.data(), view.size());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    basic_string& assign(const basic_string& other)
    {
        return operator=(other);
    }

    basic_string& assign(basic_string&& other) noexcept
    {
        return operator=(other);
    }

    basic_string& assign(std::initializer_list<T> init)
    {
        return operator=(init);
    }

    basic_string& assign(const T* const ptr)
    {
        return operator=(ptr);
    }

    basic_string& assign(const T* const ptr, size_type count)
    {
        assign_from<construct_method::from_pointer>(ptr, count);
        return *this;
    }

    basic_string& assign(const T c)
    {
        return operator=(c);
    }

    basic_string& assign(const basic_string& other, size_type off, size_type count = npos)
    {
        if (other.check_offset(off))
        {
            count = other.clamp_suffix_size(off, count);
            assign_from<construct_method::from_pointer>(other.data() + off, count);
        }

        return *this;
    }

    basic_string& assign(const basic_string_view<T> view)
    {
        return operator=(view);
    }

    basic_string& assign(const basic_string_view<T> view, size_type off, size_type count = npos)
    {
        if (view.check_offset(off))
        {
            count = view.clamp_suffix_size(off, count);
            assign_from<construct_method::from_pointer>(view.data() + off, count);
        }

        return *this;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& assign(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            assign_from<construct_method::from_pointer>(first.ptr(), count);
        }
        else
        {
            assign_from<construct_method::from_iterator_range>(first, last, count);
        }
    }

    //=========================================================================
    // operators
    //=========================================================================

    operator basic_string_view<T>() const noexcept
    {
        return basic_string_view<T>(data(), size());
    }

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        VX_ASSERT(m_array.size() > 0);
        return m_array[0];
    }

    const T& front() const noexcept
    {
        VX_ASSERT(m_array.size() > 0);
        return m_array[0];
    }

    T& back() noexcept
    {
        VX_ASSERT(m_array.size() > 0);
        return m_array[m_array.size() - 1];
    }

    const T& back() const noexcept
    {
        VX_ASSERT(m_array.size() > 0);
        return m_array[m_array.size() - 1];
    }

    T* data() noexcept
    {
        return m_array.data();
    }

    const T* data() const noexcept
    {
        return m_array.data();
    }

    T& operator[](size_type i) noexcept
    {
        VX_ASSERT(i < m_array.size());
        return m_array[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_array.size());
        return m_array[i];
    }

    const T* c_str() const noexcept
    {
        return m_array.data();
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_array.data());
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_array.data());
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_array.data() + m_array.size() - 1);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_array.data() + m_array.size() - 1);
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

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        m_array.clear();
        m_array.emplace_back(T());
    }

    void clear_and_deallocate()
    {
        m_array.clear_and_deallocate();
        m_array.emplace_back(T());
    }

    bool shrink_to_fit()
    {
        return m_array.shrink_to_fit();
    }

    T* release() noexcept
    {
        return m_array.release();
    }

    bool acquire(T* ptr, size_type count, bool add_null = false)
    {
        count = add_null ? count + 1 : count;
        if (!m_array.acquire(ptr, count))
        {
            return false;
        }

        if (add_null)
        {
            traits_type::assign(m_array[count - 1], T());
        }

        return true;
    }

    void swap(basic_string& other) noexcept
    {
        m_array.swap(other.m_array);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_array.size() <= 1;
    }

    size_type size() const noexcept
    {
        return m_array.size() - 1;
    }

    size_type length() const noexcept
    {
        return size();
    }

    size_type data_size() const noexcept
    {
        return m_array.data_size();
    }

    size_type max_size() const noexcept
    {
        return m_array.max_size() - 1;
    }

    //=========================================================================
    // capacity
    //=========================================================================

    size_type capacity() const noexcept
    {
        return m_array.capacity() - 1;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    bool reserve(size_type new_capacity)
    {
        return m_array.reserve(new_capacity + 1);
    }

    //=========================================================================
    // resize
    //=========================================================================

    bool resize(size_type new_size, const T& value = T())
    {
        if (m_array.resize(new_size + 1, value))
        {
            traits_type::assign(m_array[new_size], T());
            return true;
        }

        return false;
    }

    //=========================================================================
    // insert
    //=========================================================================

    //=========================================================================
    // push back
    //=========================================================================

    bool push_back(const T& value)
    {
        return m_array.push_back(value);
    }

    //=========================================================================
    // erase
    //=========================================================================

    basic_string& erase(size_type off = 0, size_type count = npos)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            m_array.erase_n(m_array.data() + off, count);
        }

        return *this;
    }

    iterator erase(const_iterator pos)
    {
        return m_array.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return m_array.erase(first, last);
    }

    void pop_back()
    {
        m_array.pop_back();
        traits_type::assign(m_array[m_array.size() - 1], T());
    }

    //=========================================================================
    // comparison
    //=========================================================================

    //=========================================================================
    // append
    //=========================================================================

private:

    bool append_impl(const T* const s, size_type count)
    {
        const size_type old_size = size();
        const size_type new_size = old_size + count;
        if (reserve(new_size))
        {
            str::_priv::traits_copy_batch(m_array.data() + old_size, s, count);
            return true;
        }
        return false;
    }

public:

    basic_string& append(size_type count, T c)
    {
        const size_type new_size = size() + count;
        resize(new_size, c);
        return *this;
    }

    basic_string& append(const T* const s, size_type count)
    {
        append_impl(s, count);
        return *this;
    }

    basic_string& append(const T* const s)
    {
        return append(s, static_cast<size_type>(traits_type::length(s)));
    }

    basic_string& append(const basic_string_view<T> view)
    {
        return append(view.data(), static_cast<size_type>(view.size()));
    }

    basic_string& append(const basic_string_view<T> view, size_type off, size_type count = npos)
    {
        if (!view.check_offset(off))
        {
            return *this;
        }
        count = static_cast<size_type>(view.clamp_suffix_size(off, count));
        return append(view.data() + off, count);
    }

    basic_string& append(const basic_string& other)
    {
        return append(other.data(), other.size());
    }

    basic_string& append(const basic_string& other, size_type off, size_type count = npos)
    {
        if (!other.check_offset(off))
        {
            return *this;
        }
        count = other.clamp_suffix_size(off, count);
        return append(other.data() + off, count);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& append(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        const size_type old_size = size();
        const size_type new_size = old_size + count;
        if (reserve(new_size))
        {
            str::_priv::traits_copy_batch(m_array.data() + old_size, s, count);
            return true;
        }
        return false;
    }

    //=========================================================================
    // replace
    //=========================================================================

    //=========================================================================
    // copy
    //=========================================================================
};

} // namespace vx
