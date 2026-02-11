#pragma once

#include <ratio>
#include <string>

#include "vertex/std/char_traits.hpp"
#include "vertex/std/string_view.hpp"

namespace vx {

template <typename T, typename Allocator = mem::default_allocator<T>>
class basic_string
{
private:

    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = str::char_traits<T>;

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

    using iterator = _priv::pointer_iterator<basic_string, T>;
    using const_iterator = _priv::pointer_iterator<basic_string, const T>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

private:

    struct buffer_type
    {
        pointer ptr;
        size_type size;
        size_type capacity;
    };

    buffer_type m_buffer = {};

private:

    //=========================================================================
    // bounds checkers
    //=========================================================================

    constexpr size_type clamp_suffix_size(const size_type off, const size_type count) const noexcept
    {
        return traits_type::clamp_suffix_size(m_buffer.size, off, count);
    }

    constexpr bool check_offset(const size_type off) const noexcept
    {
        return traits_type::check_offset(m_buffer.size, off);
    }

private:

    //=========================================================================
    // move/copy helpers
    //=========================================================================

    template <typename U>
    static void copy_batch(T* const first1, const U* const first2, const size_t count) noexcept
    {
        VX_IF_CONSTEXPR (std::is_volatile<U>::value)
        {
            for (size_t i = 0; i != count; ++i)
            {
                traits_type::assign(first1[i], T{ first2[i] });
            }
        }
        else
        {
            traits_type::copy(first1, first2, count);
        }
    }

    template <typename U>
    static void move_batch(T* const first1, const U* const first2, const size_t count) noexcept
    {
        VX_IF_CONSTEXPR (std::is_volatile<U>::value)
        {
            bool loop_forward = true;

            for (const U* src = first2; src != first2 + count; ++src)
            {
                if (first1 == src)
                {
                    loop_forward = false;
                    break;
                }
            }

            if (loop_forward)
            {
                for (size_t i = 0; i != count; ++i)
                {
                    traits_type::assign(first1[i], T{ first2[i] });
                }
            }
            else
            {
                for (size_t i = count; i != 0; --i)
                {
                    traits_type::assign(first1[i - 1], T{ first2[i - 1] });
                }
            }
        }
        else
        {
            traits_type::move(first1, first2, count);
        }
    }

    //=========================================================================
    // allocation helpers
    //=========================================================================

    template <typename growth_rate>
    size_type grow_capacity(size_type required_capacity, size_type current_capacity) const noexcept
    {
        constexpr size_type max_capacity = max_size();

        VX_IF_CONSTEXPR (growth_rate::num == 1 && growth_rate::den == 1)
        {
            if (required_capacity > max_capacity)
            {
                return max_capacity;
            }

            return required_capacity;
        }
        else
        {
            current_capacity = current_capacity ? current_capacity : 1;

            size_type new_capacity;

            VX_IF_CONSTEXPR (growth_rate::num == 3 && growth_rate::den == 2)
            {
                // Guard against multiplication overflow: old_capacity * num
                if (current_capacity > max_capacity - current_capacity / 2)
                {
                    return max_capacity;
                }

                new_capacity = current_capacity + current_capacity / 2;
            }
            else
            {
                // Guard against multiplication overflow: old_capacity * num
                if (current_capacity > max_capacity / growth_rate::num)
                {
                    return max_capacity;
                }

                new_capacity = current_capacity * growth_rate::num / growth_rate::den;
            }

            if (new_capacity < required_capacity)
            {
                new_capacity = required_capacity;
            }

            return new_capacity;
        }
    }

    template <typename growth_rate, typename Fn, typename... Args>
    basic_string& reallocate_grow_by(const size_type size_increase, Fn fn, Args&&... args)
    {
        auto& b = m_buffer;
        const size_type old_size = b.size;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        if (max_size() - size_increase < old_size)
        {
            err::set(err::size_error);
            return *this;
        }

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        const size_type new_size = old_size + size_increase;
        const size_type old_capacity = b.capacity;
        const size_type new_capacity = grow_capacity<growth_rate>(new_size, old_capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return *this;
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        pointer old_ptr = b.ptr;
        fn(new_ptr, old_ptr, old_size, std::forward<Args>(args)...);
        destroy_and_deallocate(old_ptr, old_size, old_capacity);

        b.ptr = new_ptr;
        b.size = new_size;
        b.capacity = new_capacity;

        return *this;
    }

    static void destroy_size(T* ptr, size_type size)
    {
        mem::destroy_range(ptr, size + 1);
    }

    static void deallocate_capacity(T* ptr, size_type capacity)
    {
        allocator_type::deallocate(ptr, capacity + 1);
    }

    static void destroy_and_deallocate(T* ptr, size_type size, size_type capacity)
    {
        destroy_size(ptr, size + 1);
        deallocate_capacity(ptr, capacity + 1);
    }

    void destroy_range()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (ptr)
        {
            destroy_and_deallocate(ptr, size, capacity);
        }

        ptr = nullptr;
        size = 0;
        capacity = 0;
    }

