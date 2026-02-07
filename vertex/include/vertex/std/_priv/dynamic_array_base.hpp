#pragma once

#include <cstdint>
#include <initializer_list>
#include <ratio>
#include <utility>

#include "vertex/config/language_config.hpp"
#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace _priv {

//#define VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK

// Testing shows that allocating memory aligned to the element type is often faster than using no explicit alignment.
// Using an optimal alignment can provide additional speedups by taking advantage of vectorization. Any alignment
// introduces a small amount of wasted memory at the start of the allocation. In typical use cases the performance
// gains are negligible, but for large vectors aligned memory is beneficial, as it enables more effective SIMD
// vectorization.

template <typename T, typename Allocator = mem::default_allocator<T>>
class dynamic_array_base
{
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

    using iterator = pointer_iterator<dynamic_array_base, T*>;
    using const_iterator = pointer_iterator<dynamic_array_base, const T*>;
    using reverse_iterator = reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = reverse_pointer_iterator<const_iterator>;

    enum class construct_method
    {
        single,             // construct a single value
        default_range,      // construct from size
        fill_range,         // fill range
        copy_range,         // copy range (no overlap)
        move_range,         // move range (no overlap)
        iterator_range      // construct from iterator range
    };

    struct buffer
    {
        pointer ptr;
        size_type size;
        size_type capacity;
    };

    //=========================================================================
    // construction helpers
    //=========================================================================

    buffer release_buffer() noexcept
    {
        auto& b = m_buffer;
        auto& ptr = b.ptr;
        auto& size = b.size;
        auto& capacity = b.capacity;

        buffer old_buffer = std::move(b);

        ptr = nullptr;
        size = 0;
        capacity = 0;

        return old_buffer;
    }