    //=========================================================================
    // constructor helpers
    //=========================================================================

    enum class construct_method
    {
        from_char,
        from_pointer,
        from_string,
        from_iterator_range
    };

    buffer_type release_buffer() noexcept
    {
        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        buffer_type old_buffer = std::move(b);

        ptr = nullptr;
        size = 0;
        capacity = 0;

        return old_buffer;
    }

    bool construct_empty()
    {
        auto new_ptr = allocator_type::allocate(1);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        m_buffer.ptr = new_ptr;
        m_buffer.size = 0;
        m_buffer.capacity = 0;

        mem::construct_in_place(m_buffer.ptr);
        traits_type::assign(*m_buffer.ptr, T());

        return true;
    }

    template <construct_method M, typename... Args>
    void construct_n(size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                err::set(err::size_error);
                return;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        const size_type alloc_count = count + 1;
        auto new_ptr = allocator_type::allocate(alloc_count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        ptr = new_ptr;
        size = count;
        capacity = count;

        mem::construct_range(ptr, alloc_count);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(ptr, count, std::forward<Args>(args)...);
            traits_type::assign(ptr[count], T());
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(ptr, std::forward<Args>(args)..., count);
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

    basic_string()
    {
        construct_empty();
    }

    basic_string(const basic_string& other)
    {
        construct_n<construct_method::from_string>(other.size(), other.data());
    }

    basic_string(basic_string&& other) noexcept
        : m_buffer(other.release_buffer())
    {}

    //=========================================================================

    basic_string(const basic_string& other, size_t off)
    {
        if (other.check_offset(off))
        {
            construct_n<construct_method::from_pointer>(other.size() - off, other.data() + off);
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
            count = other.clamp_suffix_size(off, count);
            construct_n<construct_method::from_pointer>(count, other.data() + off);
        }
        else
        {
            construct_empty();
        }
    }

    //=========================================================================

    basic_string(size_type count, const T value)
    {
        construct_n<construct_method::from_char>(count, value);
    }

    //=========================================================================

    basic_string(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        construct_n<construct_method::from_pointer>(count, ptr);
    }

    basic_string(const T* const ptr, size_type count)
    {
        construct_n<construct_method::from_pointer>(count, ptr);
    }

    //=========================================================================

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            construct_n<construct_method::from_pointer>(count, first.ptr());
        }
        else
        {
            construct_n<construct_method::from_iterator_range>(count, first, last);
        }
    }

    //=========================================================================

    basic_string(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        construct_n<construct_method::from_pointer>(count, init.begin());
    }

    //=========================================================================

    basic_string(const basic_string_view<T> view)
    {
        const size_type count = static_cast<size_type>(view.size());
        construct_n<construct_method::from_pointer>(count, view.data());
    }

    bool is_valid() const noexcept
    {
        return m_buffer.ptr != nullptr;
    }

public:

    //=========================================================================
    // destructor
    //=========================================================================

    ~basic_string()
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

    operator std::basic_string<T>() const
    {
        return std::basic_string<T>(data(), size());
    }

private:

    //=========================================================================
    // assignment helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool assign_from(const size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                err::set(err::size_error);
                return;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        if (count > capacity)
        {
            const size_type alloc_count = count + 1;
            pointer new_ptr = allocator_type::allocate(alloc_count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            VX_UNLIKELY_COLD_PATH(!new_ptr,
                {
                    return false;
                });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            destroy_and_deallocate(ptr, size, capacity);

            ptr = new_ptr;
            capacity = alloc_count;
        }
        else
        {
            destroy_size(ptr, size);
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
            traits_type::copy(ptr, std::forward<Args>(args)..., count + 1);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");

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

    basic_string& operator=(const basic_string& other)
    {
        assign_from<construct_method::from_string>(other.size(), other.data());
        return *this;
    }

    basic_string& operator=(basic_string&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            destroy_range();
            m_buffer = other.release_buffer();
        }
        return *this;
    }

    basic_string& operator=(const T c)
    {
        assign_from<construct_method::from_char>(1, c);
        return *this;
    }

    basic_string& operator=(const T* const ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));
        assign_from<construct_method::from_pointer>(count, ptr);
        return *this;
    }

    basic_string& operator=(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        assign_from<construct_method::from_pointer>(count, init.begin());
    }

    basic_string& operator=(const basic_string_view<T> view)
    {
        const size_type count = static_cast<size_type>(view.size());
        assign_from<construct_method::from_pointer>(count, view.data());
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

    //=========================================================================

    basic_string& assign(const basic_string& other, size_type off, size_type count = npos)
    {
        if (other.check_offset(off))
        {
            count = other.clamp_suffix_size(off, count);
            assign_from<construct_method::from_pointer>(count, other.data() + off);
        }

        return *this;
    }

    //=========================================================================

    basic_string& assign(const T c)
    {
        return operator=(c);
    }

    //=========================================================================

    basic_string& assign(const T* const ptr)
    {
        return operator=(ptr);
    }

    basic_string& assign(const T* const ptr, size_type count)
    {
        assign_from<construct_method::from_pointer>(count, ptr);
        return *this;
    }

    //=========================================================================

    basic_string& assign(std::initializer_list<T> init)
    {
        return operator=(init);
    }

    //=========================================================================

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

    basic_string& assign(const basic_string_view<T> view)
    {
        return operator=(view);
    }

    basic_string& assign(const basic_string_view<T> view, size_type off, size_type count = npos)
    {
        if (view.check_offset(off))
        {
            count = static_cast<size_type>(view.clamp_suffix_size(off, count));
            assign_from<construct_method::from_pointer>(count, view.data() + off);
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
        return m_buffer.ptr[m_buffer.size];
    }

    const T& back() const noexcept
    {
        VX_ASSERT(m_buffer.size > 0);
        return m_buffer.ptr[m_buffer.size];
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
    // reallocate
    //=========================================================================

    bool reallocate_shrink(size_type new_capacity)
    {
        const size_type bytes = m_buffer.size * sizeof(T);
        constexpr size_type reallocate_threshold = 96000;

        if (bytes < reallocate_threshold)
        {
            return reallocate<true, false>(new_capacity);
        }
        else
        {
            return reallocate<true, true>(new_capacity);
        }
    }

    template <bool shrinking = false, bool try_reallocate = false>
    bool reallocate(size_type new_capacity)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        const size_type alloc_capacity = new_capacity + 1;
        pointer new_ptr;

        VX_IF_CONSTEXPR (try_reallocate && std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
        {
            new_ptr = allocator_type::reallocate(ptr, alloc_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return false;
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)
        }
        else
        {
            new_ptr = allocator_type::allocate(alloc_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return false;
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            VX_IF_CONSTEXPR (shrinking)
            {
                VX_ASSERT(size > 0);
                mem::construct_range(new_ptr, ptr, alloc_capacity);
                traits_type::copy(new_ptr, ptr, alloc_capacity);
            }
            else
            {
                mem::construct_range(new_ptr, ptr, size);
                traits_type::copy(new_ptr, ptr, size);
            }

            destroy_and_deallocate(ptr, size, capacity);
        }

        ptr = new_ptr;
        VX_IF_CONSTEXPR (shrinking)
        {
            size = new_capacity;
        }
        capacity = new_capacity;

        return true;
    }

private:

    //=========================================================================
    // append helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    bool append_capacity(size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        T* const dst = ptr + size;

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(dst, std::forward<Args>(args)...);
        }

        size += count;
        traits_type::assign(ptr[size], T());

        return true;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    bool append_reallocate(size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size() - size,
            {
                err::set(err::size_error);
                return false;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        const size_type new_size = size + count;
        const size_type new_capacity = grow_capacity<growth_rate>(new_size, capacity);
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity + 1);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        // copy prefix [ptr, ptr + size) to [new_ptr, ...), then construct suffix [ptr + size, ...)
        traits_type::copy(new_ptr, ptr, size);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(ptr + size, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(ptr + size, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(ptr + size, std::forward<Args>(args)...);
        }

        traits_type::assign(new_ptr[new_size], T());

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return true;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    bool append_n(const size_type count, Args&&... args)
    {
        VX_STATIC_ASSERT_MSG(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT_MSG(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        const size_type available = m_buffer.capacity - m_buffer.size;

        if (count <= available)
        {
            return append_capacity<M>(count, std::forward<Args>(args)...);
        }
        else
        {
            return append_reallocate<growth_rate, M>(count, std::forward<Args>(args)...);
        }
    }

public:

    //=========================================================================
    // append
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    basic_string& append(size_type count, T c)
    {
        append_n<growth_rate, construct_method::from_char>(count, c);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const T* const s, const size_type count)
    {
        append_n<growth_rate, construct_method::from_pointer>(count, s);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        append_n<growth_rate, construct_method::from_pointer>(count, s);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const basic_string_view<T> view)
    {
        const size_type count = static_cast<size_type>(view.size());
        append_n<growth_rate, construct_method::from_pointer>(count, view.data());
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const basic_string_view<T> view, size_type off, size_type count = npos)
    {
        if (view.check_offset(off))
        {
            count = static_cast<size_type>(view.clamp_suffix_size(off, count));
            append_n<growth_rate, construct_method::from_pointer>(count, view.data() + off);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const basic_string& other)
    {
        append_n<growth_rate, construct_method::from_pointer>(other.size(), other.data());
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(const basic_string& other, size_type off, size_type count = npos)
    {
        if (other.check_offset(off))
        {
            count = static_cast<size_type>(other.clamp_suffix_size(off, count));
            append_n<growth_rate, construct_method::from_pointer>(count, other.data() + off);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& append(IT first, IT last)
    {
        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            append_n<growth_rate, construct_method::from_pointer>(count, first.ptr());
        }
        else
        {
            const size_type count = static_cast<size_type>(std::distance(first, last));
            append_n<growth_rate, construct_method::from_iterator_range>(count, first, last);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& append(std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        append_n<growth_rate, construct_method::from_pointer>(count, init.begin());
        return *this;
    }

    //=========================================================================

    basic_string& operator+=(const basic_string& other)
    {
        return append(other);
    }

    basic_string& operator+=(const T* const s)
    {
        return append(s);
    }

    basic_string& operator+=(const T c)
    {
        push_back(c);
        return *this;
    }

    basic_string& operator+=(const basic_string_view<T> view)
    {
        return append(view);
    }

    basic_string& operator+=(std::initializer_list<T> init)
    {
        return append(init);
    }

    //=========================================================================

    friend basic_string operator+(const basic_string& lhs, const basic_string& rhs)
    {
        basic_string result(lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(const basic_string& lhs, const T* const rhs)
    {
        basic_string result(lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(const T* const lhs, const basic_string& rhs)
    {
        basic_string result(lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(const basic_string& lhs, const T rhs)
    {
        basic_string result(lhs);
        result.push_back(rhs);
        return result;
    }

    friend basic_string operator+(const T lhs, const basic_string& rhs)
    {
        basic_string result(1, lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(const basic_string& lhs, const basic_string_view<T> rhs)
    {
        basic_string result(lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(const basic_string_view<T> lhs, const basic_string& rhs)
    {
        basic_string result(lhs);
        return result.append(rhs);
    }

    friend basic_string operator+(basic_string&& lhs, const basic_string& rhs)
    {
        return std::move(lhs).append(rhs);
    }

    friend basic_string operator+(basic_string&& lhs, const T* const rhs)
    {
        return std::move(lhs).append(rhs);
    }

    friend basic_string operator+(basic_string&& lhs, const T rhs)
    {
        std::move(lhs).push_back(rhs);
        return std::move(lhs);
    }

    friend basic_string operator+(basic_string&& lhs, const basic_string_view<T> rhs)
    {
        return std::move(lhs).append(rhs);
    }

    friend basic_string operator+(const basic_string& lhs, basic_string&& rhs)
    {
        return std::move(rhs).append(lhs);
    }

    friend basic_string operator+(const T* const lhs, basic_string&& rhs)
    {
        return std::move(rhs).append(lhs);
    }

    friend basic_string operator+(const T lhs, basic_string&& rhs)
    {
        std::move(rhs).push_back(lhs);
        return std::move(rhs);
    }

    friend basic_string operator+(const basic_string&& lhs, basic_string&& rhs)
    {
        return std::move(lhs).append(rhs);
    }

private:

    //=========================================================================
    // insert helpers
    //=========================================================================

    template <construct_method M, typename... Args>
    pointer insert_capacity(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        const pointer back = ptr + size;

        // initialize the new elements that will be moved into uninitialized memory
        mem::construct_range(back + 1, count);
        // move the tail backward to make room for the new elements
        move_batch(back, pos, count + 1);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(pos, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(pos, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(pos, std::forward<Args>(args)...);
        }

        size += count;
        return pos;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    basic_string& insert_reallocate(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size() - size,
            {
                err::set(err::size_error);
                return nullptr;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        const size_type new_size = size + count;
        const size_type new_capacity = grow_capacity<growth_rate>(new_size, capacity);
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity + 1);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return nullptr;
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        const size_type off = static_cast<size_type>(pos - ptr);
        pointer dst = new_ptr + off;

        // copy prefix [ptr, ptr + size) to [new_ptr, ...), then construct suffix [ptr + size, ...)
        mem::construct_range(new_ptr, new_ptr + new_size + 1);
        traits_type::copy(new_ptr, ptr, size);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(ptr + size, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(ptr + size, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(ptr + size, std::forward<Args>(args)...);
        }

        // copy second range
        traits_type::copy(dst + count, pos, size - off);

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return dst;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    pointer insert_n(pointer pos, const size_type count, Args&&... args)
    {
        VX_STATIC_ASSERT_MSG(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT_MSG(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        const size_type available = m_buffer.capacity - m_buffer.size;

        if (count <= available)
        {
            return insert_capacity<M>(pos, count, std::forward<Args>(args)...);
        }
        else
        {
            return insert_reallocate<growth_rate, M>(pos, count, std::forward<Args>(args)...);
        }
    }

public:

    //=========================================================================
    // insert
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, size_type count, const T c)
    {
        insert_n<growth_rate, construct_method::from_char>(m_buffer.ptr + off, count, c);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const T* const s, size_type count)
    {
        insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, s);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, s);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const basic_string_view<T> view)
    {
        const size_type count = static_cast<size_type>(view.size());
        insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, view.data());
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const basic_string_view<T> view, size_type view_off, size_type count = npos)
    {
        if (!view.check_offset(view_off))
        {
            return *this;
        }
        count = static_cast<size_type>(view.clamp_suffix_size(view_off, count));
        insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, view.data() + view_off);
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const basic_string& other)
    {
        return insert(off, other.data(), other.size());
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, const basic_string& other, size_type other_off, size_type count = npos)
    {
        if (!other.check_offset(other_off))
        {
            return *this;
        }
        count = static_cast<size_type>(other.clamp_suffix_size(other_off, count));
        return insert(off, other.data() + other_off, count);
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& insert(size_type off, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            return insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, first.ptr());
        }
        else
        {
            return insert_n<growth_rate, construct_method::from_iterator_range>(m_buffer.ptr + off, count, first, last);
        }
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& insert(size_type off, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        return insert_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count, init.begin());
    }

    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T c)
    {
        pointer new_pos = insert_n<growth_rate, construct_method::from_char>(pos.ptr(), 1, c);
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T* const s, size_type count)
    {
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, s);
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T* const s)
    {
        const size_type count = static_cast<size_type>(traits_type::length(s));
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, s);
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const basic_string_view<T> view)
    {
        const size_type count = static_cast<size_type>(view.size());
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, view.data());
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const basic_string_view<T> view, size_type view_off, size_type count = npos)
    {
        if (!view.check_offset(view_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(view.clamp_suffix_size(view_off, count));
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, view.data() + view_off);
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const basic_string& other)
    {
        return insert(pos, other.data(), other.size());
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const basic_string& other, size_type other_off, size_type count = npos)
    {
        if (!other.check_offset(other_off))
        {
            return iterator(pos);
        }
        count = static_cast<size_type>(other.clamp_suffix_size(other_off, count));
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, other.data() + other_off);
        return iterator(new_pos);
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, first.ptr());
            return iterator(new_pos);
        }
        else
        {
            pointer new_pos = insert_n<growth_rate, construct_method::from_iterator_range>(pos.ptr(), count, first, last);
            return iterator(new_pos);
        }
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        const size_type count = static_cast<size_type>(init.size());
        pointer new_pos = insert_n<growth_rate, construct_method::from_pointer>(pos.ptr(), count, init.begin());
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
        destroy_and_deallocate(m_buffer.ptr, m_buffer.size, m_buffer.capacity);
        construct_empty();
    }

    bool shrink_to_fit()
    {
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (size == capacity)
        {
            return true;
        }

        return reallocate_shrink(size);
    }

    T* release() noexcept
    {
        const buffer_type old_buffer = release_buffer();
        return old_buffer.ptr;
    }

    bool acquire(T* ptr)
    {
        const size_type count = static_cast<size_type>(traits_type::length(ptr));

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                err::set(err::size_error);
                return false;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        destroy_range();

        m_buffer.ptr = ptr;
        m_buffer.size = count;
        m_buffer.capacity = count;
        return true;
    }

    void swap(basic_string& other) noexcept
    {
        std::swap(m_buffer.ptr, other.m_buffer.ptr);
        std::swap(m_buffer.size, other.m_buffer.size);
        std::swap(m_buffer.capacity, other.m_buffer.capacity);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_buffer.size > 0;
    }

    size_type size() const noexcept
    {
        return m_buffer.size;
    }

    size_type length() const noexcept
    {
        return size();
    }

    size_type data_size() const noexcept
    {
        return size() * sizeof(T);
    }

    size_type max_size() const noexcept
    {
        return mem::max_array_size<T>() - 1;
    }

    //=========================================================================
    // capacity
    //=========================================================================

    size_type capacity() const noexcept
    {
        return m_buffer.capacity;
    }

    //=========================================================================
    // reserve
    //=========================================================================

    bool reserve(size_type new_capacity)
    {
        if (new_capacity > m_buffer.capacity)
        {
#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

            VX_UNLIKELY_COLD_PATH(new_capacity > max_size(),
                {
                    err::set(err::size_error);
                    return false;
                });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

            return reallocate(new_capacity);
        }

        return true;
    }

    //=========================================================================
    // resize
    //=========================================================================

    bool resize(size_type new_size, const T c = T())
    {
        if (new_size <= m_buffer.size)
        {
            traits_type::assign(m_buffer.ptr + new_size, T());
            m_buffer.size = new_size;
            return true;
        }

        return append_n<growth_rate_type<1, 1>, construct_method::from_char>(new_size - m_buffer.size, c);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    bool push_back(const T c)
    {
        VX_STATIC_ASSERT_MSG(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT_MSG(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (size == capacity)
        {
            const size_type new_capacity = grow_capacity<growth_rate>(size + 1, capacity);
            VX_UNLIKELY_COLD_PATH(!reallocate(new_capacity),
                {
                    return false;
                });
        }

        traits_type::assign(ptr[size], c);
        traits_type::assign(ptr[size + 1], T());

        ++size;
        return true;
    }

    //=========================================================================
    // erase
    //=========================================================================

private:

    pointer erase_n(pointer pos, size_type count)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type new_size = size - count;
        const size_type tail_count = size - off - count;

        // Move the tail plus the null terminator
        // old end:  ptr[size] == '\0'
        // new end:  ptr[new_size] must become '\0'
        move_batch(ptr + off, ptr + off + count, tail_count + 1);

        // Destroy removed objects
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        return pos;
    }

public:

    basic_string& erase(size_type off = 0, size_type count = npos)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            erase_n(m_buffer.ptr + off, count);
        }

        return *this;
    }

    iterator erase(const_iterator pos)
    {
        return iterator(erase_n(pos.ptr(), 1));
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));
        return iterator(erase_n(first.ptr(), count));
    }

    void pop_back()
    {
        if (m_buffer.size > 0)
        {
            --m_buffer.size;
            traits_type::assign(m_buffer.ptr[m_buffer.size], T());
        }
    }

    //=========================================================================
    // copy
    //=========================================================================

    size_type copy(T* dest, size_type count, size_type off = 0) const
    {
        if (!check_offset(off))
        {
            return 0;
        }
        count = clamp_suffix_size(off, count);
        traits_type::copy(dest, m_buffer.ptr + off, count);
        return count;
    }

    basic_string substr(size_type off = 0, size_type count = npos) const
    {
        if (!check_offset(off))
        {
            return basic_string();
        }
        count = clamp_suffix_size(off, count);
        return basic_string(m_buffer.ptr + off, count);
    }

    //=========================================================================
    // replace
    //=========================================================================

private:

    template <construct_method M, typename... Args>
    bool replace_capacity(pointer pos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (in_count > out_count)
        {
            const size_type diff = in_count - out_count;
            const pointer back = ptr + size + 1;
            mem::construct_range(back, diff);

            const size_type tail_count = static_cast<size_type>(back - (pos + in_count)) + 1;
            move_batch(pos + in_count, pos, tail_count);

            size += diff;
            capacity -= diff;
        }

        if (in_count < out_count)
        {
            const size_type diff = out_count - in_count;
            const pointer back = ptr + size + 1;
            const size_type tail_count = static_cast<size_type>(back - (pos + out_count)) + 1;
            move_batch(pos + in_count, pos + out_count, tail_count);

            mem::destroy_range(back - diff, diff);

            size -= diff;
            capacity += diff;
        }

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(pos, out_count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(pos, std::forward<Args>(args)..., out_count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(pos, std::forward<Args>(args)...);
        }

        return true;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    bool replace_reallocate(pointer pos, size_type in_count, size_type out_count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        const size_type new_size = size - in_count + out_count;
        const size_type new_capacity = grow_capacity<growth_rate>(new_size, capacity);
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity + 1);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        const size_type off = static_cast<size_type>(pos - ptr);
        pointer dst = new_ptr + off;

        // copy prefix [ptr, ptr + size) to [new_ptr, ...), then construct suffix [ptr + size, ...)
        mem::construct_range(new_ptr, new_ptr + new_size + 1);
        traits_type::copy(new_ptr, ptr, off);

        VX_IF_CONSTEXPR (M == construct_method::from_char)
        {
            traits_type::assign(ptr + size, out_count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::from_pointer)
        {
            copy_batch(ptr + size, std::forward<Args>(args)..., out_count);
        }
        else // VX_IF_CONSTEXPR (M == construct_method::from_iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::from_iterator_range, "invalid tag");
            traits_type::assign_range(ptr + size, std::forward<Args>(args)...);
        }

        // copy second range
        traits_type::copy(dst + out_count, pos + in_count, size - off - in_count);

        // destroy original range
        destroy_and_deallocate(ptr, size, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_capacity;

        return true;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    bool replace_n(pointer pos, size_type in_count, size_type out_count, Args&&... args)
    {
        VX_STATIC_ASSERT_MSG(growth_rate::num >= 0 && growth_rate::den > 0, "Growth rate must be positive");
        VX_STATIC_ASSERT_MSG(growth_rate::num >= growth_rate::den, "Growth rate must be greater or equal to 1");

        const size_type available = m_buffer.capacity - m_buffer.size + in_count;

        if (out_count <= available)
        {
            return replace_capacity<M>(pos, in_count, out_count, std::forward<Args>(args)...);
        }
        else
        {
            return replace_reallocate<growth_rate, M>(pos, in_count, out_count, std::forward<Args>(args)...);
        }
    }

public:

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, size_type count2, const T c)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            return replace_n<growth_rate, construct_method::from_char>(m_buffer.ptr + off, count2, count, c);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const T* const s, size_type count2)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, s);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const T* const s)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            const size_type count2 = static_cast<size_type>(traits_type::length(s));
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, s);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const basic_string_view<T> view)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            const size_type count2 = static_cast<size_type>(view.size());
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, view.data());
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const basic_string_view<T> view, size_type view_off, size_type count2 = npos)
    {
        if (check_offset(off) && view.check_offset(view_off))
        {
            count = clamp_suffix_size(off, count);
            count2 = static_cast<size_type>(view.clamp_suffix_size(view_off, count2));
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, view.data() + view_off);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const basic_string& other)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, other.size(), count, other.data());
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(size_type off, size_type count, const basic_string& other, size_type other_off, size_type count2 = npos)
    {
        if (check_offset(off) && other.check_offset(other_off))
        {
            count = clamp_suffix_size(off, count);
            count2 = static_cast<size_type>(other.clamp_suffix_size(other_off, count2));
            return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, other.data() + other_off);
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& replace(size_type off, size_type count, IT first, IT last)
    {
        if (check_offset(off))
        {
            count = clamp_suffix_size(off, count);
            const size_type count2 = static_cast<size_type>(std::distance(first, last));
            VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
            {
                return replace_n<growth_rate, construct_method::from_pointer>(m_buffer.ptr + off, count2, count, first.ptr());
            }
            else
            {
                return replace_n<growth_rate, construct_method::from_iterator_range>(m_buffer.ptr + off, count2, count, first, last);
            }
        }
        return *this;
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, size_type count2, const T c)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        return replace_n<growth_rate, construct_method::from_char>(first.ptr(), in_count, count2, c);
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const T* const s)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(traits_type::length(s));
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, s);
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const T* const s, size_type count2)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, s);
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const basic_string_view<T> view)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(view.size());
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, view.data());
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const basic_string_view<T> view, size_type view_off, size_type count2 = npos)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        if (!view.check_offset(view_off))
        {
            return *this;
        }
        count2 = static_cast<size_type>(view.clamp_suffix_size(view_off, count2));
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, view.data() + view_off);
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const basic_string& other)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, other.size(), other.data());
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, const basic_string& other, size_type other_off, size_type count2 = npos)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        if (!other.check_offset(other_off))
        {
            return *this;
        }
        count2 = static_cast<size_type>(other.clamp_suffix_size(other_off, count2));
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, other.data() + other_off);
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    basic_string& replace(const_iterator first, const_iterator last, IT first2, IT last2)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(std::distance(first2, last2));

        VX_IF_CONSTEXPR (_priv::is_pointer_iterator<IT>::value)
        {
            return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, first2.ptr());
        }
        else
        {
            return replace_n<growth_rate, construct_method::from_iterator_range>(first.ptr(), in_count, count2, first2, last2);
        }
    }

    template <typename growth_rate = default_growth_rate>
    basic_string& replace(const_iterator first, const_iterator last, std::initializer_list<T> init)
    {
        const size_type in_count = static_cast<size_type>(std::distance(first, last));
        const size_type count2 = static_cast<size_type>(init.size());
        return replace_n<growth_rate, construct_method::from_pointer>(first.ptr(), in_count, count2, init.begin());
    }

    //=========================================================================
    // searching
    //=========================================================================

    bool contains(const T c) const noexcept
    {
        return find(c) != npos;
    }

    bool contains(const basic_string& other) const noexcept
    {
        return find(other) != npos;
    }

    bool contains(const T* const s) const noexcept
    {
        return find(s) != npos;
    }

    //=========================================================================

    size_type find(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type find(const basic_string_view<T> view, size_type off = 0) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_find<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================

    size_type rfind(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_rfind_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type rfind(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type rfind(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type rfind(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type rfind(const basic_string_view<T> view, size_type off = npos) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_rfind<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================

    size_type find_first_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_first_of(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_first_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_first_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type find_first_of(const basic_string_view<T> view, size_type off = 0) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_find_first_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================

    size_type find_last_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_rfind_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_last_of(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_last_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_last_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type find_last_of(const basic_string_view<T> view, size_type off = npos) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_find_last_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================

    size_type find_first_not_of(const T c, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_not_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_first_not_of(const basic_string& other, size_type off = 0) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_first_not_of(const T* const s, size_type off = 0) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_first_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type find_first_not_of(const basic_string_view<T> view, size_type off = 0) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_find_first_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================

    size_type find_last_not_of(const T c, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_rfind_not_ch<traits_type>(
            m_buffer.ptr, m_buffer.size, off, c));
    }

    size_type find_last_not_of(const basic_string& other, size_type off = npos) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, other.data(), other.size()));
    }

    size_type find_last_not_of(const T* const s, size_type off = npos) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return static_cast<size_type>(str::_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, s_len));
    }

    size_type find_last_not_of(const T* const s, size_type off, size_type count) const noexcept
    {
        return static_cast<size_type>(str::_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, s, count));
    }

    size_type find_last_not_of(const basic_string_view<T> view, size_type off = npos) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return static_cast<size_type>(str::_priv::traits_find_last_not_of<traits_type>(
            m_buffer.ptr, m_buffer.size, off, view.data(), view_size));
    }

    //=========================================================================
    // comparison
    //=========================================================================

    int compare(const basic_string& other) const noexcept
    {
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            other.data(), other.size());
    }

    int compare(const T* const s) const noexcept
    {
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            s, s_len);
    }

    int compare(size_type off, size_type count, const T* const s) const noexcept
    {
        if (!check_offset(off))
        {
            return 1;
        }
        count = clamp_suffix_size(off, count);
        const size_type s_len = static_cast<size_type>(traits_type::length(s));
        return compare(off, count, s, s_len);
    }

    int compare(const basic_string_view<T> view) const noexcept
    {
        const size_type view_size = static_cast<size_type>(view.size());
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr, m_buffer.size,
            view.data(), view_size);
    }

    int compare(size_type off1, size_type count1, const basic_string_view<T> view, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!check_offset(off1) || !view.check_offset(off2))
        {
            return 1;
        }
        count1 = clamp_suffix_size(off1, count1);
        count2 = static_cast<size_type>(view.clamp_suffix_size(off2, count2));
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr + off1, count1,
            view.data() + off2, count2);
    }

    int compare(size_type off, size_type count, const basic_string& other) const noexcept
    {
        if (!check_offset(off))
        {
            return 1;
        }
        count = clamp_suffix_size(off, count);
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr + off, count,
            other.data(), other.size());
    }

    int compare(size_type off1, size_type count1, const basic_string& other, size_type off2, size_type count2 = npos) const noexcept
    {
        if (!check_offset(off1) || !other.check_offset(off2))
        {
            return 1;
        }
        count1 = clamp_suffix_size(off1, count1);
        count2 = other.clamp_suffix_size(off2, count2);
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr + off1, count1,
            other.data() + off2, count2);
    }

    int compare(size_type off, size_type count1, const T* const s, size_type count2) const noexcept
    {
        if (!check_offset(off))
        {
            return 1;
        }
        count1 = clamp_suffix_size(off, count1);
        return str::_priv::traits_compare<traits_type>(
            m_buffer.ptr + off, count1,
            s, count2);
    }

    //=========================================================================

    friend bool operator==(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    friend bool operator==(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    friend bool operator==(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    friend bool operator!=(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    friend bool operator!=(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    friend bool operator!=(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    friend bool operator<(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend bool operator<(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend bool operator<(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    friend bool operator>(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    friend bool operator>(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    friend bool operator>(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    friend bool operator<=(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    friend bool operator<=(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    friend bool operator<=(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    friend bool operator>=(const basic_string& lhs, const basic_string& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    friend bool operator>=(const basic_string& lhs, const T* const rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    friend bool operator>=(const T* const lhs, const basic_string& rhs) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }
};

//=========================================================================

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
#if defined(__cpp_lib_char8_t)
using u8string = basic_string<char8_t>;
#endif // defined(__cpp_lib_char8_t)
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

} // namespace vx

//namespace std {
//
//template <typename T>
//struct hash<vx::basic_string<T>>
//{
//    size_t operator()(const vx::basic_string<T> sv) const noexcept
//    {
//        return vx::str::_priv::traits_hash<typename vx::basic_string<T>::traits_type>(sv.data(), sv.size());
//    }
//};
//
//} // namespace std