    template <construct_method M, typename... Args>
    inline void construct_n(size_type count, Args&&... args)
    {
        VX_UNLIKELY_COLD_PATH(!count,
            {
                return;
            });

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(count > max_size(),
            {
                err::set(err::size_error);
                return;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        auto new_ptr = allocator_type::allocate(count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        VX_IF_CONSTEXPR (M == construct_method::default_range)
        {
            mem::construct_range(new_ptr, count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_uninitialized_range(new_ptr, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            // move the range out of the source, ranges will never overlap since we just allocated out memory
            mem::move_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            // copy elements from the source to my vector, memcpy is safe
            mem::copy_uninitialized_range(new_ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_uninitialized_range(new_ptr, std::forward<Args>(args)...);
        }

        m_buffer.ptr = new_ptr;
        m_buffer.size = count;
        m_buffer.capacity = count;
    }

    //=========================================================================
    // constructors
    //=========================================================================

    dynamic_array_base() noexcept
    {}

    dynamic_array_base(size_type count)
    {
        construct_n<construct_method::default_range>(count);
    }

    dynamic_array_base(const size_type count, const T& value)
    {
        construct_n<construct_method::fill_range>(count, value);
    }

    dynamic_array_base(std::initializer_list<T> init)
    {
        construct_n<construct_method::copy_range>(init.size(), init.begin());
    }

    dynamic_array_base(const dynamic_array_base& other)
    {
        construct_n<construct_method::copy_range>(
            other.m_buffer.size,
            other.m_buffer.ptr);
    }

    dynamic_array_base(dynamic_array_base&& other) noexcept
        : m_buffer(other.release_buffer())
    {}

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    dynamic_array_base(IT first, IT last) noexcept
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (is_pointer_iterator<IT>::value)
        {
            construct_n<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            construct_n<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }
    }

    //=========================================================================
    // destructor helpers
    //=========================================================================

    void destroy_range()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        if (ptr)
        {
            mem::destroy_range(ptr, size);
            allocator_type::deallocate(ptr, capacity);
        }

        ptr = nullptr;
        size = 0;
        capacity = 0;
    }

    //=========================================================================
    // destructor
    //=========================================================================

    ~dynamic_array_base()
    {
        destroy_range();
    }

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
                return false;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        if (count > capacity)
        {
            pointer new_ptr = allocator_type::allocate(count);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            VX_UNLIKELY_COLD_PATH(!new_ptr,
                {
                    return false;
                });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            mem::destroy_range(ptr, size);
            allocator_type::deallocate(ptr, capacity);
            ptr = new_ptr;
            capacity = count;
        }
        else
        {
            mem::destroy_range(ptr, size);
        }

        VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(new_ptr, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_range(ptr, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_range(ptr, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(ptr, std::forward<Args>(args)...);
        }

        size = count;
        return true;
    }

    //=========================================================================
    // assignment operators
    //=========================================================================

    dynamic_array_base& operator=(const dynamic_array_base& other)
    {
        if (this != std::addressof(other))
        {
            assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
        }

        return *this;
    }

    dynamic_array_base& operator=(dynamic_array_base&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            destroy_range();
            m_buffer = other.release_buffer();
        }

        return *this;
    }

    dynamic_array_base& operator=(std::initializer_list<T> init)
    {
        assign_from<construct_method::copy_range>(init.size(), init.begin());
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    bool assign(const dynamic_array_base& other)
    {
        if (this == std::addressof(other))
        {
            return true;
        }

        return assign_from<construct_method::copy_range>(other.m_buffer.size, other.m_buffer.ptr);
    }

    bool assign(dynamic_array_base&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            destroy_range();
            m_buffer = other.release_buffer();
        }

        return true;
    }

    bool assign(std::initializer_list<T> init)
    {
        return assign_from<construct_method::copy_range>(init.size(), init.begin());
    }

    bool assign(const T* ptr, size_type count)
    {
        return assign_from<construct_method::copy_range>(count, ptr);
    }

    bool assign(size_type count, const T& value)
    {
        return assign_from<construct_method::fill_range>(count, value);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    bool assign(IT first, IT last)
    {
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (is_pointer_iterator<IT>::value)
        {
            return assign_from<construct_method::copy_range>(count, first.ptr());
        }
        else
        {
            return assign_from<construct_method::iterator_range>(count, std::move(first), std::move(last));
        }
    }

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return *m_buffer.ptr;
    }

    const T& front() const noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return *m_buffer.ptr;
    }

    T& back() noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
        return m_buffer.ptr[m_buffer.size - 1];
    }

    const T& back() const noexcept
    {
        VX_ASSERT(m_buffer.ptr && m_buffer.size);
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

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        mem::destroy_range(m_buffer.ptr, m_buffer.size);
        m_buffer.size = 0;
    }

    void clear_and_deallocate()
    {
        destroy_range();
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
        const buffer old_buffer = release_buffer();
        return old_buffer.ptr;
    }

    bool acquire(T* ptr, size_type count) noexcept
    {
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

    void swap(dynamic_array_base& other) noexcept
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
        return m_buffer.size == 0;
    }

    size_type size() const noexcept
    {
        return m_buffer.size;
    }

    size_type data_size() const noexcept
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size() noexcept
    {
        return mem::max_array_size<T>();
    }

    //=========================================================================
    // capacity
    //=========================================================================

    size_type capacity() const noexcept
    {
        return m_buffer.capacity;
    }

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

        pointer new_ptr;

        VX_IF_CONSTEXPR (try_reallocate && std::is_trivially_destructible<T>::value && std::is_trivially_copyable<T>::value)
        {
            new_ptr = allocator_type::reallocate(ptr, new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return false;
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)
        }
        else
        {
            new_ptr = allocator_type::allocate(new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

            if (!new_ptr)
            {
                return false;
            }

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

            VX_IF_CONSTEXPR (shrinking)
            {
                VX_ASSERT(size > 0);
                mem::move_uninitialized_range(new_ptr, ptr, new_capacity);
            }
            else
            {
                mem::move_uninitialized_range(new_ptr, ptr, size);
            }

            mem::destroy_range(ptr, size);
            allocator_type::deallocate(ptr, capacity);
        }

        ptr = new_ptr;
        VX_IF_CONSTEXPR (shrinking)
        {
            size = new_capacity;
        }
        capacity = new_capacity;

        return true;
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

    template <typename... Args>
    bool resize_reallocate(const size_type new_size, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

#if !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        VX_UNLIKELY_COLD_PATH(new_size > max_size(),
            {
                err::set(err::size_error);
                return false;
            });

#endif // !defined(VX_DYNAMIC_ARRAY_DISABLE_MAX_SIZE_CHECK)

        pointer new_ptr = allocator_type::allocate(new_size);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return false;
            });

#endif // !defined(VX_ALLOCATE_FAIL_FAST)

        const size_type grow_count = new_size - size;
        pointer end_ptr = new_ptr + size;

        VX_IF_CONSTEXPR (sizeof...(Args) == 0)
        {
            mem::construct_range(end_ptr, grow_count);
        }
        else // VX_IF_CONSTEXPR(sizeof...(Args) == 1)
        {
            VX_STATIC_ASSERT_MSG(sizeof...(Args) == 1, "Invalid arguments");
            mem::fill_uninitialized_range(end_ptr, grow_count, std::forward<Args>(args)...);
        }

        mem::move_uninitialized_range(new_ptr, ptr, size);

        mem::destroy_range(ptr, size);
        allocator_type::deallocate(ptr, capacity);

        ptr = new_ptr;
        size = new_size;
        capacity = new_size;

        return true;
    }

    template <typename... Args>
    bool resize_impl(const size_type new_size, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        // trim
        if (new_size < size)
        {
            const size_type shrink_count = size - new_size;
            pointer end_ptr = ptr + new_size;
            mem::destroy_range(end_ptr, shrink_count);
        }

        if (new_size > size)
        {
            if (new_size > capacity)
            {
                // seems to be slightly faster to call this rather than inlining
                return resize_reallocate(new_size, std::forward<Args>(args)...);
            }
            else
            {
                const size_type grow_count = new_size - size;
                pointer end_ptr = ptr + size;

                VX_IF_CONSTEXPR (sizeof...(Args) == 0)
                {
                    mem::construct_range(end_ptr, grow_count);
                }
                else // VX_IF_CONSTEXPR(sizeof...(Args) == 1)
                {
                    VX_STATIC_ASSERT_MSG(sizeof...(Args) == 1, "Invalid arguments");
                    mem::fill_uninitialized_range(end_ptr, grow_count, std::forward<Args>(args)...);
                }
            }
        }

        size = new_size;
        return true;
    }

    template <typename U>
    bool resize(const size_type count, const U& value)
    {
        return resize_impl(count, value);
    }

    bool resize(const size_type count)
    {
        return resize_impl(count);
    }

    //=========================================================================
    // insert
    //=========================================================================

    template <construct_method M, typename... Args>
    pointer insert_capacity(pointer pos, size_type count, Args&&... args)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        pointer back = ptr + size;

        if (pos != back)
        {
            // move the values that will spill over into uninitialized memory
            pointer src = back - count;
            mem::move_uninitialized_range(back, src, count);

            // move the values that will be moved into already initialized memory

            VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
            {
                const size_type off = static_cast<size_type>(pos - ptr);
                const size_type tail_count = size - off - count;
                // pointer point to the last element in the range, so adjust
                mem::move(pos + count, pos, tail_count * sizeof(T));
            }
            else
            {
                --back;
                --src;

                while (back != pos)
                {
                    *back = std::move(*src);
                    --src;
                    --back;
                }
            }
        }

        VX_IF_CONSTEXPR (M == construct_method::single)
        {
            *pos = T(std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(pos, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_range(pos, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_range(pos, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(pos, std::forward<Args>(args)...);
        }

        size += count;
        return pos;
    }

    // reallocate the vector data, split at pos and shift back by shift
    // caller should ensure
    template <typename growth_rate, construct_method M, typename... Args>
    pointer insert_reallocate(pointer pos, size_type count, Args&&... args) noexcept
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

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type new_capacity = grow_capacity<growth_rate>(size + count, capacity);
        VX_ASSERT(new_capacity > capacity);

        pointer new_ptr = allocator_type::allocate(new_capacity);

#if !defined(VX_ALLOCATE_FAIL_FAST)

        VX_UNLIKELY_COLD_PATH(!new_ptr,
            {
                return nullptr;
            });

#endif // defined(VX_ALLOCATE_FAIL_FAST)

        pointer dst = new_ptr + off;

        VX_IF_CONSTEXPR (M == construct_method::single)
        {
            mem::construct_in_place(dst, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::fill_range)
        {
            mem::fill_range(dst, count, std::forward<Args>(args)...);
        }
        else VX_IF_CONSTEXPR (M == construct_method::move_range)
        {
            mem::move_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else VX_IF_CONSTEXPR (M == construct_method::copy_range)
        {
            mem::copy_uninitialized_range(dst, std::forward<Args>(args)..., count);
        }
        else // VX_IF_CONSTEXPR(M == construct_method::iterator_range)
        {
            VX_STATIC_ASSERT_MSG(M == construct_method::iterator_range, "invalid tag");
            mem::copy_range(dst, std::forward<Args>(args)...);
        }

        // copy first range
        mem::move_uninitialized_range(new_ptr, ptr, off);
        // copy second range
        mem::move_uninitialized_range(dst + count, pos, size - off);

        // destroy original range
        mem::destroy_range(ptr, size);
        allocator_type::deallocate(ptr, capacity);

        ptr = new_ptr;
        size += count;
        capacity = new_capacity;

        return dst;
    }

    template <typename growth_rate, construct_method M, typename... Args>
    pointer insert_n(pointer pos, size_type count, Args&&... args)
    {
        const size_type available = m_buffer.capacity - m_buffer.size;

        if (count <= available)
        {
            return insert_capacity<Tag>(pos, count, std::forward<Args>(args)...);
        }
        else
        {
            return insert_reallocate<growth_rate, Tag>(pos, count, std::forward<Args>(args)...);
        }
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T& value)
    {
        return emplace<growth_rate>(pos, value);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, T&& value) noexcept
    {
        return emplace<growth_rate>(pos, std::move(value));
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<growth_rate, construct_method::fill_range>(ptr, count, value);
        return iterator(ptr);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<growth_rate, construct_method::copy_range>(ptr, init.size(), init.begin());
        return iterator(ptr);
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        const size_type count = static_cast<size_type>(std::distance(first, last));

        VX_IF_CONSTEXPR (is_pointer_iterator<IT>::value)
        {
            ptr = insert_n<growth_rate, construct_method::copy_range>(ptr, count, first.ptr());
        }
        else
        {
            ptr = insert_n<growth_rate, construct_method::iterator_range>(ptr, count, first, last);
        }

        return iterator(ptr);
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename growth_rate = default_growth_rate, typename... Args>
    pointer emplace_back(Args&&... args)
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
                    return nullptr;
                });
        }

        pointer dst = ptr + size;
        mem::construct_in_place(dst, std::forward<Args>(args)...);
        ++size;

        return dst;
    }

    template <typename growth_rate = default_growth_rate, typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = insert_n<growth_rate, construct_method::single>(ptr, 1, std::forward<Args>(args)...);
        return iterator(ptr);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    void push_back(const T& value)
    {
        emplace_back<growth_rate>(value);
    }

    template <typename growth_rate = default_growth_rate>
    void push_back(T&& value) noexcept
    {
        emplace_back<growth_rate>(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

    pointer erase_n(pointer pos, size_type count)
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;
        auto& capacity = m_buffer.capacity;

        const size_type off = static_cast<size_type>(pos - ptr);
        const size_type tail_count = size - off - count;
        const size_type new_size = size - count;

        mem::move_range(pos, pos + count, tail_count);
        mem::destroy_range(ptr + new_size, count);

        size = new_size;
        capacity += count;

        return pos;
    }

    iterator erase(const_iterator pos)
    {
        auto ptr = const_cast<pointer>(pos.ptr());
        ptr = erase_n(ptr, 1);
        return iterator(ptr);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        auto ptr = const_cast<pointer>(first.ptr());
        const size_type count = static_cast<size_type>(last.ptr() - first.ptr());
        ptr = erase_n(ptr, count);
        return iterator(ptr);
    }

    void pop_back()
    {
        auto& ptr = m_buffer.ptr;
        auto& size = m_buffer.size;

        if (size)
        {
            mem::destroy_in_place(ptr + size - 1);
            --size;
        }
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        if (lhs.m_buffer.size != rhs.m_buffer.size)
        {
            return false;
        }

        return mem::equal_range(lhs.m_buffer.ptr, rhs.m_buffer.ptr, lhs.m_buffer.size);
    }

    friend bool operator!=(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        return !operator==(lhs, rhs);
    }

    friend bool operator<(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        const size_type min_size = lhs.m_buffer.size < rhs.m_buffer.size ? lhs.m_buffer.size : rhs.m_buffer.size;

        if (!mem::equal_range(lhs.m_buffer.ptr, rhs.m_buffer.ptr, min_size))
        {
            return mem::less_range(lhs.m_buffer.ptr, rhs.m_buffer.ptr, min_size);
        }

        return lhs.m_buffer.size < rhs.m_buffer.size;
    }

    friend bool operator>(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const dynamic_array_base& lhs, const dynamic_array_base& rhs)
    {
        return !(lhs < rhs);
    }

    buffer m_buffer = {};
};

} // namespace _priv
} // namespace vx
